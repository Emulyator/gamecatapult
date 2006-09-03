#ifndef _GCTP_ZFILTER_HPP_
#define _GCTP_ZFILTER_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * zlibファイルバッファ・ストリームフィルタークラスヘッダファイル
 *
 * 平山直行氏によるVAFX内の、zlibファイルバッファクラスが元になっています。\n
 * http://www.jah.ne.jp/~naoyuki/Writings/ExtIos.html
 *
 * ランダムアクセスには対応していないので、seekしてはいけない。
 * 
 * i/ostreamから構成するインターフェースも残すが、やっぱzstreambufはstreambufを内部でポイントし、
 * ストリームバッファへのフィルタにすべきだなぁ。
 @code
	// 圧縮
    {
        std::ifstream ifs("test.txt",std::ios::binary);

        std::ofstream ofs("test.out",std::ios::binary|std::ios::trunc);
        gctp::ozfilter ozf(ofs);

        int c;
        while((c=ifs.get())!=EOF){
            ozf << (char)c;
        }
    }

    // 伸長
    {
        std::ifstream ifs("test.out",std::ios::binary);
        gctp::izfilter izf(ifs);

        for(;;){
            std::vector<char> v(128);
            izf.read(v.begin(),v.size());
            if(izf.gcount()==0){
                break;
            }
            std::cout << std::string(v.begin(),izf.gcount());
        }
    }
 @endcode
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/09 23:00:42
 *
 * Copyright (C) 1999 Naoyuki Hirayama. All rights reserved. 
 *
 * modified by S.A.M.\n
 * Copyright (C) 2005 SAM (T&GG, Org.). All rights reserved.
 */
#ifdef GCTP_USE_ZLIB
#include "zlib.h"
#include <ios> // for std::iostream
#include <vector> // for in/deflate buffer

namespace gctp {

	class ibstream;
	class obstream;

	/// zlibのエラーを投げる例外クラス
	class zlib_exception : public std::runtime_error {
	public:
		zlib_exception(const char* s) : std::runtime_error(s) {}
		~zlib_exception() {}
	};

	/// zlibフィルターのストリームバッファベースクラス
	template< class Ch, class Tr=std::char_traits<Ch> >
	class basic_zfilterbuf : public std::basic_streambuf<Ch,Tr> {
	public:
		enum { BUFFER_SIZE = 4096 };
		typedef std::basic_streambuf<Ch,Tr> superclass;

	public:
		basic_zfilterbuf(superclass *buf, bool do_write = false) : do_write_(do_write)
		{
			buf_= buf;
			if(do_write_) setp(sbuffer_, sbuffer_+BUFFER_SIZE);
			else setg(dbuffer_, dbuffer_+BUFFER_SIZE, dbuffer_+BUFFER_SIZE);

			// zlib 初期化
			zs_.zalloc	= Z_NULL;
			zs_.zfree	= Z_NULL;
			zs_.opaque	= Z_NULL;
			zs.next_in	= Z_NULL;
			zs.avail_in	= 0;
			if(do_write_) {
				if(deflateInit(&zs_, Z_DEFAULT_COMPRESSION) != Z_OK) {
					throw zlib_exception(zs_.msg);
				}
			}
			else {
				if(inflateInit(&zs_)!=Z_OK){
					throw zlib_exception(zs_.msg);
				}
			}
			flush_		= Z_NO_FLUSH;
		}
		~basic_zfilterbuf()
		{
			oflush = Z_FINISH;
			iflush = Z_FINISH;
			sync();

			if(out!=NULL){
				if(deflateEnd(&zs)!=Z_OK){
					throw zlib_exception(zs.msg);
				}
			}
			if(in!=NULL){
				if(inflateEnd(&zs)!=Z_OK){
					throw zlib_exception(zs.msg);
				}
			}
		}

		bool open(std::ios::openmode mode)
		{
		}

	protected:
		std::basic_streambuf<Ch,Tr>* setbuf(Ch* b,int s)
		{
			if(out!=NULL){
				sbuffer.resize(0);
				setp(b,b+s);
			} 
			if(in!=NULL){
				dbuffer.resize(0);
				setg(b,b,b+s);
			}
			buffer	= b;
			size	= s;
			return this;
		}

		int_type overflow(int_type c=Tr::eof())
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

		int_type underflow(void)
		{
			if(egptr()<=gptr()) {
				if(iflush==Z_FINISH) {
					return Tr::eof();
				}
				decompress();
				if(egptr()<=gptr()) {
					return Tr::eof();
				}
			}
			return Tr::to_int_type(*gptr());
		}

		int sync(void)
		{
			if(in!=NULL){
			}
			if(out!=NULL){
				compress();
			}
			return 0;
		}

	protected:
		void compress(void)
		{
			zs.next_in	=(unsigned char*)buffer;
			zs.avail_in	=pptr()-buffer;
			zs.next_out	=(unsigned char*)&dbuffer.front();
			zs.avail_out=dbuffer.size();

			for(;;){
				int status = deflate(&zs, oflush); 
				if(status==Z_STREAM_END) {
					// 完了
					out->write(&dbuffer.front(),dbuffer.size()-zs.avail_out);
					return;
				}
				if(status!=Z_OK) {
					throw zlib_exception(zs.msg);
				}
				if(zs.avail_in==0) {
					// 入力バッファが尽きた
					out->write(&dbuffer.front(),dbuffer.size()-zs.avail_out);
					setp(buffer,buffer+size);
					return;
				}
				if(zs.avail_out==0) {
					// 出力バッファが尽きた
					out->write(&dbuffer.front(),dbuffer.size());
					zs.next_out	=(unsigned char*)&dbuffer.front();
					zs.avail_out=dbuffer.size();
				}
			}
		}

		void decompress(void)
		{
			zs.next_out	=(unsigned char*)buffer;
			zs.avail_out=size;

			for(;;){
				if(zs.avail_in==0){
					// 入力バッファが尽きた
					in->read(&*sbuffer.begin(),sbuffer.size());
					zs.next_in	=(unsigned char*)&*sbuffer.begin();
					zs.avail_in	=in->gcount();
					if(zs.avail_in==0){
						iflush=Z_FINISH;
					}
				}
				if(zs.avail_out==0){
					// 出力バッファが尽きた
					setg(buffer,buffer,buffer+size);
					return;
				}
				int status = inflate(&zs, iflush); 
				if(status==Z_STREAM_END){
					// 完了
					setg(buffer,buffer,buffer+size-zs.avail_out);
					return;
				}
				if(status!=Z_OK){
					throw zlib_exception(zs.msg);
				}
			}
		}

	private:
		superclass	*buf_;

		Ch			sbuffer[BUFFER_SIZE];
		Ch			dbuffer[BUFFER_SIZE];

		z_stream	in_zs_;
		z_stream	out_zs_;
	};
	
	template< class Ch, class Tr=std::char_traits<Ch> >
	class basic_zfilterbuf : public basic_zfilterbuf_base<std::basic_ostream<Ch,Tr>,std::basic_istream<Ch,Tr>,Ch,Tr> {
	public:
		basic_zfilterbuf(std::basic_ostream<Ch,Tr>& os) : basic_zfilterbuf_base(os) {}
		basic_zfilterbuf(std::basic_istream<Ch,Tr>& is) : basic_zfilterbuf_base(is) {}
	};

	typedef basic_zfilterbuf_base<obstream,ibstream,char> bzfilterbuf;

	/// 非圧縮→圧縮
	template< class OutputStream, class InputStream, class Ch, class Tr=std::char_traits<Ch> >
	class basic_ozfilter_base : public OutputStream {
	public:
		basic_ozfilter_base(OutputStream& os)
			: OutputStream(new basic_zfilterbuf_base<OutputStream,InputStream,Ch,Tr>(os))
		{
		}

		~basic_ozfilter_base(void)
		{
			flush();
			delete rdbuf();
		}
	};

	template< class Ch, class Tr=std::char_traits<Ch> >
	class basic_ozfilter: public basic_ozfilter_base<std::basic_ostream<Ch,Tr>,std::basic_istream<Ch,Tr>,Ch,Tr> {
	public:
		basic_ozfilter(std::basic_ostream<Ch,Tr>& os) : basic_ozfilter_base(os) {}
	};

	/// 圧縮→非圧縮
	template< class OutputStream, class InputStream, class Ch, class Tr=std::char_traits<Ch> >
	class basic_izfilter_base : public InputStream {
	public:
		basic_izfilter_base(InputStream& is)
			: InputStream(new basic_zfilterbuf_base<OutputStream,InputStream,Ch,Tr>(is))
		{
		}

		~basic_izfilter_base(void)
		{
			delete rdbuf();
		}
	};

	template< class Ch, class Tr=std::char_traits<Ch> >
	class basic_izfilter: public basic_izfilter_base<std::basic_ostream<Ch,Tr>,std::basic_istream<Ch,Tr>,Ch,Tr> {
	public:
		basic_izfilter(std::basic_istream<Ch,Tr>& os) : basic_izfilter_base(os) {}
	};

	typedef basic_ozfilter<char> ozfilter;
	typedef basic_izfilter<char> izfilter;
	typedef basic_ozfilter<wchar_t> wozfilter;
	typedef basic_izfilter<wchar_t> wizfilter;
	typedef basic_ozfilter_base<obstream,ibstream,char> obzfilter;
	typedef basic_izfilter_base<obstream,ibstream,char> ibzfilter;
}
#endif // GCTP_USE_ZLIB

#endif // _GCTP_ZFILTER_HPP_
