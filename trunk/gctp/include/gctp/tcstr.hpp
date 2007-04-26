#ifndef _GCTP_TCSTR_HPP_
#define _GCTP_TCSTR_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 固定文字列クラスヘッダファイル
 *
 * バッファを自分で保持しない、あるいはバッファの伸張を行わない文字列クラス。ワイド文字版
 * 
 * ワイド文字版とそうでない版を、MSの_TCHARマクロ風に切り替えるヘッダ
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 13:29:41
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#ifdef UNICODE
#include <gctp/wcstr.hpp>
#else
#include <gctp/cstr.hpp>
#endif

namespace gctp {

#ifdef UNICODE
	typedef WLStr TLStr;
	typedef WCStr TCStr;
	typedef TempWCStr TempTCStr;
#else
	typedef LStr TLStr;
	typedef CStr TCStr;
	typedef TempCStr TempTCStr;
#endif

} // namespace gctp

#endif //_GCTP_TCSTR_HPP_
