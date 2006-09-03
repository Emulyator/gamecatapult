#ifndef _GCTP_TYPEMAP_HPP_
#define _GCTP_TYPEMAP_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult type_info���L�[�Ƃ���}�b�v�̋��ʎ����w�b�_�t�@�C��
 *
 * void *�ւ̎����ŋ��ʉ����A�����������L���Ȃ��B���̂͂��̃}�b�v����
 * �����������ʂ̃I�u�W�F�N�g���ێ����邱�ƁB
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

	/** �ÓI��������L�[�Ƃ���}�b�v�̋��ʎ���
	 *
	 * �l�̋L����͕ێ����Ȃ��B
	 * �l�͂��̃}�b�v�������������I�u�W�F�N�g�i�֐��Ȃǁj�ł���K�v������B
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
	
	/** ����̌^�ɓ���
	 */
	template<class _T>
	class TypeMap : public TypeMapBase {
		BOOST_STATIC_ASSERT(boost::is_pointer<_T>::value); // �|�C���^�[�^�ȊO�Ή����Ȃ�
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