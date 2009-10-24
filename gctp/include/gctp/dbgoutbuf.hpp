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
#include <set>
#include <mbctype.h>

namespace gctp {

	/// コンソールストリームバッファ
	template<class _Ch, class _Tr = std::char_traits<_Ch> >
	class basic_win32console_streambuf : public std::basic_streambuf<_Ch, _Tr> {
	public:
		basic_win32console_streambuf()
		{
			setg(input_buf_, input_buf_, input_buf_);
			AllocConsole();
		}
		~basic_win32console_streambuf()
		{
			FreeConsole();
		}

	protected:
		virtual int_type overflow(int_type ch)
		{
			DWORD n;
			char_type _ch = (char_type)ch;
			WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), &_ch, 1, &n, NULL);
			return 0;
		}
		virtual int_type underflow()
		{
			if(egptr()<=gptr()) {
				/*if(gptr() < (input_buf_+256) && *gptr() == 4) {
					// ^Dは一旦EOFを返す
					*gptr() = 0;
					return traits_type::eof();
				}*/
				DWORD n = 0;
				while(n==0) {
					if(!_ReadConsole(GetStdHandle(STD_INPUT_HANDLE), input_buf_, 256, &n, NULL)) {
						return traits_type::eof();
					}
				}
				/*for(DWORD i = 0; i < n; i++) {
					// ^Dを見つけたら、バッファはそこまで
					if(input_buf_[i] == 4) {
						n = i;
						break;
					}
				}*/
				setg(input_buf_, input_buf_, input_buf_+n);
			}
			return traits_type::to_int_type(*gptr());
		}
		
		BOOL _ReadConsole(HANDLE hConsoleInput, LPVOID lpBuffer, DWORD nNumberOfCharsToRead, LPDWORD lpNumberOfCharsRead, PCONSOLE_READCONSOLE_CONTROL pInputControl)
		{
			return ReadConsoleW(hConsoleInput, lpBuffer, nNumberOfCharsToRead, lpNumberOfCharsRead, pInputControl);
		}

	private:
		_Ch prev_char_;
		_Ch input_buf_[256];
	};
	
	template<>
	basic_win32console_streambuf<char>::int_type basic_win32console_streambuf<char>::overflow(int_type ch)
	{
		DWORD n;
		if(isleadbyte(ch)) {
			prev_char_ = ch;
		}
		else if(isleadbyte(prev_char_)) {
			char buffer[3] = {prev_char_, ch, 0};
			prev_char_ = 0;
			WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), buffer, 2, &n, NULL);
		}
		else {
			char buffer[2] = {ch, 0};
			WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), buffer, 1, &n, NULL);
		}
		return 0;
	}

	template<>
	BOOL basic_win32console_streambuf<char>::_ReadConsole(HANDLE hConsoleInput, LPVOID lpBuffer, DWORD nNumberOfCharsToRead, LPDWORD lpNumberOfCharsRead, PCONSOLE_READCONSOLE_CONTROL pInputControl)
	{
		return ReadConsoleA(hConsoleInput, lpBuffer, nNumberOfCharsToRead, lpNumberOfCharsRead, pInputControl);
	}

	/// デバッガログストリームクラス
	template<typename _CharType, class _Traits = std::char_traits<_CharType> >
	class debuggeroutbuf : public std::basic_streambuf<_CharType, _Traits> {
		enum {
			BUF_LEN = 256
		};
	public:
		debuggeroutbuf() : std::basic_streambuf<_CharType, _Traits>()
		{
			__buf[BUF_LEN-1] = (_CharType)'\0';
			setp(__buf, __buf+(BUF_LEN-1));
		}
		~debuggeroutbuf()
		{
			if(pptr() != pbase()) sync();
		}
		void insertSyncBuf(std::basic_streambuf<_TCHAR> *sync_buf)
		{
			sync_stream_set_.insert(sync_buf);
		}
		void eraseSyncBuf(std::basic_streambuf<_TCHAR> *sync_buf)
		{
			sync_stream_set_.erase(sync_buf);
		}

	protected:
		int sync()
		{
			output();
			for(SyncStreamSet::iterator i = sync_stream_set_.begin(); i != sync_stream_set_.end(); ++i) {
				(*i)->sputn(pbase(), (std::streamsize)(pptr()-pbase()));
			}
			setp(__buf, __buf+(BUF_LEN-1));
			return 0;
		}
		virtual int_type overflow(int_type ch)
		{
			if(ch == traits_type::eof()) return traits_type::eof();
			*epptr() = (char_type)L'\0';
			sync();
			sputc(ch);
			return traits_type::not_eof(ch);
		}
		void output()
		{
			if(pptr() != epptr()) *pptr() = L'\0';
			OutputDebugStringW(pbase());
		}

	private:
		typedef std::set<std::basic_streambuf<_CharType, _Traits> *> SyncStreamSet;
		SyncStreamSet sync_stream_set_;
		_CharType __buf[BUF_LEN];
	};
	//class_end

	template<>
	debuggeroutbuf<char>::int_type debuggeroutbuf<char>::overflow(int_type ch)
	{
		if(ch == traits_type::eof()) return traits_type::eof();
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
	void debuggeroutbuf<char>::output()
	{
		if(pptr() != epptr()) *pptr() = '\0';
		OutputDebugStringA(pbase());
	}

} // namespace gctp

#endif //_GCTP_DBGOUTBUF_HPP_