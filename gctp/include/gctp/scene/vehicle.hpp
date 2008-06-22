#ifndef _GCTP_SCENE_VEHICLE_HPP_
#define _GCTP_SCENE_VEHICLE_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 属性情報モデルクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/28 12:42:22
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/tuki.hpp>

class btDynamicsWorld;
class btRigidBody;
class btRaycastVehicle;

namespace gctp { namespace scene {

	class VehicleImpl;
	/** 車両クラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2008/06/21 17:55:36
	 * Copyright (C) 2008 SAM (T&GG, Org.). All rights reserved.
	 */
	class Vehicle : public Object {
	public:
		Vehicle() : engine_force(0), breaking_force(0), steering(0) {}

		/// セットアップ
		HRslt setUp(btRigidBody *chassis, btDynamicsWorld *world);

		/*void addWheel(Vector pos, bool is_front_wheel
			, float wheel_radius, float wheel_width, float wheel_friction
			, float suspension_stiffness, float suspension_damping, float suspension_compression, float suspension_restlen
			, float roll_influence);*/

		void update();

		const btRaycastVehicle *vehicle() const;
		
		float	engine_force;
		float	breaking_force;
		float	steering;

	protected:
		bool setUp(luapp::Stack &L);
		void makeup(luapp::Stack &L);
		int getEngineForce(luapp::Stack &L);
		void setEngineForce(luapp::Stack &L);
		int getBreakingForce(luapp::Stack &L);
		void setBreakingForce(luapp::Stack &L);
		int getSteering(luapp::Stack &L);
		void setSteering(luapp::Stack &L);
		void update(luapp::Stack &L);
		void reset(luapp::Stack &L);
		void addWheel(luapp::Stack &L);
		int getSizeAndOffset(luapp::Stack &L);
		int speed(luapp::Stack &L);

	private:
		Pointer<VehicleImpl> impl_;
		
		GCTP_DECLARE_CLASS;
		TUKI_DECLARE(Vehicle);
	};

}} //namespace gctp::scene

#endif //_GCTP_SCENE_VEHICLE_HPP_