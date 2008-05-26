#ifndef _GCTP_SCENE_ENTITY_HPP_
#define _GCTP_SCENE_ENTITY_HPP_
/** @file
 * GameCatapult �G���e�B�e�B�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/signal.hpp>
#include <gctp/scene/body.hpp>
#include <gctp/tuki.hpp>

namespace gctp {
	namespace core {
		class Context;
	}
}

namespace gctp { namespace scene {

	class World;
	class Motion;
	class MotionMixer;
	/** �G���e�B�e�B�N���X
	 *
	 * Body�Ƀ��[�V������^������
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2004/02/16 1:06:15
	 */
	class Entity : public Object
	{
	public:
		/// �R���X�g���N�^
		Entity(int16_t update_pri = 0);

		/// �Z�b�g�A�b�v
		void setUp(Pointer<Body> src);
		/// �Z�b�g�A�b�v
		void setUp(const _TCHAR *filename);

		/// �o��
		void enter(World &world);
		/// �v���C�I���e�B�[���w�肵�ăX�e�[�W�o��
		void enter(World &world, int16_t update_pri);
		/// �ޏ�
		void exit(World &world);

		/// �X�P���g�������邩�H
		bool hasSkeleton() { return target_; }
		/// �X�P���g��
		Skeleton &skeleton() { return *target_; }
		/// �X�P���g��
		const Skeleton &skeleton() const { return *target_; }
		/// ���[�V�����~�L�T�[�����邩�H
		bool hasMotionMixer() { return mixer_; }
		/// ���[�V�����~�L�T�[
		MotionMixer &mixer() { return *mixer_; }
		/// ���[�V�����~�L�T�[
		const MotionMixer &mixer() const { return *mixer_; }

		/// ����Ώ�
		Handle<Body> target() { return target_; }

		/// �^�[�Q�b�g�̃��[�J�����W�ʒu
		const Vector &lpos() const { return target_->root()->val.lcm().position(); }
		/// �^�[�Q�b�g�̃��[�J�����W�ʒu
		Vector &getLpos() { return target_->root()->val.getLCM().position(); }
		/// �^�[�Q�b�g�̃O���[�o�����W�ʒu
		const Vector &pos() const { return target_->root()->val.wtm().position(); }
		/// �^�[�Q�b�g�̃O���[�o�����W�ʒu
		Vector &pos() { return target_->root()->val.wtm().position(); }
		/// �^�[�Q�b�g�̃��[�J�����W
		const Matrix &lcm() const { return (*target_).root()->val.lcm(); }
		/// �^�[�Q�b�g�̃��[�J�����W
		Matrix &getLCM() { return target_->root()->val.getLCM(); }
		/// �^�[�Q�b�g�̃O���[�o�����W
		const Matrix &wtm() const { return target_->root()->val.wtm(); }
		/// �^�[�Q�b�g�̃O���[�o�����W
		Matrix &wtm() { return target_->root()->val.wtm(); }

		/// �X�V
		bool onUpdate(float delta);
		/// �X�V�X���b�g
		MemberSlot1<Entity, float /*delta*/, &Entity::onUpdate> update_slot;

	protected:
		bool setUp(luapp::Stack &L);
		void load(luapp::Stack &L);
		void enter(luapp::Stack &L);
		void exit(luapp::Stack &L);
		void setPosition(luapp::Stack &L);
		int getPosition(luapp::Stack &L);
		void setPosture(luapp::Stack &L);
		int getPosture(luapp::Stack &L);
		void setScale(luapp::Stack &L);
		int getScale(luapp::Stack &L);
		int getMotionMixer(luapp::Stack &L);
		
		virtual bool doOnUpdate(float delta);
		Pointer<MotionMixer> mixer_;
		Pointer<Body> target_;
		Handle<Body> source_;

	GCTP_DECLARE_CLASS
	TUKI_DECLARE(Entity)
	};

	/// �G���e�B�e�B�[���V�[���ɒǉ�
	extern Handle<Entity> newEntity(core::Context &context, World &world, const char *classname, const _TCHAR *name, const _TCHAR *srcfilename);
	/// �G���e�B�e�B�[���V�[���ɒǉ�
	extern Handle<Entity> newEntity(core::Context &context, World &world, const std::type_info &typeinfo, const _TCHAR *name, const _TCHAR *srcfilename);

}} // namespace gctp::scene

#endif //_GCTP_SCENE_ENTITY_HPP_