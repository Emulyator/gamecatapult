#ifndef _GCTP_CORE_CONTEXT_HPP_
#define _GCTP_CORE_CONTEXT_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult ���\�[�X�R���e�L�X�g�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/pointerlist.hpp>
#include <gctp/extension.hpp>
#include <gctp/db.hpp>
#include <gctp/class.hpp>
#include <gctp/tuki.hpp>
#include <gctp/signal.hpp>
#include <tchar.h>

namespace gctp {
	class Serializer;
}

namespace gctp { namespace core {

	/** ���\�[�X�R���e�L�X�g�N���X
	 *
	 * ���̃R���e�L�X�g�Ń��[�h�������\�[�X��ێ����A��̎��ɕێ����Ă������\�[�X�����������
	 * �������B
	 *
	 * �J�����g�R���e�L�X�g���A�X�^�b�N��ɕێ�����@�\������
	 *
	 * �f�[�^�x�[�X�ւ̓o�^���Ǘ����A�v�����\�[�X�\�[�X������̃R���X�g���N�^�����A
	 * ���\�[�X�\�[�X�����烊�\�[�X���ւ̕ϊ��A�c�a�̃K�x�[�W�A��S������B
	 * @see Extention
	 * 
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 11:18:11
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Context : public Object {
	public:
		/** �R���X�g���N�^
		 *
		 * �����ɃJ�����g�ɐݒ�B�i�R���e�L�X�g�X�^�b�N�Ɏ������v�b�V���j
		 */
		Context(bool do_push = true) : is_pushed_(false), loading_async_(false)
		{
			on_ready_slot.bind(this);
			if(do_push) push();
		}

		/** �R���X�g���N�^
		 *
		 * �e���w�肷��
		 *
		 * �����ɃJ�����g�ɐݒ�B�i�R���e�L�X�g�X�^�b�N�Ɏ������v�b�V���j
		 */
		Context(Handle<Context> parent, bool do_push = true) : is_pushed_(false), loading_async_(false), parent_(parent)
		{
			on_ready_slot.bind(this);
			if(do_push) push();
		}

		/** �f�X�g���N�^
		 *
		 * �����ɃR���e�L�X�g�X�^�b�N���|�b�v
		 *
		 * �R���e�L�X�g�͐������ꂽ���ɊJ������Ȃ���΂Ȃ�Ȃ��B�i�������Ȃ���
		 * �t���O�����g���N��������j�J�����g�������Ŗ��������ꍇ�A�G���[���b�Z�[�W�𑗏o����
		 *
		 * �f�X�g���N�^���Ȃ̂ŁA��O�͓����Ȃ�
		 */
		~Context()
		{
			pop();
		}

		/// �R���e�L�X�g���J�����g��
		void push();
		/// ���O�̃R���e�L�X�g���J�����g��
		void pop();

		/// �q�R���e�L�X�g�����
		Pointer<Context> newChild(bool do_push = true)
		{
			return new Context(this, do_push);
		}

		Handle<Context> parent() { return parent_; }

		/** �R���e���c���[�h
		 *
		 * @return �ǂݍ���ō\�z�ς݂̃I�u�W�F�N�g�B���s�����ꍇ�̓k���n���h�����Ԃ�B
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/08 11:18:22
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		Hndl load(const _TCHAR *name);

		/** ���[�h�v��
		 *
		 * @return ���\�z�̃I�u�W�F�N�g�B���N�G�X�g�Ɏ��s�����ꍇ�̓k���n���h�����Ԃ�B
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/08 11:18:22
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		Hndl loadAsync(const _TCHAR *name);

		/** (�쐬�ς݂�)�I�u�W�F�N�g�o�^
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/08 11:18:22
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		void add(const Ptr ptr);

		/** (�쐬�ς݂�)�I�u�W�F�N�g�o�^
		 *
		 * �ق��̃N���G�C�g�n�ƈႢ�A����͊����G���g�����㏑������
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/08 11:18:22
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		Hndl insert(const Ptr ptr, const _TCHAR *name);

		/// �I�u�W�F�N�g�o�^
		template<class T>
		Handle<T> create(const _TCHAR *name)
		{
			if(name) {
				Hndl h = find(name);
				if(h) return h;
			}
			Pointer<T> p = new T;
			if(p) return insert(p, name);
			return 0;
		}
		/// �I�u�W�F�N�g�o�^
		template<class T>
		Handle<T> createAndSetUp(const _TCHAR *name = 0)
		{
			if(name) {
				Hndl h = find(name);
				if(h) return h;
			}
			Pointer<T> p = new T;
			if(p) {
				if(p->setUp()) return insert(p, name);
			}
			return 0;
		}
		/// �I�u�W�F�N�g�o�^
		template<class T>
		Handle<T> createAndSetUpEx(const _TCHAR *name = 0)
		{
			if(name) {
				Hndl h = find(name);
				if(h) return h;
			}
			Pointer<T> p = new T;
			if(p) {
				if(p->setUp(name)) return insert(p, name);
			}
			return 0;
		}

		/** �I�u�W�F�N�g����
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/08 11:18:22
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		Hndl create(const GCTP_TYPEINFO &typeinfo);
		
		/** �I�u�W�F�N�g����
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/08 11:18:22
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		Hndl create(const char *classname);

		/** ���O�t���I�u�W�F�N�g����
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/08 11:18:22
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		Hndl create(const GCTP_TYPEINFO &typeinfo, const _TCHAR *name);
		
		/** ���O�t���I�u�W�F�N�g����
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/08 11:18:22
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		Hndl create(const char *classname, const _TCHAR *name);

		/** �I�u�W�F�N�g����
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/08 11:18:22
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		Hndl find(const _TCHAR *name);

		/// �I�u�W�F�N�g����
		Hndl operator[](const _TCHAR *name)
		{
			return find(name);
		}

		/// �V�[�P���X�̐擪��Ԃ�
		PtrList::iterator begin()
		{
			return ptrs_.begin();
		}
		/// �V�[�P���X�̐擪��Ԃ�
		PtrList::const_iterator begin() const
		{
			return ptrs_.begin();
		}
		/// �V�[�P���X�̍Ō��Ԃ�
		PtrList::iterator end()
		{
			return ptrs_.end();
		}
		/// �V�[�P���X�̍Ō��Ԃ�
		PtrList::const_iterator end() const
		{
			return ptrs_.end();
		}

		virtual void serialize(Serializer &);

		/// �J�����g�R���e�L�X�g��Ԃ�
		inline static Context &current()
		{
			GCTP_ASSERT(current_);
			return *current_;
		}

		/// �񓯊��ǂݍ��݂ɂ�郍�[�h�������H�i���A���C�Y�֐��̒��ȂǂŎg�p�j
		bool loadingAsync()
		{
			return loading_async_;
		}

	protected:
		// luapp
		bool setUp(luapp::Stack &L);
		int newChild(luapp::Stack &L);
		int load(luapp::Stack &L);
		int loadAsync(luapp::Stack &L);
		int isReady(luapp::Stack &L);
		int create(luapp::Stack &L);
		int find(luapp::Stack &L);
		int pairs(luapp::Stack &L);
		int ipairs(luapp::Stack &L);
		static int current(luapp::Stack &L);

		bool loadAsync(const _TCHAR *name, const Slot2<const _TCHAR *, BufferPtr> *callback);
		bool onReady(const _TCHAR *name, BufferPtr buffer);
		MemberSlot2<Context, const _TCHAR *, BufferPtr, &Context::onReady> on_ready_slot;

	private:
		bool is_pushed_;
		bool loading_async_;
		PtrList ptrs_;
		DB db_;
		Handle<Context> parent_;
		Handle<Context> prev_;

		static Context *current_;

		struct LuaPair {
			DB *db;
			DB::Index::iterator i;
		};
		struct LuaIPair {
			int n;
			PtrList *list;
			PtrList::iterator i;
		};
		static int _pair_next(lua_State *l);
		static int _pair_gc(lua_State *l);
		static int _ipair_next(lua_State *l);
		static int _ipair_gc(lua_State *l);
	GCTP_DECLARE_CLASS
	TUKI_DECLARE(Context)
	};

}} // namespace gctp.core

namespace gctp {
	/// �J�����g�R���e�L�X�g
	inline core::Context &context()
	{
		return core::Context::current();
	}
}

#endif //_GCTP_CORE_CONTEXT_HPP_