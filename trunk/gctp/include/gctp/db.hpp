#ifndef _GCTP_DB_HPP_
#define _GCTP_DB_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult ���\�[�X�f�[�^�x�[�X�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 18:26:10
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/pointer.hpp>
#include <gctp/cstr.hpp>
#include <map>
#include <iosfwd>

namespace gctp {

	/** �f�[�^�x�[�X
	 *
	 * �o�^���ꂽ���\�[�X�̍����B
	 *
	 * �L�[�l�͔C�ӂ̕����񂪎g���邪�A���ׂđ啶���̂��̂́A�V�X�e���ɗ\�񂳂�Ă���B
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 17:47:20
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class DB {
	public:
		typedef std::map<CStr, Hndl> Index;

		/// �ǉ�
		bool insert(const char *key, const Hndl hndl);

		/// �u������
		void set(const char *key, Hndl);

		/// �폜
		void erase(const char *key);

		/// ����
		Hndl find(const char *key) const;

		/// ����
		Hndl operator[](const char *key) const
		{
			return find(key);
		}
		
		Index::iterator begin()
		{
			return index_.begin();
		}
		Index::const_iterator begin() const
		{
			return index_.begin();
		}
		Index::iterator end()
		{
			return index_.end();
		}
		Index::const_iterator end() const
		{
			return index_.end();
		}

		Index::iterator findUpper(const char *key)
		{
			return index_.upper_bound(key);
		}
		Index::const_iterator findUpper(const char *key) const
		{
			return index_.upper_bound(key);
		}
		Index::iterator findLower(const char *key)
		{
			return index_.lower_bound(key);
		}
		Index::const_iterator findLower(const char *key) const
		{
			return index_.lower_bound(key);
		}

	private:
		Index index_;
	};

	// operator<<
	template<class E, class T> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, DB const & d)
	{
		for(DB::Index::const_iterator i = d.begin(); i != d.end(); ++i) {
			os << i->first << std::endl;
		}
		return os;
	}

	/// �O���[�o���f�[�^�x�[�X�C���X�^���X
	DB &db();

	/** �f�[�^�x�[�X�ɓo�^������
	 *
	 * ���łɃf�[�^�x�[�X�ɂ���Ȃ炻���Ԃ��A�����Ŗ���������V�K���삵�c�a�ɓo�^����A�Ƃ���
	 * ����̃e���v���[�g
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/16 14:03:54
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T>
	Pointer<_T> createOnDB(const char *name)
	{
		Handle<_T> h = db()[name];
		if(h) return h.lock();
		else {
			Pointer<_T> ret = new _T;
			if(ret) {
				if(ret->setUp(name)) {
					PRNN("���\�[�X"<<name<<"�𐧍�");
					h = ret;
					db().insert(name, h);
					return ret;
				}
			}
			PRNN("���\�[�X"<<name<<"�̐���Ɏ��s");
		}
		return 0;
	}

#if !defined(_MSC_VER) || (_MSC_VER > 1300)
	/** �f�[�^�x�[�X�ɓo�^������
	 *
	 * ���łɃf�[�^�x�[�X�ɂ���Ȃ炻���Ԃ��A�����Ŗ���������V�K���삵�c�a�ɓo�^����A�Ƃ���
	 * ����̃e���v���[�g
	 *
	 * setUp���\�b�h���w��ł���
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/16 14:03:54
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T, bool (_T::*SetUpMethod)(const char *name)>
	Pointer<_T> createOnDB(const char *name)
	{
		Handle<_T> h = db()[name];
		if(h) return h.lock();
		else {
			Pointer<_T> ret = new _T;
			if(ret) {
				if((ret.get()->*SetUpMethod)(name)) {
					PRNN("���\�[�X"<<name<<"�𐧍�");
					h = ret;
					db().insert(name, h);
					return ret;
				}
			}
			PRNN("���\�[�X"<<name<<"�̐���Ɏ��s");
		}
		return 0;
	}
#endif

	/** �f�[�^�x�[�X�ɓo�^������
	 *
	 * ���łɃf�[�^�x�[�X�ɂ���Ȃ炻���Ԃ��A�����Ŗ���������V�K���삵�c�a�ɓo�^����A�Ƃ���
	 * ����̃e���v���[�g
	 *
	 * DB�o�^����setUp�ɓn���������ʁX�Ɏw��ł���
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/16 14:03:54
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T>
	Pointer<_T> createOnDB(const char *name, const char *setup)
	{
		Handle<_T> h = db()[name];
		if(h) return h.lock();
		else {
			Pointer<_T> ret = new _T;
			if(ret) {
				if(ret->setUp(setup)) {
					PRNN("���\�[�X"<<name<<"�𐧍�");
					h = ret;
					db().insert(name, h);
					return ret;
				}
			}
			PRNN("���\�[�X"<<name<<"�̐���Ɏ��s");
		}
		return 0;
	}

#if !defined(_MSC_VER) || (_MSC_VER > 1300)
	/** �f�[�^�x�[�X�ɓo�^������
	 *
	 * ���łɃf�[�^�x�[�X�ɂ���Ȃ炻���Ԃ��A�����Ŗ���������V�K���삵�c�a�ɓo�^����A�Ƃ���
	 * ����̃e���v���[�g
	 *
	 * DB�o�^����setUp�ɓn���������ʁX�Ɏw��ł���
	 * �X��setUp���\�b�h���w�肷��
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/16 14:03:54
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T, bool (_T::*SetUpMethod)(const char *name)>
	Pointer<_T> createOnDB(const char *name, const char *setup)
	{
		Handle<_T> h = db()[name];
		if(h) return h.lock();
		else {
			Pointer<_T> ret = new _T;
			if(ret) {
				if((ret->*SetUpMethod)(setup)) {
					PRNN("���\�[�X"<<name<<"�𐧍�");
					h = ret;
					db().insert(name, h);
					return ret;
				}
			}
			PRNN("���\�[�X"<<name<<"�̐���Ɏ��s");
		}
		return 0;
	}
#endif
} // namespace gctp

#endif //_GCTP_DB_HPP_