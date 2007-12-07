/** @file
 * GameCatapult デバッグ出力支援マクロ・関数
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:09:55
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/dbgout.hpp>
#include <gctp/dbgoutbuf.hpp>
#include <mbctype.h>

using namespace std;

namespace gctp {

	template<>
	debuggeroutbuf<char>::int_type debuggeroutbuf<char>::overflow(debuggeroutbuf<char>::int_type ch)
	{
		if(ch == traits_type::eof()) traits_type::eof();
		*epptr() = ch;
		// SJIS?
		if(-1 == _ismbstrail((const unsigned char *)pbase(), (const unsigned char *)epptr())) {
			char *lbp;
			for(lbp = epptr()-1; lbp > pbase(); lbp--) {
				if(-1 == _ismbslead((const unsigned char *)pbase(), (const unsigned char *)lbp)) {
					break;
				}
			}
			int _ch = *lbp; *lbp = '\0';
			sync();
			sputc(_ch);
			for(lbp++; lbp <= epptr(); lbp++) {
				sputc(*lbp);
			}
			*epptr() = '\0';
		}
		else {
			*epptr() = '\0';
			sync();
			sputc(ch);
		}
		return traits_type::not_eof(ch);
	}

	template<>
	debuggeroutbuf<wchar_t>::int_type debuggeroutbuf<wchar_t>::overflow(debuggeroutbuf<wchar_t>::int_type ch)
	{
		if(ch == traits_type::eof()) traits_type::eof();
		*epptr() = ch;
		*epptr() = L'\0';
		sync();
		sputc(ch);
		return traits_type::not_eof(ch);
	}

	template<>
	void debuggeroutbuf<char>::output()
	{
		if(pptr() != epptr()) *pptr() = '\0';
		OutputDebugStringA(pbase());
	}

	template<>
	void debuggeroutbuf<wchar_t>::output()
	{
		if(pptr() != epptr()) *pptr() = L'\0';
		OutputDebugStringW(pbase());
	}

	template<>
	basic_win32console_streambuf<wchar_t>::int_type basic_win32console_streambuf<wchar_t>::overflow(basic_win32console_streambuf<wchar_t>::int_type ch)
	{
		wchar_t buffer[2] = {ch, 0};
		WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), buffer, 1, NULL, NULL);
		return 0;
	}

	template<>
	basic_win32console_streambuf<char>::int_type basic_win32console_streambuf<char>::overflow(basic_win32console_streambuf<char>::int_type ch)
	{
		if(isleadbyte(ch)) {
			prev_char_ = ch;
		}
		else if(isleadbyte(prev_char_)) {
			char buffer[3] = {prev_char_, ch, 0};
			prev_char_ = 0;
			WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), buffer, 2, NULL, NULL);
		}
		else {
			char buffer[2] = {ch, 0};
			WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), buffer, 1, NULL, NULL);
		}
		return 0;
	}

#if GCTP_DBGOUT
	static debuggeroutbuf<_TCHAR> _dbgout_buf;
	basic_ostream<_TCHAR> dbgout(&_dbgout_buf);		// デバッガアウトプットストリーム
#endif

#if GCTP_LOGFILE
	//basic_ofstream<_TCHAR> logfile(_T("log.txt"));		// ログファイルストリーム
	basic_ofstream<_TCHAR> logfile;
#endif

};	// namespace gctp
