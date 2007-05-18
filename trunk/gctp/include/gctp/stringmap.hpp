#ifndef _GCTP_STRINGMAP_HPP_
#define _GCTP_STRINGMAP_HPP_
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
#include <gctp/cstr.hpp>
#include <boost/static_assert.hpp>

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
	class StaticStringMapBase : public std::map<LStr, void *> {
	public:
		void put(const char *key, const void *val);
		void *get(const char *key) const;
	};
	
	/** 特定の型に特化
	 */
	template<class _T>
	class StaticStringMap : public StaticStringMapBase {
		BOOST_STATIC_ASSERT(sizeof(_T) <= sizeof(void *));
	public:
		void put(const char *key, _T val)
		{
			StaticStringMapBase::put(key, val);
		}
		_T get(const char *key) const
		{
			return reinterpret_cast<_T>(StaticStringMapBase::get(key));
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
	class StringMapBase : public std::map<CStr, void *> {
	public:
		typedef std::map<CStr, void *> BaseType;
		void *&operator[](const CStr key);
		void *&operator[](const char *key);
		void put(const char *key, const void *val);
		void put(const CStr key, const void *val);
		void *get(const char *key) const;
		void *get(const CStr key) const;
	};
	
	/** 特定の型に特化
	 */
	template<class _T>
	class StringMap : public StringMapBase {
		BOOST_STATIC_ASSERT(sizeof(_T) <= sizeof(void *));
	public:
		void put(const char *key, _T val)
		{
			StringMapBase::put(key, val);
		}
		void put(const CStr key, _T val)
		{
			StringMapBase::put(key, val);
		}
		_T get(const char *key) const
		{
			return reinterpret_cast<_T>(StringMapBase::get(key));
		}
		_T get(const CStr key) const
		{
			return reinterpret_cast<_T>(StringMapBase::get(key));
		}
	};

} // namespace gctp

#endif //_GCTP_STRINGMAP_HPP_