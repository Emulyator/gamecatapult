#ifndef _GCTP_HANDLEVECTOR_HPP_
#define _GCTP_HANDLEVECTOR_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult gctp::Handle��std::vector�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/26 22:37:50
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <vector>
#include <gctp/pointer.hpp>

namespace gctp {

//#if defined _MSC_VER && _MSC_VER == 1400
#if 1
	/** Handle�z��N���X
	 *
	 * �悭�g����̂ŁAstd::vector<Hndl>�����ʂ̎����ɂ���悤�ɂ��Ă݂�
	 *
	 * ���݈Ӗ����Ȃ�
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 22:45:38
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T>
	class HandleVector : public std::vector< Handle<_T> >
	{
	};

#else
	/** Handle�z����N���X
	 *
	 * �悭�g����̂ŁAstd::vector<HandleBase>�����ʂ̎����ɂ���悤�ɂ��Ă݂�
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 22:45:38
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class HndlVector : public std::vector<Hndl>
	{
	};

	/** Handle�z��N���X
	 *
	 * �悭�g����̂ŁAstd::vector<HandleBase>�����ʂ̎����ɂ���悤�ɂ��Ă݂�
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 22:45:38
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T>
	class HandleVector : public HndlVector {
		typedef HndlVector Super;
		typedef Handle<_T> value_type;
#ifdef _STLP_DEBUG
		typedef std::vector< Handle<typename _T> > VectorType;
#endif

	public:
		typedef value_type & reference;
		typedef const value_type & const_reference;
#ifdef _STLP_DEBUG
		typedef typename VectorType::iterator iterator;
		typedef typename VectorType::const_iterator const_iterator;
#else
		typedef value_type *iterator;
		typedef const value_type *const_iterator;
#endif

		reference at(size_type pos) { return (reference)Super::at(pos); }
		const_reference at(size_type pos) const { return (const_reference)Super::at(pos); }
		reference operator[](size_type pos) { return (reference)Super::operator[](pos); }
		const_reference operator[](size_type pos) const { return (const_reference)Super::operator[](pos); }
		reference front() { return (reference)Super::front(); }
		const_reference front() const { return (const_reference)Super::front(); }
		reference back() { return (reference)Super::back(); }
		const_reference back() const { return (const_reference)Super::back(); }

#ifdef _STLP_DEBUG
		iterator begin() { return ((VectorType *)this)->begin(); }
		const_iterator begin() const { return ((VectorType *)this)->begin(); }
		iterator end() { return ((VectorType *)this)->end(); }
		const_iterator end() const { return ((VectorType *)this)->end(); }
		reverse_iterator rbegin() { return ((VectorType *)this)->rbegin(); }
		const_reverse_iterator rbegin() const { return ((VectorType *)this)->rbegin(); }
		reverse_iterator rend() { return ((VectorType *)this)->rend(); }
		const_reverse_iterator rend() const { return ((VectorType *)this)->rend(); }

		iterator erase(iterator it) { return ((VectorType *)this)->erase(it); }
		iterator erase(iterator first, iterator last) { return ((VectorType *)this)->erase(first, last); }
#else
		iterator begin() { return (iterator)&*Super::begin(); }
		const_iterator begin() const { return (const_iterator)&*Super::begin(); }
		iterator end() { return (iterator)&*Super::end(); }
		const_iterator end() const { return (const_iterator)&*Super::end(); }
		reverse_iterator rbegin() { return (reverse_iterator)&*Super::rbegin(); }
		const_reverse_iterator rbegin() const { return (const_reverse_iterator)&*Super::rbegin(); }
		reverse_iterator rend() { return (reverse_iterator)&*Super::rend(); }
		const_reverse_iterator rend() const { return (const_reverse_iterator)&*Super::rend(); }
# ifdef __MWERKS__
		iterator erase(iterator it) { return (iterator)&*Super::erase((Super::iterator)&*it); }
		iterator erase(iterator first, iterator last) { return (iterator)&*Super::erase((Super::iterator)&*first, (Super::iterator)&*last); }
# else
		iterator erase(iterator it) { return (iterator)&*Super::erase(it); }
		iterator erase(iterator first, iterator last) { return (iterator)&*Super::erase(first, last); }
# endif
#endif
	};
#endif
} // namespace gctp

#endif //_GCTP_HANDLEVECTOR_HPP_