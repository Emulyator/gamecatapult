#ifndef _GCTP_ZFILTER_HPP_
#define _GCTP_ZFILTER_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * zlibファイルバッファ・ストリームフィルタークラスヘッダファイル
 *
 * ランダムアクセスには対応していないので、seekしてはいけない。
 * 
 @code
	// 圧縮
    {
        std::ifstream ifs("test.txt",std::ios::binary);

        std::filebuf ofs;
		ofs.open("test.out",std::ios::out|std::ios::binary|std::ios::trunc);
        gctp::zfilter ozf(ofs,std::ios::out);
        std::ostream ofs(ozf);

        int c;
        while((c=ifs.get())!=EOF){
            ofs << (char)c;
        }
    }

    // 伸長
    {
        std::filebuf ifs;
		ifs.open("test.out",std::ios::in|std::ios::binary);
        gctp::zfilter izf(ifs,std::ios::in);
        std::ifstream ifs(izf);

        for(;;){
            std::vector<char> v(128);
            ifs.read(v.begin(),v.size());
            if(ifs.gcount()==0){
                break;
            }
            std::cout << std::string(v.begin(),ifs.gcount());
        }
    }
 @endcode
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/09 23:00:42
 *
 * このクラスは平山直之氏の記事を参考にかかれました。\n
 * http://www.jah.ne.jp/~naoyuki/Writings/ExtIos.html
 *
 * C) 1999 Naoyuki Hirayama. All rights reserved.
 */
#ifdef GCTP_USE_ZLIB
#include <ios> // for std::iostream
#include <vector> // for in/deflate buffer
//#include <gctp/dbgout.hpp>

namespace gctp {

	struct ZStreamImpl;
	class ZStream {
	public:
		enum Status {
			STATUS_OK,
			STATUS_END,
		};
		enum FlushMode {
			FLUSHMODE_NONE,
			FLUSHMODE_FINISH,
		};
		ZStream();
		ZStream(const ZStream &); // not implement
		~ZStream();
		bool open(bool infrate);
		void close();
		Status process();
		void setInput(unsigned char *p, unsigned int s);
		void setOutput(unsigned char *p, unsigned int s);
		void setFlushMode(FlushMode mode);
		FlushMode getFlushMode();
		unsigned int inputAvail();
		unsigned int outputAvail();
	private:
		ZStreamImpl *impl_;
	};

	/// zlibのストリームバッファフィルタークラス
	template< int Buf_Size, class Ch, class Tr=std::char_traits<Ch> >
	class basic_zfilter : public std::basic_streambuf<Ch,Tr> {
	public:
		enum { BUFFER_SIZE = Buf_Size };
		typedef std::basic_streambuf<Ch,Tr> Super;

	public:
		basic_zfilter *open(Super *streambuf, std::ios::open_mode mode = std::ios::in|std::ios::out)
		{
			mode_ = mode;
			deflated_.resize(BUFFER_SIZE);
			if(mode_ & std::ios::in) {
				if(!inflate_stream_.open(true)) return 0;
			}
			if(mode_ & std::ios::out) {
				if(!deflate_stream_.open(false)) return 0;
			}
			
			setbuf(0, BUFFER_SIZE);
			streambuf_ = streambuf;
			return this;
		}

		void close()
		{
			inflate_stream_.setFlushMode(ZStream::FLUSHMODE_FINISH);
			deflate_stream_.setFlushMode(ZStream::FLUSHMODE_FINISH);
			sync();

			if(mode_ & std::ios::in){
				inflate_stream_.close();
			}
			if(mode_ & std::ios::out){
				deflate_stream_.close();
			}
			streambuf_ = 0;
			raw_.resize(0);
			deflated_.resize(0);
		}

		bool is_open()
		{
			return streambuf_ ? true : false;
		}

		basic_zfilter(Super *streambuf, std::ios::open_mode mode = std::ios::in|std::ios::out) : Super(), buffer_(0)
		{
			open(streambuf, mode);
		}

		~basic_zfilter()
		{
			close();
		}

	protected:
		virtual std::basic_streambuf<Ch,Tr> *setbuf(Ch *b, std::streamsize s)
		{
			if(buffer_) sync();
			if(b) {
				buffer_	= b;
				size_ = s;
				raw_.resize(0);
			}
			else {
				raw_.resize(s);
				buffer_	= &raw_.front();
				size_ = s;
			}
			if(mode_ & std::ios::in) setg(buffer_,buffer_+size_,buffer_+size_);
			if(mode_ & std::ios::out) setp(buffer_,buffer_+size_);
			return this;
		}

		virtual int_type overflow(int_type c=Tr::eof())
		{
			compress();
			if(c!=Tr::eof()) {
				*pptr() = Tr::to_char_type(c);
				pbump(1);
				return Tr::not_eof(c);
			} else {
				return Tr::eof();
			}
		}

		virtual int_type underflow()
		{
			if(egptr()<=gptr()) {
				if(inflate_stream_.getFlushMode()==ZStream::FLUSHMODE_FINISH) {
					return Tr::eof();
				}
				decompress();
				if(egptr()<=gptr()) {
					return Tr::eof();
				}
			}
			return Tr::to_int_type(*gptr());
		}

		virtual int sync()
		{
			if(mode_ & std::ios::out){
				compress();
				//if(pptr() > buffer_) compress();
			}
			return 0;
		}

		virtual pos_type seekoff(off_type type, std::ios_base::seekdir dir, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
		{
			// アタッチしているストリームへの操作とみなす
			return streambuf_->pubseekoff(type, dir, mode);
		}

		virtual pos_type seekpos(pos_type type, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
		{
			// アタッチしているストリームへの操作とみなす
			return streambuf_->pubseekpos(type, mode);
		}

		virtual void imbue(const std::locale &loc)
		{
			// アタッチしているストリームへの操作とみなす
			streambuf_->pubimbue(loc);
		}

		void compress()
		{
			ZStream &zs = deflate_stream_;
			zs.setInput((unsigned char *)buffer_, (unsigned int)((pptr()-buffer_)*sizeof(Ch)));
			zs.setOutput((unsigned char *)&deflated_.front(), (unsigned int)(deflated_.size()*sizeof(Ch)));
			for(;;) {
				if(ZStream::STATUS_END == zs.process()) {
					// 完了
					streambuf_->sputn(&deflated_.front(), (std::streamsize)(deflated_.size()-zs.outputAvail()/sizeof(Ch)));
					//PRNN(deflated_.size() << ",0 " << zs.outputAvail());
					return;
				}
				if(zs.outputAvail()==0) {
					// 出力バッファが尽きた
					streambuf_->sputn(&deflated_.front(), (std::streamsize)deflated_.size());
					//PRNN(deflated_.size() << ",2 " << zs.outputAvail());
					zs.setOutput((unsigned char*)&deflated_.front(), (unsigned int)(deflated_.size()*sizeof(Ch)));
					continue;
				}
				if(zs.inputAvail()==0) {
					// 入力バッファが尽きた
					streambuf_->sputn(&deflated_.front(), (std::streamsize)(deflated_.size()-zs.outputAvail()/sizeof(Ch)));
					//PRNN(deflated_.size() << ",1 " << zs.outputAvail());
					setp(buffer_, buffer_+size_);
					return;
				}
			}
		}

		void decompress()
		{
			ZStream &zs = inflate_stream_;
			zs.setOutput((unsigned char*)buffer_, size_*sizeof(Ch));

			for(;;){
				if(zs.inputAvail() == 0){
					// 入力バッファが尽きた
					zs.setInput((unsigned char*)&deflated_.front(), (unsigned int)(streambuf_->sgetn(&deflated_.front(), deflated_.size())*sizeof(Ch)));
					if(zs.inputAvail() == 0){
						zs.setFlushMode(ZStream::FLUSHMODE_FINISH);
					}
				}
				if(zs.outputAvail() == 0){
					// 出力バッファが尽きた
					setg(buffer_, buffer_, buffer_+size_);
					return;
				}
				if(ZStream::STATUS_END==zs.process()){
					// 完了
					setg(buffer_, buffer_, buffer_+size_-zs.outputAvail());
					return;
				}
			}
		}

	private:
		Super *streambuf_;
		std::ios::open_mode mode_;
		
		Ch *buffer_;
		std::streamsize size_;

		std::vector<Ch> raw_;
		std::vector<Ch> deflated_;

		ZStream inflate_stream_;
		ZStream deflate_stream_;
	};

	typedef basic_zfilter<1024*4, char> zfilter;
	typedef basic_zfilter<1024*2, wchar_t> wzfilter; // wchar_tは動かないかも
}
#endif // GCTP_USE_ZLIB

#endif // _GCTP_ZFILTER_HPP_
