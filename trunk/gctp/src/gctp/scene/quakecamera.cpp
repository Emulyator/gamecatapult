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

	QuakeCamera::QuakeCamera() : update_slot(1), yaw_(0), pitch_(0), speed_(5.0f)
	{
		update_slot.bind(this);
		update_slot.setMask(0x10);
	}

	void QuakeCamera::activate(bool yes)
	{
		if(yes) {
			app().update_signal.connectOnce(update_slot);
			if(target_) {
				Pointer<Camera> target = target_.lock();
				if(target->node()) {
					yaw_ = atan2f(target->node()->val.lcm().at().x, target->node()->val.lcm().at().z);
					Matrix m;
					m.rotY(-yaw_);
					Vector v = m.transformVector(target->node()->val.lcm().at());
					pitch_ = atan2f(-v.y, v.z);
				}
			}
		}
		else app().update_signal.disconnect(update_slot);
	}

	bool QuakeCamera::update(float delta)
	{
		Pointer<Camera> target = target_.lock();
		if(target) {
			Stance newstance = target->stance();
			float neck_speed = 0.01f;
#ifdef GCTP_COORD_RH
			yaw_ += -neck_speed*input().mouse().dx;
#else
			yaw_ += neck_speed*input().mouse().dx;
#endif
			if(yaw_ > g_pi) yaw_ -= (((int)yaw_/g_pi)+1)*g_pi;
			else if(yaw_ < -g_pi) yaw_ -= (((int)yaw_/g_pi)-1)*g_pi;
			pitch_ += neck_speed*input().mouse().dy;
			if(pitch_ > g_pi/2) pitch_ = g_pi/2;
			else if(pitch_ < -g_pi/2) pitch_ = -g_pi/2;
			target->fov() += 0.0005f*input().mouse().dz;
			if(target->fov() > g_pi) target->fov() = g_pi;
			if(target->fov() < 0) target->fov() = 0;
			float roll = 0;
			if(input().kbd().press(DIK_Q)) roll += 1.0f;
			if(input().kbd().press(DIK_E)) roll -= 1.0f;
			newstance.posture = QuatC(yaw_, pitch_, roll);

			if(input().kbd().push(DIK_PGUP)) speed_ += 1.0f;
			if(input().kbd().push(DIK_PGDN)) speed_ -= 1.0f;

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
			dir = Quat().rotY(yaw_).transform(dir);
			newstance.position += dir.normalize()*speed_*delta;

			target->setStance(newstance);
		}
		return true;
	}

	bool QuakeCamera::setUp(luapp::Stack &L)
	{
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

	void QuakeCamera::activate(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			activate(L[1].toBoolean());
		}
	}

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, QuakeCamera, Object);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, QuakeCamera)
		TUKI_METHOD(QuakeCamera, attach)
		TUKI_METHOD(QuakeCamera, activate)
	TUKI_IMPLEMENT_END(QuakeCamera)

}} // namespace gctp::scene
