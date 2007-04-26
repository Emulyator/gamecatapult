#ifndef _GCTP_WSTRINGMAP_HPP_
#define _GCTP_WSTRINGMAP_HPP_
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
#include <gctp/wcstr.hpp>

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
	class WStaticStringMapBase : public std::map<WLStr, void *> {
	public:
		void put(const wchar_t *key, const void *val);
		void *get(const wchar_t *key) const;
	};
	
	/** ����̌^�ɓ���
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

	/** ��������L�[�Ƃ���}�b�v�̋��ʎ���
	 *
	 * ������̃R�s�[�͕ێ����邪�A�l�̋L����͕ێ����Ȃ��B
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
	
	/** ����̌^�ɓ���
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
