/** @file
 * GameCatapult 車両クラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: modeltag.cpp,v 1.3 2005/02/21 02:21:12 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/scene/vehicle.hpp>
#include <gctp/scene/physicworld.hpp>
#include <gctp/scene/entity.hpp>
#include <btBulletDynamicsCommon.h>

using namespace std;

namespace gctp { namespace scene {

	class VehicleImpl : public Object {
	public:
		VehicleImpl() : vehicle_raycaster_(0), vehicle_(0)
		{
		}

		~VehicleImpl()
		{
			if(vehicle_) delete vehicle_;
			if(vehicle_raycaster_) delete vehicle_raycaster_;
		}

		/// セットアップ
		HRslt setUp(btRigidBody *chassis, btDynamicsWorld *world)
		{
			vehicle_raycaster_ = new btDefaultVehicleRaycaster(world);
			vehicle_ = new btRaycastVehicle(tuning_, chassis, vehicle_raycaster_);

			// never deactivate the vehicle
			chassis->setActivationState(DISABLE_DEACTIVATION);

			//choose coordinate system
			vehicle_->setCoordinateSystem(0, 1, 2);

			world->addVehicle(vehicle_);
			return S_OK;
		}
		
		void getSizeAndOffset(Vector &out_size, Vector &out_offset)
		{
			btRigidBody *rigid_body = vehicle_->getRigidBody();
			btCompoundShape *shape = reinterpret_cast<btCompoundShape *>(rigid_body->getCollisionShape());
			btBoxShape *chassis = reinterpret_cast<btBoxShape *>(shape->getChildShape(0));
			const btVector3 &size = chassis->getHalfExtentsWithoutMargin();
			btTransform	chassis_local = shape->getChildTransform(0);
			const btVector3 &offset = chassis_local.getOrigin();

			out_size.x = size.x();
			out_size.y = size.y();
			out_size.z = size.z();

			out_offset.x = offset.x();
			out_offset.y = offset.y();
			out_offset.z = offset.z();
		}

		void addWheel(Vector pos, bool is_front_wheel
			, float wheel_radius, float wheel_width, float wheel_friction
			, float suspension_stiffness, float suspension_damping, float suspension_compression, float suspension_restlen
			, float roll_influence)
		{
			btVector3 wheel_direction(0,-1,0);
			btVector3 wheel_axle(1,0,0);

			btVector3 connection_point = btVector3(pos.x, pos.y, pos.z);
			btWheelInfo &wheel = vehicle_->addWheel(connection_point, wheel_direction, wheel_axle, suspension_restlen, wheel_radius, tuning_, is_front_wheel);
			wheel.m_suspensionStiffness = suspension_stiffness;
			wheel.m_wheelsDampingRelaxation = suspension_damping;
			wheel.m_wheelsDampingCompression = suspension_compression;
			wheel.m_frictionSlip = wheel_friction;
			wheel.m_rollInfluence = roll_influence;
		}

		btRaycastVehicle::btVehicleTuning	tuning_;
		btDefaultVehicleRaycaster			*vehicle_raycaster_;
		btRaycastVehicle					*vehicle_;
	};



	HRslt Vehicle::setUp(btRigidBody *chassis, btDynamicsWorld *world)
	{
		impl_ = new VehicleImpl;
		return impl_->setUp(chassis, world);
	}

	/*void Vehicle::addWheel(Vector pos, bool is_front_wheel
			, float wheel_radius, float wheel_width, float wheel_friction
			, float suspension_stiffness, float suspension_damping, float suspension_compression, float suspension_restlen
			, float roll_influence)
	{
		impl_->addWheel(pos, is_front_wheel, wheel_radius, wheel_width, wheel_friction
			, suspension_stiffness, suspension_damping, suspension_compression, suspension_restlen, roll_influence);
	}*/

	void Vehicle::update()
	{
		impl_->vehicle_->applyEngineForce(engine_force, 2);
		impl_->vehicle_->setBrake(breaking_force, 2);
		impl_->vehicle_->applyEngineForce(engine_force, 3);
		impl_->vehicle_->setBrake(breaking_force, 3);

		impl_->vehicle_->setSteeringValue(steering, 0);
		impl_->vehicle_->setSteeringValue(steering, 1);
	}

	const btRaycastVehicle *Vehicle::vehicle() const
	{
		if(impl_) return impl_->vehicle_;
		return 0;
	}

	bool Vehicle::setUp(luapp::Stack &L)
	{
		// Context:createで製作する
		return false;
	}

	void Vehicle::makeup(luapp::Stack &L)
	{
		if(L.top() >= 2) {
			Pointer<PhysicWorld> physic = tuki_cast<PhysicWorld>(L[1]);
			Pointer<Entity> entity = tuki_cast<Entity>(L[2]);
			if(physic && entity) {
				btRigidBody *chassis = physic->getRigidBody(entity->target()->root());
				if(chassis) {
					setUp(chassis, physic->getDynamicsWorld());
				}
			}
		}
	}

	int Vehicle::getEngineForce(luapp::Stack &L)
	{
		L << engine_force;
		return 1;
	}

	void Vehicle::setEngineForce(luapp::Stack &L)
	{
		if(L.top() >= 1) engine_force = (float)L[1].toNumber();
	}

	int Vehicle::getBreakingForce(luapp::Stack &L)
	{
		L << breaking_force;
		return 1;
	}

	void Vehicle::setBreakingForce(luapp::Stack &L)
	{
		if(L.top() >= 1) breaking_force = (float)L[1].toNumber();
	}

	int Vehicle::getSteering(luapp::Stack &L)
	{
		L << steering;
		return 1;
	}

	void Vehicle::setSteering(luapp::Stack &L)
	{
		if(L.top() >= 1) steering = (float)L[1].toNumber();
	}

	void Vehicle::update(luapp::Stack &L)
	{
		update();
	}

	void Vehicle::reset(luapp::Stack &L)
	{
		btRigidBody *chassis = impl_->vehicle_->getRigidBody();
		btTransform start_transform;
		start_transform.setIdentity();
		if(L.top()>=3) {
			btVector3 p((float)L[1].toNumber(), (float)L[2].toNumber(), (float)L[3].toNumber());
			start_transform.setOrigin(p);
		}
		chassis->setCenterOfMassTransform(start_transform);
		chassis->setLinearVelocity(btVector3(0,0,0));
		chassis->setAngularVelocity(btVector3(0,0,0));
		impl_->vehicle_->resetSuspension();
		for (int i = 0; i < impl_->vehicle_->getNumWheels(); i++)
		{
			//synchronize the wheels with the (interpolated) chassis worldtransform
			impl_->vehicle_->updateWheelTransform(i, true);
		}
	}

	void Vehicle::addWheel(luapp::Stack &L)
	{
		if(L.top()>=12) {
			impl_->addWheel(VectorC((float)L[1].toNumber(), (float)L[2].toNumber(), (float)L[3].toNumber()), L[4].toBoolean()
				, (float)L[5].toNumber(), (float)L[6].toNumber(), (float)L[7].toNumber()
				, (float)L[8].toNumber(), (float)L[9].toNumber(), (float)L[10].toNumber(), (float)L[11].toNumber()
				, (float)L[12].toNumber());
		}
	}

	int Vehicle::getWheelRotation(luapp::Stack &L)
	{
		if(L.top()>=1) {
			btWheelInfo &wheel = impl_->vehicle_->getWheelInfo(L[1].toInteger());
			L << wheel.m_rotation << wheel.m_deltaRotation;
			return 2;
		}
		return 0;
	}

	int Vehicle::getSizeAndOffset(luapp::Stack &L)
	{
		Vector size, offset;
		impl_->getSizeAndOffset(size, offset);
		L << size.x << size.y << size.z << offset.x << offset.y << offset.z;
		return 6;
	}

	int Vehicle::speed(luapp::Stack &L)
	{
		L << impl_->vehicle_->getCurrentSpeedKmHour();
		return 1;
	}

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, Vehicle, Object);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, Vehicle)
		TUKI_METHOD(Vehicle, makeup)
		TUKI_METHOD(Vehicle, getEngineForce)
		TUKI_METHOD(Vehicle, setEngineForce)
		TUKI_METHOD(Vehicle, getBreakingForce)
		TUKI_METHOD(Vehicle, setBreakingForce)
		TUKI_METHOD(Vehicle, getSteering)
		TUKI_METHOD(Vehicle, setSteering)
		TUKI_METHOD(Vehicle, update)
		TUKI_METHOD(Vehicle, reset)
		TUKI_METHOD(Vehicle, addWheel)
		TUKI_METHOD(Vehicle, getWheelRotation)
		TUKI_METHOD(Vehicle, getSizeAndOffset)
		TUKI_METHOD(Vehicle, speed)
	TUKI_IMPLEMENT_END(Vehicle)

}} // namespace gctp::scene
