#ifndef _GCTP_TYPEMAP_HPP_
#define _GCTP_TYPEMAP_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult type_infoをキーとするマップの共通実装ヘッダファイル
 *
 * void *への実装で共通化し、メモリを所有しない。実体はこのマップよりも
 * 寿命が長い別のオブジェクトが保持すること。
 * 
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 13:29:41
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <map>
#include <gctp/typeinfo.hpp>
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>

namespace gctp {

	/** 静的文字列をキーとするマップの共通実装
	 *
	 * 値の記憶域は保持しない。
	 * 値はこのマップより寿命が長いオブジェクト（関数など）である必要がある。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 14:07:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class TypeMapBase : public std::map<const GCTP_TYPEINFO *, void *> {
	public:
		void put(const GCTP_TYPEINFO &key, const void *val);
		void *get(const GCTP_TYPEINFO &key) const;
	};
	
	/** 特定の型に特化
	 */
	template<class _T>
	class TypeMap : public TypeMapBase {
		BOOST_STATIC_ASSERT(boost::is_pointer<_T>::value); // ポインター型以外対応しない
	public:
		void put(const GCTP_TYPEINFO &key, _T val)
		{
			TypeMapBase::put(key, val);
		}
		_T get(const GCTP_TYPEINFO &key) const
		{
			return reinterpret_cast<_T>(TypeMapBase::get(key));
		}
	};

} // namespace gctp

#endif //_GCTP_TYPEMAP_HPP_