#ifndef _GCTP_HANDLELIST_HPP_
#define _GCTP_HANDLELIST_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult gctp::Handleのstd::listクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/26 22:37:50
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <list>
#include <gctp/pointer.hpp>

namespace gctp {
	/** Handleリスト基底クラス
	 *
	 * よく使われるので、std::list<HandleBase>を共通の実装にするようにしてみる
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 22:45:38
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class HndlList : public std::list<Hndl>
	{
	};

	/** Handleリストクラス
	 *
	 * よく使われるので、std::list<HandleBase>を共通の実装にするようにしてみる
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 22:45:38
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T>
	class HandleList : public HndlList {
		typedef HndlList Super;
		typedef Handle<_T> value_type;
		typedef std::list< Handle<typename _T> > ListType;

	public:
		typedef value_type & reference;
		typedef const value_type & const_reference;
		typedef typename ListType::iterator iterator;
		typedef typename ListType::const_iterator const_iterator;

		reference front() { return (reference)Super::front(); }
		const_reference front() const { return (const_reference)Super::front(); }
		reference back() { return (reference)Super::back(); }
		const_reference back() const { return (const_reference)Super::back(); }

		iterator begin() { return ((ListType *)this)->begin(); }
		const_iterator begin() const { return ((ListType *)this)->begin(); }
		iterator end() { return ((ListType *)this)->end(); }
		const_iterator end() const { return ((ListType *)this)->end(); }
		reverse_iterator rbegin() { return ((ListType *)this)->rbegin(); }
		const_reverse_iterator rbegin() const { return ((ListType *)this)->rbegin(); }
		reverse_iterator rend() { return ((ListType *)this)->rend(); }
		const_reverse_iterator rend() const { return ((ListType *)this)->rend(); }

		iterator erase(iterator it) { return ((ListType *)this)->erase(it); }
		iterator erase(iterator first, iterator last) { return ((ListType *)this)->erase(first, last); }
		void remove(const Handle<_T>& value) { ((ListType *)this)->remove(value); }
		void unique() { ((ListType *)this)->unique(); }
	};

} // namespace gctp

#endif //_GCTP_HANDLELIST_HPP_