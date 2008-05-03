#ifndef _GCTP_SCENE_STAGE_HPP_
#define _GCTP_SCENE_STAGE_HPP_
/** @file
 * GameCatapult �X�e�[�W�N���X�w�b�_�t�@�C��
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
	class Context;
}

namespace gctp { namespace scene {

	class Body;
	class Light;
	class DefaultSB;
	/** �X�e�[�W�N���X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 1:05:32
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Stage : public Object, public DB
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
		/** �`��V�O�i��
		 *
		 * �`��X���b�g�͂���ɐڑ�����
		 */
		Signal1<false, float /*delta*/> draw_signal;

		/// �R���X�g���N�^
		Stage();

		/// DB�ɓo�^
		bool insert(const Hndl hndl, const _TCHAR *key);

		/// �t�@�C������Z�b�g�A�b�v
		virtual void setUp(const _TCHAR *filename);

		bool onUpdate(float delta);
		/// �X�V�X���b�g
		MemberSlot1<Stage, float /*delta*/, &Stage::onUpdate> update_slot;

		bool onDraw(float delta) const;
		/// �`��X���b�g
		ConstMemberSlot1<const Stage, float /*delta*/, &Stage::onDraw> draw_slot;

		/// �J�����g�X�e�[�W�i���̃X�e�[�W��update�Adraw�c�Ȃǂ̊Ԃ����L���j
		Stage &current() { return *current_; }

		/// �m�[�h�ǉ�
		Hndl newNode(Context &context, const char *classname, const _TCHAR *name = 0, const _TCHAR *srcfilename = 0);
		/// �m�[�h�ǉ�
		Hndl newNode(Context &context, const GCTP_TYPEINFO &typeinfo, const _TCHAR *name = 0, const _TCHAR *srcfilename = 0);

	GCTP_DECLARE_CLASS
	TUKI_DECLARE(Stage)
	
	protected:
		virtual bool doOnUpdate(float delta);
		virtual bool doOnDraw(float delta) const;

		bool setUp(luapp::Stack &L);
		void load(luapp::Stack &L);
		void newNode(luapp::Stack &L);
		void activate(luapp::Stack &L);
		void show(luapp::Stack &L);
		void hide(luapp::Stack &L);

	private:
#ifdef _MT
		mutable Mutex monitor_;
#endif
		mutable Stage* backup_current_;
		Pointer<DefaultSB> dsb_;
		GCTP_TLS static Stage* current_;
	};

}} // namespace gctp::scene

#endif //_GCTP_SCENE_STAGE_HPP_