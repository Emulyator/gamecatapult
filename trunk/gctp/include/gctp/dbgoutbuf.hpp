#ifndef _GCTP_DBGOUTBUF_HPP_
#define _GCTP_DBGOUTBUF_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult デバッグ出力バッファ
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/10 7:59:30
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */

#include <streambuf>
#include <mbctype.h>

namespace gctp {

	/// デバッガログストリームクラス
	template<typename _CharType>
	class debuggeroutbuf : public std::basic_streambuf<_CharType> {
		enum {
			BUF_LEN = 256
		};
	public:
		debuggeroutbuf() : std::basic_streambuf<_CharType>() {
			setbuf(__buf, BUF_LEN-1);
			__buf[BUF_LEN-1] = (_CharType)'\0';
		}
		~debuggeroutbuf() {
			if(pptr() != pbase()) sync();
		}
	protected:
		std::basic_streambuf<_CharType> *setbuf(_CharType *s, std::streamsize n) {
			if(s && n) {
				setp(s, s+n);
				return this;
			}
			return 0;
		}
		int sync() {
			output();
			setbuf(__buf, BUF_LEN-1);
			return std::basic_streambuf<_CharType>::sync();
		}
		virtual int_type overflow(int_type ch);
		void output();
	private:
		_CharType __buf[BUF_LEN];
	};
	//class_end

	template<> debuggeroutbuf<char>::int_type debuggeroutbuf<char>::overflow(debuggeroutbuf<char>::int_type ch);

	template<> debuggeroutbuf<wchar_t>::int_type debuggeroutbuf<wchar_t>::overflow(debuggeroutbuf<wchar_t>::int_type ch);

	template<> void debuggeroutbuf<char>::output();

	template<> void debuggeroutbuf<wchar_t>::output();

} // namespace gctp

#endif //_GCTP_DBGOUTBUF_HPP_