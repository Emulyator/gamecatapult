#ifndef _GCTP_TWSTRINGMAP_HPP_
#define _GCTP_TWSTRINGMAP_HPP_
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
#include <gctp/wstringmap.hpp>
#else
#include <gctp/stringmap.hpp>
#endif

namespace gctp {

#ifdef UNICODE
	template<class _T>
	class TStaticStringMap : public WStaticStringMap<_T> {};
	template<class _T>
	class TStringMap : public WStringMap<_T> {};
#else
	template<class _T>
	class TStaticStringMap : public StaticStringMap<_T> {};
	template<class _T>
	class TStringMap : public StringMap<_T> {};
#endif

} // namespace gctp

#endif //_GCTP_TWSTRINGMAP_HPP_
