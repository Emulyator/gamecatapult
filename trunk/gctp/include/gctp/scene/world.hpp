#ifndef _GCTP_SCENE_WORLD_HPP_
#define _GCTP_SCENE_WORLD_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult ���[���h�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/db.hpp>
#include <gctp/strutumtree.hpp>
#include <gctp/signal.hpp>
#include <gctp/tuki.hpp>
#ifdef _MT
#include <gctp/mutex.hpp>
#endif

namespace gctp {
	namespace core {
		class Context;
	}
}

namespace gctp { namespace scene {

	class Body;
	class Light;
	/** ���[���h�N���X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 1:05:32
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class World : public Object
	{
	public:
		/// �K�w�c���[
		StrutumTree strutum_tree;
		/// Body���X�g
		HandleList<Body> body_list;
		// Light���X�g(ModifierList�ƒ��ۉ����ׂ��H)
		HandleList<Light> light_list;

		/** �X�V�V�O�i��
		 *
		 * �X�V�X���b�g�͂���ɐڑ�����
		 */
		Signal1<false, float /*delta*/> update_signal;
		/** �X�V��V�O�i��
		 *
		 * �A�j���[�V�����ɂ�郍�[�J���K�w�̃Z�b�g�A�b�v�A�����ltm�Z�b�g�A�b�v��̏���\n
		 * �R���W�����X���b�g�͂���ɐڑ�����
		 */
		Signal1<false, float /*delta*/> postupdate_signal;

		/// �R���X�g���N�^
		World();

		/// �t�@�C������Z�b�g�A�b�v
		virtual void setUp(const _TCHAR *filename);

		bool onUpdate(float delta);
		/// �X�V�X���b�g
		MemberSlot1<World, float /*delta*/, &World::onUpdate> update_slot;

		/// �`��E�X�V�J�n����
		void begin() const;
		/// �`��E�X�V�I������
		void end() const;

		/// �i�W���́j�`�揈��
		void draw() const;

		/// �J�����g���[���h�i���̃��[���h��update�Adraw�c�Ȃǂ̊Ԃ����L���j
		World &current() { return *current_; }

	GCTP_DECLARE_CLASS
	TUKI_DECLARE(World)
	
	protected:
		Pointer<Body> world_body_;

		virtual bool doOnUpdate(float delta);

		bool setUp(luapp::Stack &L);
		void load(luapp::Stack &L);
		void activate(luapp::Stack &L);
		void setPosition(luapp::Stack &L);
		int getPosition(luapp::Stack &L);
		int getBoundingSphere(luapp::Stack &L);

	private:
#ifdef _MT
		mutable Mutex monitor_;
#endif
		mutable World* backup_current_;
		GCTP_TLS static World* current_;
	};

}} // namespace gctp::scene

#endif // _GCTP_SCENE_WORLD_HPP_