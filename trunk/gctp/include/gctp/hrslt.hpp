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
#include <Dxerr.h>
#include <iosfwd>
#include <boost/detail/workaround.hpp>
#include <tchar.h>

namespace gctp {

	/** DirectXのエラーコード
	 *
	 * 要するにWidnowsのHRESULT
	 * D3DXGetErrorStringを使ってエラーコードをostreamに出力する機能を提供。
	 */
	struct HRslt {
		HRslt() : i(S_OK) {}
		HRslt(const HRESULT &src) : i(src) {}
		_TCHAR *message(_TCHAR *buf, std::size_t buflen) const {
			_tcsncpy(buf, DXGetErrorString(i), buflen);
			_tcsncpy(buf, _T(" : "), buflen);
			_tcsncpy(buf, DXGetErrorDescription(i), buflen);
			return buf;
		}

#if defined(__SUNPRO_CC) && BOOST_WORKAROUND(__SUNPRO_CC, <= 0x530)

		operator bool () const
		{
			return SUCCEEDED(i) ? true : false;
		}

#elif \
    ( defined(__MWERKS__) && BOOST_WORKAROUND(__MWERKS__, < 0x3200) ) || \
    ( defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__ < 304) )

		typedef _TCHAR * (HRslt::*SafeBoolType) (_TCHAR *, std::size_t) const;

		operator SafeBoolType() const // never throws
		{
			return SUCCEEDED(i) ? &HRslt::message : 0;
		}

#else 

		/*typedef HRESULT HRslt::*SafeBoolType;

		operator SafeBoolType() const // never throws
		{
			return SUCCEEDED(i) ? &HRslt::i : 0;
		}*/

	private:
		static void safebooltrue( HRslt *** ) {}
	    typedef void (*SafeBoolType)( HRslt *** );
	public:
		operator SafeBoolType() const
		{
			return SUCCEEDED(i) ?
				safebooltrue 
				: 0;
		}

#endif
		// operator! is a Borland-specific workaround
		bool operator! () const
		{
			return FAILED(i);
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
		return os << hr.message(buf, 256);
	}

} //namespace gctp

#endif //_GCTP_HRSLT_HPP_