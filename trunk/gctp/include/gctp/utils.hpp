#ifndef _GCTP_UTILS_HPP_
#define _GCTP_UTILS_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult ユーティリーいろいろヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 2:42:16
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/types.hpp>

namespace gctp {
	
	/** intで渡された文字を、文字列に変換する。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/20 4:06:17
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline std::string charToStr(int c)
	{
		char buf[5];
		int _c = (c>>24)&0xFF;
		if(_c>0 && _c != 0xFF) {
			buf[0] = _c;
			buf[1] = (c>>16)&0xFF;
			buf[2] = (c>>8)&0xFF;
			buf[3] = (c)&0xFF;
			buf[4] = 0;
			return buf;
		}
		_c = (c>>16)&0xFF;
		if(_c>0 && _c != 0xFF) {
			buf[0] = _c;
			buf[1] = (c>>8)&0xFF;
			buf[2] = (c)&0xFF;
			buf[3] = 0;
			return buf;
		}
		_c = (c>>8)&0xFF;
		if(_c>0 && _c != 0xFF) {
			buf[0] = _c;
			buf[1] = (c)&0xFF;
			buf[2] = 0;
			return buf;
		}
		buf[0] = c;
		buf[1] = 0;
		return buf;
	}

	/** 文字列から（マルチバイトを考慮して）一文字読み取る
	 *
	 * @return 読み取った文字のバイトサイズ
	 * @param c 読み取った文字を入れる
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/20 4:06:17
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline int getChar(int &c, const char *text)
	{
		if(text && *text) {
#ifdef GCTP_SHIFT_JIS
			if(isleadbyte(*text) && text[1]) {
				c = (text[0]&0xFF)<< 8 | (text[1]&0xFF);
				return 2;
			}
			c =  text[0]&0xFF;
			return 1;
			// msvcrtのmblenは、不正な文字がくるとハングする場合が多い…
#else
			mblen(NULL, 0);
			int n = mblen(text, MB_CUR_MAX);
			switch(n) {
			case 1: c =  text[0]&0xFF; return n;
			case 2: c = (text[0]&0xFF)<< 8 | (text[1]&0xFF); return n;
			case 3: c = (text[0]&0xFF)<<16 | (text[1]&0xFF)<< 8 | (text[3]&0xFF); return n;
			case 4: c = (text[0]&0xFF)<<24 | (text[1]&0xFF)<<16 | (text[2]&0xFF)<<8 | (text[3]&0xFF); return n;
			}
#endif
		}
		c = 0;
		return 0;
	}

} //namespace gctp

#endif //_GCTP_UTILS_HPP_