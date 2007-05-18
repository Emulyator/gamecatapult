#ifndef _GCTP_STRINGMAP_HPP_
#define _GCTP_STRINGMAP_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult ��������L�[�Ƃ���}�b�v�̋��ʎ����w�b�_�t�@�C��
 *
 * void *�ւ̎����ŋ��ʉ����A�����������L���Ȃ��B���̂͂��̃}�b�v����
 * �����������ʂ̃I�u�W�F�N�g���ێ����邱�ƁB
 * 
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 13:29:41
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <map>
#include <gctp/cstr.hpp>
#include <boost/static_assert.hpp>

namespace gctp {

	/** �ÓI��������L�[�Ƃ���}�b�v�̋��ʎ���
	 *
	 * �l�����łȂ��A�L�[�̋L������ێ����Ȃ��}�b�v�B
	 * �L�[����������̃}�b�v�������������I�u�W�F�N�g�i���e����������Ȃǁj�ł���K�v������B
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
	
	/** ����̌^�ɓ���
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

	/** ��������L�[�Ƃ���}�b�v�̋��ʎ���
	 *
	 * ������̃R�s�[�͕ێ����邪�A�l�̋L����͕ێ����Ȃ��B
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
	
	/** ����̌^�ɓ���
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