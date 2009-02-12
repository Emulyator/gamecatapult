/** @file
 * GameCatapult 物理シミュワールドクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/physicworld.hpp>
#include <gctp/scene/physiccontroller.hpp>
#include <gctp/scene/body.hpp>
#include <gctp/scene/flesh.hpp>
#include <gctp/scene/world.hpp>
#include <gctp/scene/attrmodel.hpp>
#include <gctp/scene/graphfile.hpp>
#include <gctp/scene/entity.hpp>
#include <gctp/graphic/model.hpp>
#include <gctp/context.hpp>
#include <gctp/app.hpp>
#include <btBulletDynamicsCommon.h> // for Bullet

extern ContactProcessedCallback	gContactProcessedCallback;
extern ContactAddedCallback		gContactAddedCallback;
extern ContactDestroyedCallback	gContactDestroyedCallback;

#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace scene {

	class PhysicMotionState : public btMotionState {
	public:
		PhysicMotionState(StrutumTree::NodePtr node, const Matrix &offset)
		{
			this->node = node;
			this->offset = offset;
			if(node) {
				Matrix m = offset.inverse()*node->val.lcm();
				//Matrix m = offset.inverse()*body->root()->val.wtm();
				xform.setFromOpenGLMatrix(&m._11);
			}
			else {
				Matrix m = offset.inverse();
				xform.setFromOpenGLMatrix(&m._11);
			}
		}

		PhysicMotionState(const btTransform &initialpos)
		{
			offset.identify();
			xform = initialpos;
		}

		virtual void getWorldTransform(btTransform &xform) const
		{
			xform = this->xform;
		}

		virtual void setWorldTransform(const btTransform &xform)
		{
			if(node) {
				StrutumTree::NodePtr _node = node.lock();
				Matrix mat;
				xform.getOpenGLMatrix(&mat._11);
				_node->val.updateWTM(offset*mat);
			}
			this->xform = xform;
		}

		StrutumTree::NodeHndl node;
		Matrix offset;
		btTransform xform;
	};

	struct PhysicWorldImpl {
		btDynamicsWorld								*world_;
		btDefaultCollisionConfiguration				*collision_configuration_;
		btCollisionDispatcher						*dispatcher_;
		btConstraintSolver							*solver_;
		btBroadphaseInterface						*overlapping_pair_cache_;
		btAlignedObjectArray<btCollisionShape *>	collision_shapes_;
		uint max_bodies_;
		int max_substeps_;
		
		PhysicWorldImpl() : world_(0), collision_configuration_(0), dispatcher_(0), solver_(0), overlapping_pair_cache_(0), max_substeps_(1)
		{
			gContactProcessedCallback = onContactProcessed;
			gContactAddedCallback = onContactAdded;
			gContactDestroyedCallback = onContactDestroyed;
		}

		~PhysicWorldImpl()
		{
			tearDown();
		}

		void setUp(const AABox &aabb, uint max_bodies)
		{
			tearDown();
			max_bodies_ = max_bodies;

			collision_configuration_ = new btDefaultCollisionConfiguration();

			dispatcher_ = new btCollisionDispatcher(collision_configuration_);
			
			// シミュレーションするShapeの限度
			overlapping_pair_cache_ = new btAxisSweep3(*(btVector3 *)&aabb.lower, *(btVector3 *)&aabb.upper, max_bodies);

			// ソルバー
			solver_ = new btSequentialImpulseConstraintSolver;

			// シミュレーションするワールドの生成
			world_ = new btDiscreteDynamicsWorld(dispatcher_, overlapping_pair_cache_, solver_, collision_configuration_);

			world_->setGravity(btVector3(0, -9.80665f, 0));

			// GIMPACT登録
			//btCollisionDispatcher *dispatcher = static_cast<btCollisionDispatcher *>(world_->getDispatcher());
			//btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);
		}

		void tearDown()
		{
			if(world_) {
				for(int i = world_->getNumCollisionObjects()-1; i >= 0; i--)
				{
					btCollisionObject *obj = world_->getCollisionObjectArray()[i];
					if(obj->getUserPointer()) {
						world_->removeCollisionObject( obj );
					}
					else {
						btRigidBody *body = btRigidBody::upcast(obj);
						if(body && body->getMotionState())
						{
							delete body->getMotionState();
						}
						world_->removeCollisionObject( obj );
						delete obj;
					}
				}

				for(int i = 0; i < collision_shapes_.size(); i++)
				{
					delete collision_shapes_[i];
				}
				collision_shapes_.clear();

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
				// シミュレーションを進める
				world_->stepSimulation(delta, max_substeps_);
			}
		}

		// 箱追加
		// これ適当だから後で変えるように
		void addBox2(Handle<Body> body, const Vector &initial_pos, float mass, const Vector &initial_velocity)
		{
			if(body && world_->getNumCollisionObjects() < (int)max_bodies_) {
				AABox aabb = body->fleshies().front()->model()->getAABB();
				btBoxShape *box = new btBoxShape(btVector3((aabb.upper.x-aabb.lower.x)/4,(aabb.upper.y-aabb.lower.y)/2,(aabb.upper.z-aabb.lower.z)/4));
				collision_shapes_.push_back(box);
				
				btVector3 local_inertia(0, 0, 0);
				if(mass > 0) box->calculateLocalInertia(mass, local_inertia);

				//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
				btRigidBody::btRigidBodyConstructionInfo rbinfo(mass, new PhysicMotionState(body->root(), Matrix().trans(-aabb.center())), box, local_inertia);
				btRigidBody *rigid_body = new btRigidBody(rbinfo);

				world_->addRigidBody(rigid_body);

				if(mass > 0) rigid_body->setLinearVelocity(*(btVector3 *)&initial_velocity);
			}
		}

		// 箱追加
		// これ適当だから後で変えるように
		void addBox(Handle<Body> body, float mass, const Vector &initial_velocity)
		{
			if(body && world_->getNumCollisionObjects() < (int)max_bodies_) {
				AABox aabb = body->fleshies().front()->model()->getAABB(body->fleshies().front()->node()->val.wtm());
				for(PointerList<Flesh>::iterator i = ++body->fleshies().begin(); i != body->fleshies().end(); ++i)
				{
					aabb |= (*i)->model()->getAABB((*i)->node()->val.wtm());
				}
				btBoxShape *box = new btBoxShape(btVector3((aabb.upper.x-aabb.lower.x)/2,(aabb.upper.y-aabb.lower.y)/2,(aabb.upper.z-aabb.lower.z)/2));
				collision_shapes_.push_back(box);

				btVector3 local_inertia(0, 0, 0);
				if(mass > 0) box->calculateLocalInertia(mass, local_inertia);

				//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
				btRigidBody::btRigidBodyConstructionInfo rbinfo(mass, new PhysicMotionState(body->root(), Matrix().trans(-aabb.center())), box, local_inertia);
				btRigidBody *rigid_body = new btRigidBody(rbinfo);

				world_->addRigidBody(rigid_body);

				if(mass > 0) rigid_body->setLinearVelocity(*(btVector3 *)&initial_velocity);
			}
		}

		// 箱追加
		// これ適当だから後で変えるように
		void addBox(Handle<Body> body, Handle<Body> src, float mass, const Vector &initial_velocity)
		{
			if(body && world_->getNumCollisionObjects() < (int)max_bodies_) {
				AABox aabb = src->fleshies().front()->model()->getAABB(src->fleshies().front()->node()->val.wtm());
				for(PointerList<Flesh>::iterator i = ++src->fleshies().begin(); i != src->fleshies().end(); ++i)
				{
					aabb |= (*i)->model()->getAABB((*i)->node()->val.wtm());
				}
				btBoxShape *box = new btBoxShape(btVector3((aabb.upper.x-aabb.lower.x)/2,(aabb.upper.y-aabb.lower.y)/2,(aabb.upper.z-aabb.lower.z)/2));
				collision_shapes_.push_back(box);

				btVector3 local_inertia(0, 0, 0);
				if(mass > 0) box->calculateLocalInertia(mass, local_inertia);

				//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
				btRigidBody::btRigidBodyConstructionInfo rbinfo(mass, new PhysicMotionState(body->root(), Matrix().trans(-aabb.center())), box, local_inertia);
				btRigidBody *rigid_body = new btRigidBody(rbinfo);

				world_->addRigidBody(rigid_body);

				if(mass > 0) rigid_body->setLinearVelocity(*(btVector3 *)&initial_velocity);
			}
		}

		// 箱追加
		// これ適当だから後で変えるように
		void addBoxAsCompound(Handle<Body> body, Handle<Body> src, float mass, const Vector &center_of_mass, const Vector &box_margin, const Vector &initial_velocity)
		{
			if(body && world_->getNumCollisionObjects() < (int)max_bodies_) {
				AABox aabb = src->fleshies().front()->model()->getAABB(src->fleshies().front()->node()->val.wtm());
				for(PointerList<Flesh>::iterator i = ++src->fleshies().begin(); i != src->fleshies().end(); ++i)
				{
					aabb |= (*i)->model()->getAABB((*i)->node()->val.wtm());
				}
				btBoxShape *box = new btBoxShape(btVector3((aabb.upper.x-aabb.lower.x)/2+box_margin.x,(aabb.upper.y-aabb.lower.y)/2+box_margin.y,(aabb.upper.z-aabb.lower.z)/2+box_margin.z));
				collision_shapes_.push_back(box);

				btCompoundShape* compound = new btCompoundShape;
				collision_shapes_.push_back(compound);
				
				btTransform local_trans;
				local_trans.setIdentity();
				//local_trans effectively shifts the center of mass with respect to the chassis
				local_trans.setOrigin(btVector3(-center_of_mass.x, -center_of_mass.y, -center_of_mass.z));
				compound->addChildShape(local_trans, box);

				btVector3 local_inertia(0, 0, 0);
				if(mass > 0) compound->calculateLocalInertia(mass, local_inertia);

				//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
				btRigidBody::btRigidBodyConstructionInfo rbinfo(mass, new PhysicMotionState(body->root(), Matrix().trans(-aabb.center()-center_of_mass)), compound, local_inertia);
				btRigidBody *rigid_body = new btRigidBody(rbinfo);

				world_->addRigidBody(rigid_body);

				if(mass > 0) rigid_body->setLinearVelocity(*(btVector3 *)&initial_velocity);
			}
		}

		// 箱追加
		// これ適当だから後で変えるように
		void addBoxAsCompound2(Handle<Body> body, float mass, const Vector &center_of_mass, const Vector &box_size, const Vector &offset, const Vector &initial_velocity)
		{
			if(body && world_->getNumCollisionObjects() < (int)max_bodies_) {
				btBoxShape *box = new btBoxShape(btVector3(box_size.x,box_size.y,box_size.z));
				collision_shapes_.push_back(box);

				btCompoundShape* compound = new btCompoundShape;
				collision_shapes_.push_back(compound);
				
				btTransform local_trans;
				local_trans.setIdentity();
				//local_trans effectively shifts the center of mass with respect to the chassis
				local_trans.setOrigin(btVector3(-center_of_mass.x, -center_of_mass.y, -center_of_mass.z));
				compound->addChildShape(local_trans, box);

				btVector3 local_inertia(0, 0, 0);
				if(mass > 0) compound->calculateLocalInertia(mass, local_inertia);

				//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
				btRigidBody::btRigidBodyConstructionInfo rbinfo(mass, new PhysicMotionState(body->root(), Matrix().trans(offset-center_of_mass)), compound, local_inertia);
				btRigidBody *rigid_body = new btRigidBody(rbinfo);

				world_->addRigidBody(rigid_body);

				if(mass > 0) rigid_body->setLinearVelocity(*(btVector3 *)&initial_velocity);
			}
		}

		// 地平面の生成
		void addPlane(const Vector &normal, const Vector &initial_pos, float mass, const Vector &initial_velocity)
		{
			if(world_->getNumCollisionObjects() < (int)max_bodies_) {
				btCollisionShape *col_shape = new btStaticPlaneShape(*(btVector3 *)&normal,0);
				collision_shapes_.push_back(col_shape);

				btTransform ground_transform;
				ground_transform.setIdentity();
				ground_transform.setOrigin(*(btVector3 *)&initial_pos);

				btVector3 local_inertia(0, 0, 0);
				if(mass > 0) col_shape->calculateLocalInertia(mass, local_inertia);
				// using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
				btDefaultMotionState *my_motion_state = new btDefaultMotionState(ground_transform);
				btRigidBody::btRigidBodyConstructionInfo rbinfo(mass, my_motion_state, col_shape, local_inertia);
				btRigidBody *rigid_body = new btRigidBody(rbinfo);

				// add the body to the dynamics world
				world_->addRigidBody(rigid_body);
				
				if(mass > 0) rigid_body->setLinearVelocity(*(btVector3 *)&initial_velocity);
			}
		}

		// ポリゴン当たり制作
		void addMesh(Handle<AttrFlesh> attr, float mass, const Vector &initial_velocity)
		{
			if(attr && world_->getNumCollisionObjects() < (int)max_bodies_) {
				const btCollisionShape *col_shape = attr->model()->shape();

				btVector3 local_inertia(0, 0, 0);
				if(mass > 0) col_shape->calculateLocalInertia(mass, local_inertia);

				// using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
				btRigidBody::btRigidBodyConstructionInfo rbinfo(mass, new PhysicMotionState(attr->node().lock(), MatrixC(true)), const_cast<btCollisionShape *>(col_shape), local_inertia);
				btRigidBody *rigid_body = new btRigidBody(rbinfo);

				// add the body to the dynamics world
				world_->addRigidBody(rigid_body);

				if(mass > 0) rigid_body->setLinearVelocity(*(btVector3 *)&initial_velocity);
			}
		}

		static bool onContactAdded(btManifoldPoint &cp, const btCollisionObject *colobj0, int partid0, int index0, const btCollisionObject* colobj1, int partid1, int index1)
		{
			Object *obj = (Object *)colobj0->getUserPointer();
			if(obj) {
				Pointer<PhysicController> pc = obj;
				if(pc) pc->onContactAdded(cp, colobj0, partid0, index0, colobj1, partid1, index1);
			}
			obj = (Object *)colobj1->getUserPointer();
			if(obj) {
				Pointer<PhysicController> pc = obj;
				if(pc) pc->onContactAdded(cp, colobj1, partid1, index1, colobj0, partid0, index0);
			}
			//dbgout << "onContactAdded " << cp.getDistance() << ", "
			//	<< colobj0 << ", " << partid0 << ", " << index0 << ", " << colobj1 << ", " << partid1 << ", " << index1 << endl;
			return true;
		}

		static bool onContactProcessed(btManifoldPoint &cp, void *body0, void *body1)
		{
			//btRigidBody *rbody0 = btRigidBody::upcast((btCollisionObject *)body0);
			//btRigidBody *rbody1 = btRigidBody::upcast((btCollisionObject *)body1);

			//dbgout << "onContactProcessed" << endl;
			return true;
		}
		
		static bool onContactDestroyed(void *user_persistent_data)
		{
			dbgout << "onContactDestroyed" << endl;
			return true;
		}
	};

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, PhysicWorld, Object);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, PhysicWorld)
		TUKI_METHOD(PhysicWorld, makeup)
		TUKI_METHOD(PhysicWorld, addBox)
		TUKI_METHOD(PhysicWorld, addBoxAsCompound)
		TUKI_METHOD(PhysicWorld, addBoxAsCompound2)
		TUKI_METHOD(PhysicWorld, addMesh)
		TUKI_METHOD(PhysicWorld, attach)
		TUKI_METHOD(PhysicWorld, detach)
		TUKI_METHOD(PhysicWorld, numBodies)
	TUKI_IMPLEMENT_END(PhysicWorld)

	PhysicWorld::PhysicWorld() : update_slot(SLOT_PRI)
	{
		update_slot.bind(this);
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

	void PhysicWorld::load(const _TCHAR *filename, uint max_bodies)
	{
		if(filename) {
			Pointer<GraphFile> file = context()[filename].lock();
			//Pointer<GraphFile> file = context()load(filename).lock();
			// どっちがいいんだろう。。。
			if(file) {
				AABox box(VectorC(10, 10, 10), VectorC(-10, -10, -10));
				Pointer<Body> pbody;
				for(GraphFile::iterator i = file->begin(); i != file->end(); ++i) {
					pbody = *i;
					if(pbody) {
						for(PointerList<AttrFlesh>::iterator i = pbody->attributes().begin(); i != pbody->attributes().end(); ++i) {
							box |= (*i)->getAABB();
						}
					}
				}

				PRNN("physic world aabb : "<<box);
				setUp(box, max_bodies);

				for(GraphFile::iterator i = file->begin(); i != file->end(); ++i) {
					pbody = *i;
					if(pbody) {
						for(PointerList<AttrFlesh>::iterator i = pbody->attributes().begin(); i != pbody->attributes().end(); ++i) {
							addMesh(*i, 0);
						}
					}
				}
			}
		}
	}

	bool PhysicWorld::canAdd()
	{
		return impl_->world_->getNumCollisionObjects() < (int)impl_->max_bodies_;
	}
	
	int PhysicWorld::numBodies()
	{
		return impl_->world_->getNumCollisionObjects();
	}

	int PhysicWorld::getMaxSubSteps()
	{
		return impl_->max_substeps_;
	}

	void PhysicWorld::setMaxSubSteps(int max_substeps)
	{
		impl_->max_substeps_ = max_substeps;
	}

	void PhysicWorld::addPlane(const Vector &normal, const Vector &initial_pos, float mass, const Vector &initial_local_inertia)
	{
		impl_->addPlane(normal, initial_pos, mass, initial_local_inertia);
	}

	void PhysicWorld::addBox(Handle<Body> body, float mass, const Vector &initial_local_inertia)
	{
		impl_->addBox(body, mass, initial_local_inertia);
	}

	void PhysicWorld::addBox(Handle<Body> body, Handle<Body> src, float mass, const Vector &initial_local_inertia)
	{
		impl_->addBox(body, src, mass, initial_local_inertia);
	}

	void PhysicWorld::addBox2(Handle<Body> body, const Vector &initial_pos, float mass, const Vector &initial_local_inertia)
	{
		impl_->addBox2(body, initial_pos, mass, initial_local_inertia);
	}

	void PhysicWorld::addMesh(Handle<AttrFlesh> attr, float mass, const Vector &initial_local_inertia)
	{
		impl_->addMesh(attr, mass, initial_local_inertia);
	}
	
	btMotionState *PhysicWorld::createMotionState(StrutumTree::NodeHndl node, const Matrix &offset)
	{
		return new PhysicMotionState(node.lock(), offset);
	}
	
	Matrix *PhysicWorld::getOffsetMatrix(btRigidBody *body)
	{
		if(body && body->getMotionState()) {
			PhysicMotionState *ms = dynamic_cast<PhysicMotionState *>(body->getMotionState());
			if(ms) return &ms->offset;
		}
		return 0;
	}

	const Matrix *PhysicWorld::getOffsetMatrix(const btRigidBody *body)
	{
		if(body && body->getMotionState()) {
			const PhysicMotionState *ms = dynamic_cast<const PhysicMotionState *>(body->getMotionState());
			if(ms) return &ms->offset;
		}
		return 0;
	}

	btRigidBody *PhysicWorld::getRigidBody(StrutumTree::NodeHndl node)
	{
		StrutumTree::NodePtr _node = node.lock();
		for(int i = impl_->world_->getNumCollisionObjects()-1; i >= 0; i--) {
			btCollisionObject *obj = impl_->world_->getCollisionObjectArray()[i];
			btRigidBody *body = btRigidBody::upcast(obj);
			if(body && body->getMotionState()) {
				PhysicMotionState *ms = dynamic_cast<PhysicMotionState *>(body->getMotionState());
				if(ms) {
					if(ms->node.get() == _node.get()) return body;
				}
			}
		}
		return 0;
	}

	btDynamicsWorld *PhysicWorld::getDynamicsWorld()
	{
		return impl_->world_;
	}

	const btDynamicsWorld *PhysicWorld::getDynamicsWorld() const
	{
		return impl_->world_;
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
		Profiling physics_profile("physics");
		//impl_->feedback();
		impl_->update(delta);
		//impl_->sync();
		return true;
	}

	bool PhysicWorld::setUp(luapp::Stack &L)
	{
		// Context:createで製作する
		return false;
	}

	void PhysicWorld::makeup(luapp::Stack &L)
	{
		if(L.top() >= 7) {
			AABox aabb;
			aabb.upper.x = (float)L[1].toNumber();
			aabb.upper.y = (float)L[2].toNumber();
			aabb.upper.z = (float)L[3].toNumber();
			aabb.lower.x = (float)L[4].toNumber();
			aabb.lower.y = (float)L[5].toNumber();
			aabb.lower.z = (float)L[6].toNumber();
			setUp(aabb, L[7].toInteger());
		}
		else if(L.top() >= 2) {
#ifdef UNICODE
			load(WCStr(L[1].toCStr()).c_str(), L[2].toInteger());
#else
			load(L[1].toCStr(), L[2].toInteger());
#endif
		}
	}

	void PhysicWorld::addBox(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			Pointer<Entity> entity = tuki_cast<Entity>(L[1]);
			if(entity) {
				if(L.top() >= 5) {
					addBox(entity->target(), entity->source(), (float)L[2].toNumber(), VectorC((float)L[3].toNumber(), (float)L[4].toNumber(), (float)L[5].toNumber()));
				}
				else if(L.top() >= 2) {
					addBox(entity->target(), entity->source(), (float)L[2].toNumber());
				}
				else {
					addBox(entity->target(), entity->source(), 0);
				}
			}
		}
	}

	void PhysicWorld::addBoxAsCompound(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			Pointer<Entity> entity = tuki_cast<Entity>(L[1]);
			if(entity) {
				if(L.top() >= 11) {
					impl_->addBoxAsCompound(entity->target(), entity->source(), (float)L[2].toNumber()
						, VectorC((float)L[3].toNumber(), (float)L[4].toNumber(), (float)L[5].toNumber())
						, VectorC((float)L[6].toNumber(), (float)L[7].toNumber(), (float)L[8].toNumber())
						, VectorC((float)L[9].toNumber(), (float)L[10].toNumber(), (float)L[11].toNumber()));
				}
				else if(L.top() >= 8) {
					impl_->addBoxAsCompound(entity->target(), entity->source(), (float)L[2].toNumber()
						, VectorC((float)L[3].toNumber(), (float)L[4].toNumber(), (float)L[5].toNumber())
						, VectorC((float)L[6].toNumber(), (float)L[7].toNumber(), (float)L[8].toNumber())
						, VectorC(0, 0, 0));
				}
				else if(L.top() >= 5) {
					impl_->addBoxAsCompound(entity->target(), entity->source(), (float)L[2].toNumber()
						, VectorC((float)L[3].toNumber(), (float)L[4].toNumber(), (float)L[5].toNumber())
						, VectorC(0, 0, 0)
						, VectorC(0, 0, 0));
				}
				else if(L.top() >= 2) {
					impl_->addBoxAsCompound(entity->target(), entity->source(), (float)L[2].toNumber()
						, VectorC(0, 0, 0)
						, VectorC(0, 0, 0)
						, VectorC(0, 0, 0));
				}
				else {
					impl_->addBoxAsCompound(entity->target(), entity->source(), 0
						, VectorC(0, 0, 0)
						, VectorC(0, 0, 0)
						, VectorC(0, 0, 0));
				}
			}
		}
	}

	void PhysicWorld::addBoxAsCompound2(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			Pointer<Entity> entity = tuki_cast<Entity>(L[1]);
			if(entity) {
				if(L.top() >= 14) {
					impl_->addBoxAsCompound2(entity->target(), (float)L[2].toNumber()
						, VectorC((float)L[3].toNumber(), (float)L[4].toNumber(), (float)L[5].toNumber())
						, VectorC((float)L[6].toNumber(), (float)L[7].toNumber(), (float)L[8].toNumber())
						, VectorC((float)L[9].toNumber(), (float)L[10].toNumber(), (float)L[11].toNumber())
						, VectorC((float)L[12].toNumber(), (float)L[13].toNumber(), (float)L[14].toNumber()));
				}
				else if(L.top() >= 11) {
					impl_->addBoxAsCompound2(entity->target(), (float)L[2].toNumber()
						, VectorC((float)L[3].toNumber(), (float)L[4].toNumber(), (float)L[5].toNumber())
						, VectorC((float)L[6].toNumber(), (float)L[7].toNumber(), (float)L[8].toNumber())
						, VectorC((float)L[9].toNumber(), (float)L[10].toNumber(), (float)L[11].toNumber())
						, VectorC(0, 0, 0));
				}
				else if(L.top() >= 8) {
					impl_->addBoxAsCompound2(entity->target(), (float)L[2].toNumber()
						, VectorC((float)L[3].toNumber(), (float)L[4].toNumber(), (float)L[5].toNumber())
						, VectorC((float)L[6].toNumber(), (float)L[7].toNumber(), (float)L[8].toNumber())
						, VectorC(0, 0, 0)
						, VectorC(0, 0, 0));
				}
				else if(L.top() >= 5) {
					impl_->addBoxAsCompound2(entity->target(), (float)L[2].toNumber()
						, VectorC((float)L[3].toNumber(), (float)L[4].toNumber(), (float)L[5].toNumber())
						, VectorC(0, 0, 0)
						, VectorC(0, 0, 0)
						, VectorC(0, 0, 0));
				}
				else if(L.top() >= 2) {
					impl_->addBoxAsCompound2(entity->target(), (float)L[2].toNumber()
						, VectorC(0, 0, 0)
						, VectorC(0, 0, 0)
						, VectorC(0, 0, 0)
						, VectorC(0, 0, 0));
				}
				else {
					impl_->addBoxAsCompound2(entity->target(), 0
						, VectorC(0, 0, 0)
						, VectorC(0, 0, 0)
						, VectorC(0, 0, 0)
						, VectorC(0, 0, 0));
				}
			}
		}
	}

	void PhysicWorld::addMesh(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			Pointer<GraphFile> file = tuki_cast<GraphFile>(L[1]);
			if(file) {
				Pointer<Body> pbody;
				for(GraphFile::iterator i = file->begin(); i != file->end(); ++i) {
					pbody = *i;
					if(pbody) {
						for(PointerList<AttrFlesh>::iterator i = pbody->attributes().begin(); i != pbody->attributes().end(); ++i) {
							addMesh(*i, 0);
						}
					}
				}
			}
		}
	}

	void PhysicWorld::attach(luapp::Stack &L)
	{
		//これじゃだめ
		if(L.top() >= 1) {
			Pointer<World> world = tuki_cast<World>(L[1]);
			if(world) world->update_signal.connectOnce(update_slot);
		}
	}

	void PhysicWorld::detach(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			Pointer<World> world = tuki_cast<World>(L[1]);
			if(world) world->update_signal.disconnect(update_slot);
		}
	}

	int PhysicWorld::numBodies(luapp::Stack &L)
	{
		L << numBodies();
		return 1;
	}

	void PhysicWorld::setMaxSubSteps(luapp::Stack &L)
	{
		setMaxSubSteps(L[1].toInteger());
	}

	int PhysicWorld::getMaxSubSteps(luapp::Stack &L)
	{
		L << getMaxSubSteps();
		return 1;
	}

}} // namespace gctp::scene
