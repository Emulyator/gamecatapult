#ifndef _GCTP_SCENE_PHYSICWORLD_HPP_
#define _GCTP_SCENE_PHYSICWORLD_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 物理シミュワールドクラスヘッダファイル
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

	/** 物理シミュワールドクラス
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
		/// コンストラクタ
		PhysicWorld();
		/// デストラクタ
		virtual ~PhysicWorld();

		/// Bulletオブジェクト制作
		void setUp(const AABox &aabb, uint max_bodies);
		/// GraphFileから地形コリジョン追加
		void load(const _TCHAR *filename, uint max_bodies);
		/// 追加可能か？
		bool canAdd();
		/// 現在存在する物体の数
		int numBodies();

		/// Bodyを箱として追加
		void addBox2(Handle<Body> body, const Vector &initial_pos, float mass, const Vector &initial_velocity = VectorC(0,0,0));
		/// Bodyを箱として追加
		void addBox(Handle<Body> body, float mass, const Vector &initial_velocity = VectorC(0,0,0));
		/// Bodyを箱として追加
		void addBox(Handle<Body> body, Handle<Body> src, float mass, const Vector &initial_velocity = VectorC(0,0,0));
		/// 平面を追加
		void addPlane(const Vector &normal, const Vector &initial_pos, float mass, const Vector &initial_velocity = VectorC(0,0,0));
		/// AttrFlesh追加
		void addMesh(Handle<AttrFlesh> attr, float mass, const Vector &initial_velocity = VectorC(0,0,0));

		/// 既存RigidBodyとStrutumNodeを紐付け
		void bind(StrutumTree::NodeHndl node, btRigidBody *body, const Matrix &offset = MatrixC(true));
		/// RigidBodyとStrutumNodeの紐付け解除
		void unbind(const btRigidBody *body);

		/// StrutumNodeに関連付けられたbtRigidBodyを取得
		btRigidBody *getRigidBody(StrutumTree::NodeHndl node);
		/// StrutumNodeに関連付けられたbtRigidBodyを取得
		const btRigidBody *getRigidBody(StrutumTree::NodeHndl node) const;
		/// btRigidBodyに関連付けられたをStrutumNode取得
		StrutumTree::NodeHndl getStrutumNode(const btRigidBody *body) const;
		/// btRigidBodyに関連付けられたをオフセット行列を取得
		Matrix *getOffsetMatrix(const btRigidBody *body);
		/// btRigidBodyに関連付けられたをオフセット行列を取得
		const Matrix *getOffsetMatrix(const btRigidBody *body) const;

		/// btDynamicsWorldを取得
		btDynamicsWorld *getDynamicsWorld();

		bool onUpdate(float delta);
		/// 更新スロット
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
		/// コピー不可
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