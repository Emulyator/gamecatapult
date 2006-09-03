#ifndef _GCTP_HRSLT_HPP_
#define _GCTP_HRSLT_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult HRESULTラップクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/04 22:09:07
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <winerror.h>
#include <Dxerr9.h>
#include <d3dx9core.h>
#include <iosfwd>
#include <boost/detail/workaround.hpp>

// とりあえず。。。
#pragma warning(disable : 4996)

namespace gctp {

	/** DirectXのエラーコード
	 *
	 * 要するにWidnowsのHRESULT
	 * D3DXGetErrorStringを使ってエラーコードをostreamに出力する機能を提供。
	 */
	struct HRslt {
		HRslt() : i(S_OK) {}
		HRslt(const HRESULT &src) : i(src) {}
#ifdef UNICODE
		wchar_t *message(wchar_t *buf, std::size_t buflen) const {
			wcsncpy(buf, DXGetErrorString9(i), buflen);
			wcsncpy(buf, L" : ", buflen);
			wcsncpy(buf, DXGetErrorDescription9(i), buflen);
			return buf;
		}
#else
		char *message(char *buf, std::size_t buflen) const {
			strncpy(buf, DXGetErrorString9(i), buflen);
			strncat(buf, " : ", buflen);
			strncat(buf, DXGetErrorDescription9(i), buflen);
			return buf;
		}
#endif

#if defined(__SUNPRO_CC) && BOOST_WORKAROUND(__SUNPRO_CC, <= 0x530) || \
	( defined(_MSC_VER) && (_MSC_VER<=1400) )

		operator bool () const
		{
			return SUCCEEDED(i);
		}

#elif \
    ( defined(__MWERKS__) && BOOST_WORKAROUND(__MWERKS__, < 0x3200) ) || \
    ( defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__ < 304) )

		typedef _TCHAR * (HRslt::*SafeBoolType) (_TCHAR *, std::size_t) const;

		operator SafeBoolType() const // never throws
		{
			return FAILED(i)? 0: &HRslt::message;
		}

#else 

		typedef HRESULT HRslt::*SafeBoolType;

		operator SafeBoolType() const // never throws
		{
			return SUCCEEDED(i) ? &HRslt::i : 0;
		}

#endif
		// operator! is a Borland-specific workaround
		bool operator! () const
		{
			return !SUCCEEDED(i);
		}

		bool operator==(const HRESULT &rhs) const {
			return i == rhs;
		}
		bool operator!=(const HRESULT &rhs) const {
			return i != rhs;
		}

		HRslt &operator=(const HRESULT &rhs) {
			i = rhs;
			return *this;
		}
		HRESULT i;
	};
	
	// operator<<
	template<class E, class T> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, HRslt const & hr)
	{
		E buf[256];
		return os << hr.message(buf, sizeof(buf));
	}

} //namespace gctp

#endif //_GCTP_HRSLT_HPP_