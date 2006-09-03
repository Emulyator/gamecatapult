#ifndef _GCTP_POINTERLIST_HPP_
#define _GCTP_POINTERLIST_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult gctp::Pointer��std::list�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/26 22:37:50
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <list>
#include <gctp/pointer.hpp>
#ifdef GCTP_FOR_EMBEDDED
#include <boost/pool/pool_alloc.hpp>
#endif

namespace gctp {

#ifdef GCTP_FOR_EMBEDDED
#	define GCTP_POINTERLISTALLOCATOR boost::fast_pool_allocator<Ptr>
#	define GCTP_POINTERLISTPOOL boost::singleton_pool<boost::fast_pool_allocator_tag, sizeof(Ptr)>
#else
#	define GCTP_POINTERLISTALLOCATOR std::allocator<Ptr>
#endif

	/** Pointer���X�g���N���X
	 *
	 * �悭�g����̂ŁAstd::list<Ptr>�����ʂ̎����ɂ���悤�ɂ��Ă݂�
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 22:45:38
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class PtrList : public std::list<Ptr, GCTP_POINTERLISTALLOCATOR>
	{
	};

	/** Pointer���X�g�N���X
	 *
	 * �悭�g����̂ŁAstd::list<Ptr>�����ʂ̎����ɂ���悤�ɂ��Ă݂�
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 22:45:38
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T>
	class PointerList : public PtrList {
		typedef PtrList Super;
		typedef Pointer<_T> value_type;
		typedef std::list< Pointer<typename _T> > ListType;

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
	};

} // namespace gctp

#endif //_GCTP_POINTERLIST_HPP_