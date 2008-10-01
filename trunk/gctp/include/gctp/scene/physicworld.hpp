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
#include <gctp/matrix.hpp>
#include <gctp/aabox.hpp>
#include <gctp/strutumtree.hpp>
#ifdef _MT
#include <gctp/mutex.hpp>
#endif

class btRigidBody;
class btCollisionObject;
class btDynamicsWorld;

namespace gctp { namespace scene {

	class World;
	class Body;
	class AttrFlesh;

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
		enum {
			SLOT_PRI = 3000,
		};
		/// �R���X�g���N�^
		PhysicWorld();
		/// �f�X�g���N�^
		virtual ~PhysicWorld();

		/// Bullet�I�u�W�F�N�g����
		void setUp(const AABox &aabb, uint max_bodies);
		/// GraphFile����n�`�R���W�����ǉ�
		void load(const _TCHAR *filename, uint max_bodies);
		/// �ǉ��\���H
		bool canAdd();
		/// ���ݑ��݂��镨�̂̐�
		int numBodies();

		/// Body�𔠂Ƃ��Ēǉ�
		void addBox2(Handle<Body> body, const Vector &initial_pos, float mass, const Vector &initial_velocity = VectorC(0,0,0));
		/// Body�𔠂Ƃ��Ēǉ�
		void addBox(Handle<Body> body, float mass, const Vector &initial_velocity = VectorC(0,0,0));
		/// Body�𔠂Ƃ��Ēǉ�
		void addBox(Handle<Body> body, Handle<Body> src, float mass, const Vector &initial_velocity = VectorC(0,0,0));
		/// ���ʂ�ǉ�
		void addPlane(const Vector &normal, const Vector &initial_pos, float mass, const Vector &initial_velocity = VectorC(0,0,0));
		/// AttrFlesh�ǉ�
		void addMesh(Handle<AttrFlesh> attr, float mass, const Vector &initial_velocity = VectorC(0,0,0));

		/// ����RigidBody��StrutumNode��R�t��
		void bind(StrutumTree::NodeHndl node, btRigidBody *body, const Matrix &offset = MatrixC(true));
		/// RigidBody��StrutumNode�̕R�t������
		void unbind(const btRigidBody *body);

		/// StrutumNode�Ɋ֘A�t����ꂽbtRigidBody���擾
		btRigidBody *getRigidBody(StrutumTree::NodeHndl node);
		/// StrutumNode�Ɋ֘A�t����ꂽbtRigidBody���擾
		const btRigidBody *getRigidBody(StrutumTree::NodeHndl node) const;
		/// btRigidBody�Ɋ֘A�t����ꂽ��StrutumNode�擾
		StrutumTree::NodeHndl getStrutumNode(const btRigidBody *body) const;
		/// btRigidBody�Ɋ֘A�t����ꂽ���I�t�Z�b�g�s����擾
		Matrix *getOffsetMatrix(const btRigidBody *body);
		/// btRigidBody�Ɋ֘A�t����ꂽ���I�t�Z�b�g�s����擾
		const Matrix *getOffsetMatrix(const btRigidBody *body) const;

		/// btDynamicsWorld���擾
		btDynamicsWorld *getDynamicsWorld();

		bool onUpdate(float delta);
		/// �X�V�X���b�g
		MemberSlot1<PhysicWorld, float /*delta*/, &PhysicWorld::onUpdate> update_slot;
	
	protected:
		virtual bool doOnUpdate(float delta);

		bool setUp(luapp::Stack &L);
		void makeup(luapp::Stack &L);
		void addBox(luapp::Stack &L);
		void addBoxAsCompound(luapp::Stack &L);
		void addBoxAsCompound2(luapp::Stack &L);
		void attach(luapp::Stack &L);
		void detach(luapp::Stack &L);
		int numBodies(luapp::Stack &L);

	private:
		/// �R�s�[�s��
		PhysicWorld(PhysicWorld &);
		mutable PhysicWorldImpl *impl_;
#ifdef _MT
		mutable Mutex monitor_;
#endif
		GCTP_DECLARE_CLASS;
		TUKI_DECLARE(PhysicWorld);
	};

}} // namespace gctp::scene

#endif // _GCTP_SCENE_PHYSICWORLD_HPP_