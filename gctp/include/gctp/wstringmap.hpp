#ifndef _GCTP_WSTRINGMAP_HPP_
#define _GCTP_WSTRINGMAP_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 文字列をキーとするマップの共通実装ヘッダファイル
 *
 * void *への実装で共通化し、メモリを所有しない。実体はこのマップよりも
 * 寿命が長い別のオブジェクトが保持すること。
 * 
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 13:29:41
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <map>
#include <gctp/wcstr.hpp>

namespace gctp {

	/** 静的文字列をキーとするマップの共通実装
	 *
	 * 値だけでなく、キーの記憶域も保持しないマップ。
	 * キー文字列もこのマップより寿命が長いオブジェクト（リテラル文字列など）である必要がある。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 14:07:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class WStaticStringMapBase : public std::map<WLStr, void *> {
	public:
		void put(const wchar_t *key, const void *val);
		void *get(const wchar_t *key) const;
	};
	
	/** 特定の型に特化
	 */
	template<class _T>
	class WStaticStringMap : public WStaticStringMapBase {
	public:
		void put(const wchar_t *key, _T val)
		{
			WStaticStringMapBase::put(key, val);
		}
		_T get(const wchar_t *key) const
		{
			return reinterpret_cast<_T>(WStaticStringMapBase::get(key));
		}
	};

	/** 文字列をキーとするマップの共通実装
	 *
	 * 文字列のコピーは保持するが、値の記憶域は保持しない。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 14:07:10
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class WStringMapBase : public std::map<WCStr, void *> {
	public:
		typedef std::map<WCStr, void *> BaseType;
		void *&operator[](const WCStr key);
		void *&operator[](const wchar_t *key);
		void put(const wchar_t *key, const void *val);
		void put(const WCStr key, const void *val);
		void *get(const wchar_t *key) const;
		void *get(const WCStr key) const;
	};
	
	/** 特定の型に特化
	 */
	template<class _T>
	class WStringMap : public WStringMapBase {
	public:
		void put(const wchar_t *key, _T val)
		{
			WStringMapBase::put(key, val);
		}
		void put(const WCStr key, _T val)
		{
			WStringMapBase::put(key, val);
		}
		_T get(const wchar_t *key) const
		{
			return reinterpret_cast<_T>(WStringMapBase::get(key));
		}
		_T get(const WCStr key) const
		{
			return reinterpret_cast<_T>(WStringMapBase::get(key));
		}
	};

} // namespace gctp

#endif //_GCTP_WSTRINGMAP_HPP_
