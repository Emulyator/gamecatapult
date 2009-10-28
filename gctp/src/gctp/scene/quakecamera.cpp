/** @file
 * GameCatapult Quakeå^ÉJÉÅÉâÉNÉâÉX
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/quakecamera.hpp>
#include <gctp/scene/camera.hpp>
#include <gctp/input.hpp>
#include <gctp/app.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace scene {

	QuakeCamera::QuakeCamera() : AspectUpdater<Object>(1, 0x10), yaw(0), pitch(0), speed(5.0f)
	{
	}

	void QuakeCamera::activate(bool yes)
	{
		AspectUpdater<Object>::activate(yes);
		if(yes) {
			Pointer<Camera> target = target_.lock();
			if(target) {
				Matrix mtx = target->node()->val.wtm();
				yaw = atan2f(mtx.at().x, mtx.at().z);
				Matrix m;
				m.rotY(-yaw);
				Vector v = m.transformVector(mtx.at());
				pitch = atan2f(-v.y, v.z);
			}
		}
	}

	bool QuakeCamera::doUpdate(float delta)
	{
		Pointer<Camera> target = target_.lock();
		if(target) {
			Stance newstance = target->node()->val.wtm();
			float neck_speed = 0.01f;
#ifdef GCTP_COORD_RH
			yaw += -neck_speed*input().mouse().dx;
#else
			yaw += neck_speed*input().mouse().dx;
#endif
			if(yaw > g_pi) yaw -= (((int)yaw/g_pi)+1)*g_pi;
			else if(yaw < -g_pi) yaw -= (((int)yaw/g_pi)-1)*g_pi;
			pitch += neck_speed*input().mouse().dy;
			if(pitch > g_pi/2) pitch = g_pi/2;
			else if(pitch < -g_pi/2) pitch = -g_pi/2;
			target->fov() += 0.0005f*input().mouse().dz;
			if(target->fov() > g_pi) target->fov() = g_pi;
			if(target->fov() < 0) target->fov() = 0;
			float roll = 0;
			if(input().kbd().press(DIK_Q)) roll += 1.0f;
			if(input().kbd().press(DIK_E)) roll -= 1.0f;
			newstance.posture = QuatC(yaw, pitch, roll);

			if(input().kbd().push(DIK_PGUP)) speed += 1.0f;
			if(input().kbd().push(DIK_PGDN)) speed -= 1.0f;

			Vector dir = {0, 0, 0};
			if(input().kbd().press(DIK_W)) dir.z += 1.0f;
			if(input().kbd().press(DIK_S)) dir.z -= 1.0f;
#ifdef GCTP_COORD_RH
			if(input().kbd().press(DIK_D)) dir.x += -1.0f;
			if(input().kbd().press(DIK_A)) dir.x -= -1.0f;
#else
			if(input().kbd().press(DIK_D)) dir.x += 1.0f;
			if(input().kbd().press(DIK_A)) dir.x -= 1.0f;
#endif
			if(input().kbd().press(DIK_SPACE)) dir.y += 1.0f;
			if(input().kbd().press(DIK_LCONTROL)) dir.y -= 1.0f;
			dir = Quat().rotY(yaw).transform(dir);
			float l = dir.length();
			if(l > 0) newstance.position += (dir/l)*speed*delta;

			target->node()->val.updateWTM(newstance.toMatrix());
		}
		return true;
	}
	
	bool QuakeCamera::LuaCtor(luapp::Stack &L)
	{
		// Context:createÇ≈êªçÏÇ∑ÇÈ
		return false;
	}

	void QuakeCamera::attach(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			target_ = tuki_cast<Camera>(L[1]);
		}
		else {
			target_ = 0;
		}
	}

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, QuakeCamera, Object);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, QuakeCamera)
		GCTP_SCENE_ASPECTUPDATER_TUKI_METHODS(QuakeCamera)
		TUKI_METHOD(QuakeCamera, attach)
	TUKI_IMPLEMENT_END(QuakeCamera)

}} // namespace gctp::scene
