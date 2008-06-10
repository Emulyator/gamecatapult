/** @file
 * GameCatapult 物理シミュワールドクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/physicworld.hpp>
#include <gctp/scene/body.hpp>
#include <gctp/scene/flesh.hpp>
#include <gctp/scene/world.hpp>
#include <gctp/graphic/model.hpp>
#include <gctp/app.hpp>
#include <btBulletDynamicsCommon.h> // for Bullet
#include <GIMPACT/Bullet/btGImpactCollisionAlgorithm.h>
#include <btGImpactConvexDecompositionShape.h>

#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace scene {

	struct PhysicWorldImpl {
		btDynamicsWorld								*world_;
		btDefaultCollisionConfiguration				*collision_configuration_;
		btCollisionDispatcher						*dispatcher_;
		btConstraintSolver							*solver_;
		btBroadphaseInterface						*overlapping_pair_cache_;
		btAlignedObjectArray<btCollisionShape *>	collision_shapes_;
		
		HandleList<Body>							sync_targets_;
		uint max_bodies_;

		PhysicWorldImpl() : world_(0), collision_configuration_(0), dispatcher_(0), solver_(0), overlapping_pair_cache_(0)
		{
		}

		~PhysicWorldImpl()
		{
			tearDown();
		}

		void setUp(const AABox &aabb, uint max_bodies)
		{
			tearDown();

			collision_configuration_ = new btDefaultCollisionConfiguration();

			dispatcher_ = new btCollisionDispatcher(collision_configuration_);
			
			// シミュレーションするShapeの限度
			overlapping_pair_cache_ = new btAxisSweep3(*(btVector3 *)&aabb.lower, *(btVector3 *)&aabb.upper, max_bodies);
			max_bodies_ = max_bodies;

			// ソルバー
			btSequentialImpulseConstraintSolver *solver = new btSequentialImpulseConstraintSolver;
			solver_ = solver;

			// シミュレーションするワールドの生成
			world_ = new btDiscreteDynamicsWorld(dispatcher_, overlapping_pair_cache_, solver_, collision_configuration_);

			// GIMPACT登録
			btCollisionDispatcher *dispatcher = static_cast<btCollisionDispatcher *>(world_->getDispatcher());
			btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);
		}

		void tearDown()
		{
			if(world_) {
				for(int i = world_->getNumCollisionObjects()-1; i >= 0; i--)
				{
					btCollisionObject *obj = world_->getCollisionObjectArray()[i];
					btRigidBody *body = btRigidBody::upcast(obj);
					if(body && body->getMotionState())
					{
						delete body->getMotionState();
					}
					world_->removeCollisionObject( obj );
					delete obj;
				}

				for(int i = 0; i < collision_shapes_.size(); i++)
				{
					delete collision_shapes_[i];
				}

				delete overlapping_pair_cache_;
				delete dispatcher_;
				delete solver_;
				delete world_;
				delete collision_configuration_;

				world_ = 0;
			}
		}

		void update(float delta)
		{
			if(world_) {
				Profiling physics_profile("physics");
				// シミュレーションを進める
				world_->stepSimulation(app().lap);
			}
		}

		void sync()
		{
			if(world_) {
				// 結果を適用
				int ii = 1;
				for(HandleList<scene::Body>::iterator i = sync_targets_.begin(); i != sync_targets_.end() && ii < world_->getCollisionObjectArray().size(); ++i, ++ii)
				{
					float mat[16];
					world_->getCollisionObjectArray()[ii]->getWorldTransform().getOpenGLMatrix(mat);
					memcpy(&(*i)->root()->val.getLCM(), mat, sizeof(float)*16);
					AABox aabb = (*i)->fleshies().front()->model()->getAABB();
					(*i)->root()->val.getLCM() = Matrix().trans(-aabb.center())*(*i)->root()->val.lcm();
				}
			}
		}

		// 箱追加
		void addBox(Handle<Body> body, const Vector &initial_pos, float mass, const Vector &initial_local_inertia)
		{
			if(body && world_->getCollisionObjectArray().size() < (int)max_bodies_) {
				sync_targets_.push_back(body);
				AABox aabb = body->fleshies().front()->model()->getAABB();

				btCollisionShape *col_shape = new btBoxShape(btVector3((aabb.upper.x-aabb.lower.x)/4,(aabb.upper.y-aabb.lower.y)/2,(aabb.upper.z-aabb.lower.z)/4));
				collision_shapes_.push_back(col_shape);
				
				btTransform start_transform;
				start_transform.setIdentity();

				btVector3 local_inertia(initial_local_inertia.x, initial_local_inertia.y, initial_local_inertia.z);
				if(mass > 0) col_shape->calculateLocalInertia(mass, local_inertia);

				start_transform.setOrigin(*(btVector3 *)&initial_pos);

				//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
				btDefaultMotionState *my_motion_state = new btDefaultMotionState(start_transform);
				btRigidBody::btRigidBodyConstructionInfo rbinfo(mass, my_motion_state, col_shape, local_inertia);
				btRigidBody *rigid_body = new btRigidBody(rbinfo);

				world_->addRigidBody(rigid_body);
			}
		}

		// 地平面の生成
		void addPlane(const Vector &normal, const Vector &initial_pos, float mass, const Vector &initial_local_inertia)
		{
			if(world_->getCollisionObjectArray().size() < (int)max_bodies_) {
				btCollisionShape *col_shape = new btStaticPlaneShape(*(btVector3 *)&normal,0);
				collision_shapes_.push_back(col_shape);

				btTransform ground_transform;
				ground_transform.setIdentity();
				ground_transform.setOrigin(*(btVector3 *)&initial_pos);

				btVector3 local_inertia(initial_local_inertia.x, initial_local_inertia.y, initial_local_inertia.z);
				if(mass > 0) col_shape->calculateLocalInertia(mass, local_inertia);
				// using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
				btDefaultMotionState *my_motion_state = new btDefaultMotionState(ground_transform);
				btRigidBody::btRigidBodyConstructionInfo rbinfo(mass, my_motion_state, col_shape, local_inertia);
				btRigidBody *body = new btRigidBody(rbinfo);

				// add the body to the dynamics world
				world_->addRigidBody(body);
			}
		}
		// ポリゴン当たり制作
		void addMesh(const graphic::Model &model, const Vector &initial_pos, float mass, const Vector &initial_local_inertia)
		{
			const void *vd = model.vertexbuffer().lock(0,0);
			const short *id = (const short *)model.indexbuffer().lock(0,0);
			int *_id = new int[model.indexbuffer().indexNum()];
			for(uint i = 0; i < model.indexbuffer().indexNum(); i++) {
				_id[i] = id[i];
			}

			btTriangleIndexVertexArray *m_indexVertexArrays2 = new btTriangleIndexVertexArray(
				model.indexbuffer().indexNum()*3,
				_id,
				3*sizeof(int),
				model.vertexbuffer().numVerticies(),
				(btScalar *)vd,
				model.vertexbuffer().fvf().stride());

			btGImpactConvexDecompositionShape *trimesh2 = new btGImpactConvexDecompositionShape(
				m_indexVertexArrays2, btVector3(4.f,4.f,4.f),btScalar(0.01));

			trimesh2->updateBound();

			model.vertexbuffer().unlock();
			model.indexbuffer().unlock();

			delete m_indexVertexArrays2;
			delete trimesh2;
		}

	};

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, PhysicWorld, Object);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, PhysicWorld)
		TUKI_METHOD(PhysicWorld, attach)
		TUKI_METHOD(PhysicWorld, detach)
	TUKI_IMPLEMENT_END(PhysicWorld)

	PhysicWorld::PhysicWorld()
	{
		postupdate_slot.bind(this);
		impl_ = new PhysicWorldImpl;
	}

	PhysicWorld::~PhysicWorld()
	{
		delete impl_;
	}

	void PhysicWorld::setUp(const AABox &aabb, uint max_bodies)
	{
		impl_->setUp(aabb, max_bodies);
	}

	bool PhysicWorld::canAdd()
	{
		return impl_->world_->getCollisionObjectArray().size() < (int)impl_->max_bodies_;
	}
	
	void PhysicWorld::addPlane(const Vector &normal, const Vector &initial_pos, float mass, const Vector &initial_local_inertia)
	{
		impl_->addPlane(normal, initial_pos, mass, initial_local_inertia);
	}

	void PhysicWorld::addBox(Handle<Body> body, const Vector &initial_pos, float mass, const Vector &initial_local_inertia)
	{
		impl_->addBox(body, initial_pos, mass, initial_local_inertia);
	}

	/** シーン更新
	 *
	 * 更新シグナルと衝突解決シグナル発行
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/11 2:34:02
	 */	
	bool PhysicWorld::onUpdate(float delta)
	{
#ifdef _MT
		ScopedLock al(monitor_);
#endif
		return doOnUpdate(delta);
	}

	bool PhysicWorld::doOnUpdate(float delta)
	{
		impl_->update(delta);
		impl_->sync();
		return true;
	}

	bool PhysicWorld::setUp(luapp::Stack &L)
	{
		// Context:createで製作する
		return false;
	}

	void PhysicWorld::attach(luapp::Stack &L)
	{
		//これじゃだめ
		if(L.top() >= 1) {
			Pointer<World> world = tuki_cast<World>(L[1]);
			if(world) world->postupdate_signal.connectOnce(postupdate_slot);
		}
	}

	void PhysicWorld::detach(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			Pointer<World> world = tuki_cast<World>(L[1]);
			if(world) world->postupdate_signal.disconnect(postupdate_slot);
		}
	}

}} // namespace gctp::scene
