/** @file
 * GameCatapult í«ê’ÉJÉÅÉâÉNÉâÉX
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/chasecamera.hpp>
#include <gctp/scene/camera.hpp>
#include <gctp/scene/entity.hpp>
#include <gctp/skeleton.hpp>
#include <gctp/app.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace scene {

	ChaseCamera::ChaseCamera() : AspectUpdater<Object>(-1, 0x10), dumping_factor(0.3f), position_offset(VectorC(0, 0, 0)), posture_offset(VectorC(0, 0, 0))
	{
	}

	bool ChaseCamera::doUpdate(float delta)
	{
		Pointer<Camera> target = target_.lock();
		if(target) {
			Matrix trgmtx = chasee_->val.wtm()*QuatC(posture_offset.y, posture_offset.x, posture_offset.z).toMatrix().setPos(position_offset);
			Matrix newmtx;
			newmtx.set2PInterpolation(target->node()->val.wtm(), trgmtx, dumping_factor);
			target->node()->val.updateWTM(newmtx);
		}
		return true;
	}
	
	bool ChaseCamera::LuaCtor(luapp::Stack &L)
	{
		// Context:createÇ≈êªçÏÇ∑ÇÈ
		return false;
	}

	void ChaseCamera::attach(luapp::Stack &L)
	{
		if(L.top() >= 2) {
			target_ = tuki_cast<Camera>(L[1]);
			Pointer<StrutumNode> node = tuki_cast<StrutumNode>(L[2]);
			chasee_ = node;
		}
		else {
			target_ = 0;
			chasee_ = 0;
		}
	}

	void ChaseCamera::setPositionOffset(luapp::Stack &L)
	{
		if(L.top() >= 3) {
			position_offset = VectorC((float)L[1].toNumber(), (float)L[2].toNumber(), (float)L[3].toNumber());
		}
	}

	void ChaseCamera::setPostureOffset(luapp::Stack &L)
	{
		if(L.top() >= 3) {
			posture_offset = VectorC((float)L[2].toNumber(), (float)L[1].toNumber(), (float)L[3].toNumber());
		}
	}

	void ChaseCamera::setDumpingFactor(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			dumping_factor = (float)L[1].toNumber();
		}
	}

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, ChaseCamera, Object);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, ChaseCamera)
		GCTP_SCENE_ASPECTUPDATER_TUKI_METHODS(ChaseCamera)
		TUKI_METHOD(ChaseCamera, attach)
		TUKI_METHOD(ChaseCamera, setPositionOffset)
		TUKI_METHOD(ChaseCamera, setPostureOffset)
		TUKI_METHOD(ChaseCamera, setDumpingFactor)
	TUKI_IMPLEMENT_END(ChaseCamera)

}} // namespace gctp::scene
