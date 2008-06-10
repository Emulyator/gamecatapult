#ifndef _GCTP_SCENE_PHYSICWORLD_HPP_
#define _GCTP_SCENE_PHYSICWORLD_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult �����V�~�����[���h�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/signal.hpp>
#include <gctp/tuki.hpp>
#include <gctp/pointer.hpp>
#include <gctp/vector.hpp>
#include <gctp/aabox.hpp>
#ifdef _MT
#include <gctp/mutex.hpp>
#endif

namespace gctp { namespace scene {

	class World;
	class Body;

	struct PhysicWorldImpl;

	/** �����V�~�����[���h�N���X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 1:05:32
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class PhysicWorld : public Object
	{
	public:
		/// �R���X�g���N�^
		PhysicWorld();
		/// �f�X�g���N�^
		virtual ~PhysicWorld();

		/// Bullet�I�u�W�F�N�g����
		void setUp(const AABox &aabb, uint max_bodies);
		/// �ǉ��\���H
		bool canAdd();
		/// Body�𔠂Ƃ��Ēǉ�
		void addBox(Handle<Body> body, const Vector &initial_pos, float mass, const Vector &initial_local_inertia = VectorC(0,0,0));
		/// ���ʂ�ǉ�
		void addPlane(const Vector &normal, const Vector &initial_pos, float mass, const Vector &initial_local_inertia = VectorC(0,0,0));

		bool onUpdate(float delta);
		/// �X�V�X���b�g(postupdatesignal�p)
		MemberSlot1<PhysicWorld, float /*delta*/, &PhysicWorld::onUpdate> postupdate_slot;

	GCTP_DECLARE_CLASS
	TUKI_DECLARE(PhysicWorld)
	
	protected:
		virtual bool doOnUpdate(float delta);

		bool setUp(luapp::Stack &L);
		void attach(luapp::Stack &L);
		void detach(luapp::Stack &L);

	private:
		/// �R�s�[�s��
		PhysicWorld(PhysicWorld &);
		mutable PhysicWorldImpl *impl_;
#ifdef _MT
		mutable Mutex monitor_;
#endif
	};

}} // namespace gctp::scene

#endif // _GCTP_SCENE_PHYSICWORLD_HPP_