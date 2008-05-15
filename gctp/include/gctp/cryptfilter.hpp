#ifndef _GCTP_CRYPTFILTER_HPP_
#define _GCTP_CRYPTFILTER_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * Cryptファイルバッファ・ストリームフィルタークラスヘッダファイル
 *
 * ランダムアクセスには対応していないので、seekしてはいけない。
 * 
 @code
	// 暗号化
    {
        std::ifstream ifs("test.txt",std::ios::binary);

        std::filebuf ofs;
		ofs.open("test.out",std::ios::out|std::ios::binary|std::ios::trunc);
        gctp::cryptfilter ocf(&ofs, std::ios::out);
        std::ostream ofs(&ocf);

        int c;
        while((c=ifs.get())!=EOF){
            ofs << (char)c;
        }
    }

    // 復号
    {
        std::filebuf ifs;
		ifs.open("test.out",std::ios::in|std::ios::binary);
        gctp::cryptfilter icf(&ifs, std::ios::in);
        std::ifstream ifs(&icf);

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
 */
#include <ios> // for std::iostream
#include <vector> // for in/deflate buffer
//#include <gctp/dbgout.hpp>
#include <gctp/crypt.hpp>

namespace gctp {

	/// Cryptストリームバッファフィルタークラス
	template< int Buf_Size, class Ch, class Tr=std::char_traits<Ch> >
	class basic_cryptfilter : public std::basic_streambuf<Ch,Tr> {
	public:
		enum { BUFFER_SIZE = Buf_Size };
		typedef std::basic_streambuf<Ch,Tr> Super;

	public:
		basic_cryptfilter(Crypt &crypt) : buffer_(0), crypt_(crypt) {}

		basic_cryptfilter(Crypt &crypt, Super *streambuf, std::ios::open_mode mode = std::ios::in|std::ios::out) : buffer_(0), crypt_(crypt)
		{
			open(streambuf, mode);
		}

		~basic_cryptfilter()
		{
			close();
		}

		basic_cryptfilter *open(Super *streambuf, std::ios::open_mode mode = std::ios::in|std::ios::out)
		{
			mode_ = mode;
			setbuf(0, BUFFER_SIZE);
			streambuf_ = streambuf;
			crypt_pos_ = buffer_pos_ = streambuf_->pubseekoff(0, std::ios_base::cur, mode_);
			return this;
		}

		void close()
		{
			if(streambuf_) sync();
			streambuf_ = 0;
			buf_.resize(0);
		}

		bool is_open()
		{
			return streambuf_ ? true : false;
		}

	protected:
		virtual std::basic_streambuf<Ch,Tr> *setbuf(Ch *b, std::streamsize s)
		{
			if(buffer_) sync();
			if(b) {
				buf_.resize(0);
				buffer_	= b;
				size_ = ((s*sizeof(Ch))&~7)/sizeof(Ch);
			}
			else {
				buf_.resize((s*sizeof(Ch))>>3);
				buffer_	= (Ch *)&buf_.front();
				size_ = (std::streamsize)buf_.size()*sizeof(Crypt::DWord)/sizeof(Ch);
			}
			if(mode_ & std::ios::in) setg(buffer_, buffer_, buffer_);
			if(mode_ & std::ios::out) setp(buffer_, buffer_+size_);
			return this;
		}

		virtual int_type overflow(int_type c=Tr::eof())
		{
			encode();
			if(c!=Tr::eof()) {
				*pptr() = Tr::to_char_type(c);
				pbump(1);
				return Tr::not_eof(c);
			}
			return Tr::eof();
		}

		virtual int_type underflow()
		{
			if(egptr()<=gptr()) {
				if(!decode()) return Tr::eof();
				if(egptr()<=gptr()) {
					return Tr::eof();
				}
			}
			return Tr::to_int_type(*gptr());
		}

		virtual int sync()
		{
			if(mode_ & std::ios::out) encode();
			return 0;
		}

		virtual pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
		{
			// アタッチしているストリームへの操作とみなす
			if(dir == std::ios_base::beg) {
				return seekpos((pos_type)off, mode);
			}
			else if(dir == std::ios_base::end) {
				return seekpos(streambuf_->pubseekoff(off, dir, mode), mode);
			}
			else {
				if(mode & std::ios_base::in) {
					if(off == 0) return buffer_pos_+(off_type)(gptr()-buffer_);
					else return seekpos(buffer_pos_+(off_type)(gptr()-buffer_)+off, mode);
				}
				else {
					if(off == 0) return buffer_pos_+(off_type)(pptr()-buffer_);
					else return seekpos(buffer_pos_+(off_type)(pptr()-buffer_)+off, mode);
				}
			}
		}

		virtual pos_type seekpos(pos_type pos, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
		{
			// ８バイト単位で動かし、あまりは読む
			buffer_pos_ = streambuf_->pubseekpos(((pos-crypt_pos_)&~7)+crypt_pos_, mode);
			if(mode & std::ios_base::in) {
				decode();
				pbump(pos-buffer_pos_);
			}
			return buffer_pos_+(off_type)(pptr()-buffer_);
		}

		virtual void imbue(const std::locale &loc)
		{
			// アタッチしているストリームへの操作とみなす
			streambuf_->pubimbue(loc);
		}

		void encode()
		{
			std::streamsize l = (std::streamsize)(pptr()-buffer_);
			if(l > 0) {
				if((l*sizeof(Ch))&7) memset(buffer_+l, 0, 8-((l*sizeof(Ch))&7));
				crypt_.encode(buffer_, (l*sizeof(Ch)+7)&~7);
				streambuf_->sputn(buffer_, (l*sizeof(Ch)+7)&~7);
				buffer_pos_ = streambuf_->pubseekoff(0, std::ios_base::cur, std::ios_base::out);
				setp(buffer_, buffer_+size_);
			}
		}

		bool decode()
		{
			buffer_pos_ = streambuf_->pubseekoff(0, std::ios_base::cur, std::ios_base::in);
			std::streamsize l = streambuf_->sgetn(buffer_, (size_&~7)/sizeof(Ch));
			if(l > 0) {
				if((l*sizeof(Ch))&7) memset(buffer_+l, 0, 8-((l*sizeof(Ch))&7));
				crypt_.decode(buffer_, (l*sizeof(Ch)+7)&~7);
				setg(buffer_, buffer_, buffer_+l);
				return true;
			}
			return false;
		}

	private:
		Super *streambuf_;
		std::ios_base::openmode mode_;
		
		Ch *buffer_;
		std::streamsize size_;
		pos_type buffer_pos_; // バッファリング開始位置のアタッチ先のシーク位置
		pos_type crypt_pos_; // 暗号化の開始基準位置

		std::vector<Crypt::DWord> buf_;
		Crypt &crypt_;
	};

	typedef basic_cryptfilter<1024*4, char> cryptfilter;
	typedef basic_cryptfilter<1024*2, wchar_t> wcryptfilter;

	/// Cryptストリームバッファフィルタークラス(CBC版)
	template< int Buf_Size, class Ch, class Tr=std::char_traits<Ch> >
	class basic_cbccryptfilter : public std::basic_streambuf<Ch,Tr> {
	public:
		enum { BUFFER_SIZE = Buf_Size };
		typedef std::basic_streambuf<Ch,Tr> Super;

	public:
		basic_cbccryptfilter(Crypt &crypt, Crypt::DWord &initial_vector) : buffer_(0), crypt_(crypt), initial_vector_(initial_vector) {}

		basic_cbccryptfilter(Crypt &crypt, Crypt::DWord &initial_vector, Super *streambuf, std::ios::open_mode mode = std::ios::in|std::ios::out) : buffer_(0), crypt_(crypt), initial_vector_(initial_vector)
		{
			open(streambuf, mode);
		}

		~basic_cbccryptfilter()
		{
			close();
		}

		basic_cbccryptfilter *open(Super *streambuf, std::ios::open_mode mode = std::ios::in|std::ios::out)
		{
			mode_ = mode;
			setbuf(0, BUFFER_SIZE);
			streambuf_ = streambuf;
			crypt_pos_ = buffer_pos_ = streambuf_->pubseekoff(0, std::ios_base::cur, mode_);
			current_vector_ = initial_vector_;
			return this;
		}

		void close()
		{
			if(streambuf_) sync();
			streambuf_ = 0;
			buf_.resize(0);
		}

		void setBasePosition(pos_type base)
		{
			crypt_pos_ = base;
		}

		bool is_open()
		{
			return streambuf_ ? true : false;
		}

	protected:
		virtual std::basic_streambuf<Ch,Tr> *setbuf(Ch *b, std::streamsize s)
		{
			if(buffer_) sync();
			if(b) {
				buf_.resize(0);
				buffer_	= b;
				size_ = ((s*sizeof(Ch))&~7)/sizeof(Ch);
			}
			else {
				buf_.resize((s*sizeof(Ch))>>3);
				buffer_	= (Ch *)&buf_.front();
				size_ = (std::streamsize)buf_.size()*sizeof(Crypt::DWord)/sizeof(Ch);
			}
			if(mode_ & std::ios::in) setg(buffer_, buffer_, buffer_);
			if(mode_ & std::ios::out) setp(buffer_, buffer_+size_);
			return this;
		}

		virtual int_type overflow(int_type c=Tr::eof())
		{
			encode();
			if(c!=Tr::eof()) {
				*pptr() = Tr::to_char_type(c);
				pbump(1);
				return Tr::not_eof(c);
			}
			return Tr::eof();
		}

		virtual int_type underflow()
		{
			if(egptr()<=gptr()) {
				if(!decode()) return Tr::eof();
				if(egptr()<=gptr()) {
					return Tr::eof();
				}
			}
			return Tr::to_int_type(*gptr());
		}

		virtual int sync()
		{
			if(mode_ & std::ios::out) encode();
			return 0;
		}

		virtual pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
		{
			// アタッチしているストリームへの操作とみなす
			if(dir == std::ios_base::beg) {
				return seekpos((pos_type)off, mode);
			}
			else if(dir == std::ios_base::end) {
				return seekpos(streambuf_->pubseekoff(off, dir, mode), mode);
			}
			else {
				if(mode & std::ios_base::in) {
					if(off == 0) return buffer_pos_+(off_type)(gptr()-buffer_);
					else return seekpos(buffer_pos_+(off_type)(gptr()-buffer_)+off, mode);
				}
				else {
					if(off == 0) return buffer_pos_+(off_type)(pptr()-buffer_);
					else return seekpos(buffer_pos_+(off_type)(pptr()-buffer_)+off, mode);
				}
			}
		}

		virtual pos_type seekpos(pos_type pos, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
		{
			// ８バイト単位で動かし、あまりは読む
			pos_type new_pos = ((pos-crypt_pos_)&~7)-8; // ベクトル取得のために一ブロック前
			streambuf_->pubseekpos(new_pos+crypt_pos_, mode);
			if(new_pos > 0) {
				streambuf_->sgetn(buffer_, (size_&~7)/sizeof(Ch));
				current_vector_ = *(Crypt::DWord *)buffer_;
			}
			else current_vector_ = initial_vector_;
			buffer_pos_ = streambuf_->pubseekpos(((pos-crypt_pos_)&~7)+crypt_pos_, mode);
			if(mode & std::ios_base::in) {
				decode();
				pbump(pos-buffer_pos_);
			}
			return buffer_pos_+(off_type)(pptr()-buffer_);
		}

		virtual void imbue(const std::locale &loc)
		{
			// アタッチしているストリームへの操作とみなす
			streambuf_->pubimbue(loc);
		}

		void encode()
		{
			std::streamsize l = (std::streamsize)(pptr()-buffer_);
			if(l > 0) {
				if((l*sizeof(Ch))&7) memset(buffer_+l, 0, 8-((l*sizeof(Ch))&7));
				crypt_.encodeCBC(current_vector_, buffer_, (l*sizeof(Ch)+7)&~7);
				streambuf_->sputn(buffer_, (l*sizeof(Ch)+7)&~7);
				buffer_pos_ = streambuf_->pubseekoff(0, std::ios_base::cur, std::ios_base::out);
				setp(buffer_, buffer_+size_);
			}
		}

		bool decode()
		{
			buffer_pos_ = streambuf_->pubseekoff(0, std::ios_base::cur, std::ios_base::in);
			std::streamsize l = streambuf_->sgetn(buffer_, (size_&~7)/sizeof(Ch));
			if(l > 0) {
				if((l*sizeof(Ch))&7) memset(buffer_+l, 0, 8-((l*sizeof(Ch))&7));
				crypt_.decodeCBC(current_vector_, buffer_, (l*sizeof(Ch)+7)&~7);
				setg(buffer_, buffer_, buffer_+l);
				return true;
			}
			return false;
		}

	private:
		Super *streambuf_;
		std::ios_base::openmode mode_;
		
		Ch *buffer_;
		std::streamsize size_;
		pos_type buffer_pos_; // バッファリング開始位置のアタッチ先のシーク位置
		pos_type crypt_pos_; // 暗号化の開始基準位置

		std::vector<Crypt::DWord> buf_;
		Crypt::DWord current_vector_;
		Crypt::DWord &initial_vector_;
		Crypt &crypt_;
	};

	typedef basic_cbccryptfilter<1024*4, char> cbccryptfilter;
	typedef basic_cbccryptfilter<1024*2, wchar_t> wcbccryptfilter;

}

#endif // _GCTP_CRYPTFILTER_HPP_
