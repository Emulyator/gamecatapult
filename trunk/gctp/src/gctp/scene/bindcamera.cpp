/** @file
 * GameCatapult ノードにくっつくカメラクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/bindcamera.hpp>
#include <gctp/scene/camera.hpp>
#include <gctp/scene/entity.hpp>
#include <gctp/skeleton.hpp>
#include <gctp/app.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace scene {

	BindCamera::BindCamera() : AspectUpdater<Object>(1, 0x10), position_offset(VectorC(0, 0, 0)), posture_offset(VectorC(0, 0, 0))
	{
	}

	bool BindCamera::doUpdate(float delta)
	{
		Pointer<Camera> target = target_.lock();
		if(target) {
			Stance chasee_stance = chasee_->val.wtm().orthoNormal();
			chasee_stance.position += chasee_stance.posture.transform(position_offset);
			chasee_stance.posture *= QuatC(posture_offset.y, posture_offset.x, posture_offset.z);

			Stance oldstance = target->node()->val.wtm().orthoNormal();
			Stance newstance;
			newstance.set2PInterpolation(oldstance, chasee_stance, 0.5f);
			target->node()->val.updateWTM(newstance.toMatrix());
		}
		return true;
	}
	
	bool BindCamera::LuaCtor(luapp::Stack &L)
	{
		// Context:createで製作する
		return false;
	}

	void BindCamera::attach(luapp::Stack &L)
	{
		if(L.top() >= 2) {
			target_ = tuki_cast<Camera>(L[1]);
			Pointer<Entity> entity = tuki_cast<Entity>(L[2]);
			if(entity) {
				if(L.top() >= 3) {
					chasee_ = entity->skeleton()[L[3].toCStr()];
				}
				else {
					chasee_ = entity->skeleton().root();
				}
			}
		}
		else {
			target_ = 0;
			chasee_ = 0;
		}
	}

	void BindCamera::setPositionOffset(luapp::Stack &L)
	{
		if(L.top() >= 3) {
			position_offset = VectorC((float)L[1].toNumber(), (float)L[2].toNumber(), (float)L[3].toNumber());
		}
	}

	void BindCamera::setPostureOffset(luapp::Stack &L)
	{
		if(L.top() >= 3) {
			posture_offset = VectorC((float)L[2].toNumber(), (float)L[1].toNumber(), (float)L[3].toNumber());
		}
	}

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, BindCamera, Object);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, BindCamera)
		GCTP_SCENE_ASPECTUPDATER_TUKI_METHODS(BindCamera)
		TUKI_METHOD(BindCamera, attach)
		TUKI_METHOD(BindCamera, setPositionOffset)
		TUKI_METHOD(BindCamera, setPostureOffset)
	TUKI_IMPLEMENT_END(BindCamera)

}} // namespace gctp::scene
