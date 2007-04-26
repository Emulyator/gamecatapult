#ifndef _GCTP_ROP_HPP_
#define _GCTP_ROP_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/**@file rop.hpp
 * ROPS(Reference of Pointers)チャンクを生成する。
 *
 * 直前のIFFチャンクの先頭からの数えた、それぞれのポインターの相対オフセットでポインターの位置を示す。
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/12/30
 * $Id: rop.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include <iosfwd> // for i/ostream
#include <strstream>
#include <set>

namespace gctp {

	/** ROPSチャンク製作クラス
	 *
	 * @code

	Rops rops;

	 * @endcode
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/12/06 17:49:58
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Rop : public std::set<int> {
	public:
		enum {
			TYPE_PAD   = 0x00,
			TYPE_CHAR  = 0x40,
			TYPE_SHORT = 0x80,
			TYPE_LONG  = 0xc0,
			CHAR_MASK  = 0xc0,
			DATA_MASK  = 0x3f
		};

		class TooLargePointerOffsetException {};

		friend std::ostream &operator<<(std::ostream &lhs, Rop &rhs) {
			std::strstream buf;
			iterator prev = rhs.end();
			for(iterator i = rhs.begin(); i != rhs.end(); i++) {
				long w;
				if(prev == rhs.end()) {w = (*i)/4; prev = i;}
				else {w = (*i - *prev)/4; prev = i;}

				if(w < 64) { // BYTE
					buf << (char)(0x40 | (char)w);
				}
				else if(w < 16384) { // SHORT
					buf << (char)(0x80 | *((char *)&w + 1));
					buf << *(char *)&w;
				}
				else if(w < 1073741824) { // LONG
					buf << (char)(0xC0 | *((char *)&w + 3));
					buf << (char)(0xC0 | *((char *)&w + 2));
					buf << (char)(0xC0 | *((char *)&w + 1));
					buf << *(char *)&w;
				}
				else {
					throw TooLargePointerOffsetException();
					//std::cerr << "ROPで解決できないポインタがありました。" << std::endl;
				}
			}
			while(lhs.tellp()%4) lhs << std::ends;
			lhs << "ROPS";
			long size = buf.pcount();
			lhs.write((char *)&size, 4);
			lhs << buf.rdbuf();
			while(lhs.tellp()%4) lhs << std::ends;
			return lhs;
		}
		
		friend std::istream &operator>>(std::istream &lhs, Rop &rhs) { // ＩＤは読まれていると仮定
			int size, offset;
			lhs.read(reinterpret_cast<char *>(&size), 4);
			int pos = lhs.tellg();
			unsigned char buf0, buf1, buf2, buf3;

			while(lhs.tellg() < pos + size) {
				lhs.read(reinterpret_cast<char *>(&buf0), 1);
				switch(buf0&CHAR_MASK){
				case TYPE_PAD:
					continue;
				case TYPE_CHAR:
					offset = (unsigned long)(buf0&DATA_MASK);
					break;
				case TYPE_SHORT:
					lhs.read(reinterpret_cast<char *>(&buf1), 1);
					offset = (unsigned long)((buf0&DATA_MASK)<<8|buf1);
					break;
				case TYPE_LONG:
					lhs.read(reinterpret_cast<char *>(&buf1), 1);
					lhs.read(reinterpret_cast<char *>(&buf2), 1);
					lhs.read(reinterpret_cast<char *>(&buf3), 1);
					offset = (unsigned long)((buf0&DATA_MASK)<<24|(buf1<<16)|(buf2<<8)|buf3);
					break;
				}
				offset *= 4;
				rhs.add(offset);
			}
			return lhs;
		}

		void add(int pos)
		{
			insert(pos);
		}
	};
}

#endif //_GCTP_ROP_HPP_