/** @file
 * GameCatapult 更新者基底クラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/physiccontroller.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace scene {

	bool PhysicController::onContactAdded(btManifoldPoint &cp, const btCollisionObjectWrapper *colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper *colObj1Wrap, int partId1, int index1)
	{
		return true;
	}

	bool PhysicController::onContactProcessed(btManifoldPoint &, void *, void *)
	{
		return true;
	}
	
	bool PhysicController::onContactDestroyed(void *)
	{
		return true;
	}

	GCTP_IMPLEMENT_TYPEINFO(PhysicController, Object);

}} // namespace gctp::scene
