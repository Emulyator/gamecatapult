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
#include <gctp/scene/stage.hpp>
#include <gctp/input.hpp>
#include <gctp/app.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace scene {

	QuakeLikeCamera::QuakeLikeCamera() : update_slot(1), yaw_(0), pitch_(0), speed_(5.0f), enable_(false)
	{
		update_slot.bind(this);
	}

	void QuakeLikeCamera::enter(Stage &stage)
	{
		stage.update_signal.connectOnce(update_slot);
	}
	
	void QuakeLikeCamera::exit(Stage &stage)
	{
		stage.update_signal.disconnect(update_slot);
	}

	bool QuakeLikeCamera::setEnable(bool _enable)
	{
		if(enable_ != _enable) {
			(std::swap)(enable_, _enable);
			if(enable_ && target_) {
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
		return _enable;
	}

	bool QuakeLikeCamera::update(float delta)
	{
		if(enable_ && target_) {
			Matrix m;

			Pointer<Camera> target = target_.lock();

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
			m.rot(yaw_, pitch_, 0);
			target->node()->val.getLCM().at() = m.transformVector(VectorC(0, 0, 1));
			target->node()->val.getLCM().up() = m.transformVector(VectorC(0, 1, 0));

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
			dir = m.rotY(yaw_).transformVector(dir);
			target->node()->val.getLCM().position() += dir.normalize()*speed_*delta;
		}
		return true;
	}

	bool QuakeLikeCamera::setUp(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			target_ = tuki_cast<Camera>(L[1]);
		}
		return true;
	}

	void QuakeLikeCamera::enter(luapp::Stack &L)
	{
		if(L.top() >= 2) {
			Pointer<Stage> stage = tuki_cast<Stage>(L[2]);
			if(stage) enter(*stage);
		}
	}

	void QuakeLikeCamera::exit(luapp::Stack &L)
	{
		if(L.top() >= 2) {
			Pointer<Stage> stage = tuki_cast<Stage>(L[2]);
			if(stage) exit(*stage);
		}
	}

	void QuakeLikeCamera::setEnable(luapp::Stack &L)
	{
		if(L.top() >= 2) {
			setEnable(L[2].toBoolean());
		}
	}

	GCTP_IMPLEMENT_CLASS_NS(gctp, QuakeLikeCamera, Object);
	TUKI_IMPLEMENT_BEGIN_NS(Scene, QuakeLikeCamera)
		TUKI_METHOD(QuakeLikeCamera, enter)
		TUKI_METHOD(QuakeLikeCamera, exit)
		TUKI_METHOD(QuakeLikeCamera, setEnable)
	TUKI_IMPLEMENT_END(QuakeLikeCamera)

}} // namespace gctp::scene
