/** @file
 * GameCatapult 更新者基底クラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/updater.hpp>
#include <gctp/app.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace scene {

	Updater::Updater(int16_t priority, int16_t mask) : update_slot(priority)
	{
		update_slot.setMask(mask);
		update_slot.bind(this);
	}

	void Updater::activate(bool yes)
	{
		if(yes) app().update_signal.connectOnce(update_slot);
		else app().update_signal.disconnect(update_slot);
	}

	bool Updater::update(float delta)
	{
		return doUpdate(delta);
	}
	
	bool Updater::setUp(luapp::Stack &L)
	{
		return false;
	}

	void Updater::activate(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			activate(L[1].toBoolean());
		}
	}

	void Updater::setPriority(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			update_slot.setPriority((uint16_t)L[1].toInteger());
		}
	}

	GCTP_IMPLEMENT_TYPEINFO(Updater, Object);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, Updater)
		TUKI_METHOD(Updater, activate)
		TUKI_METHOD(Updater, setPriority)
	TUKI_IMPLEMENT_END(Updater)

}} // namespace gctp::scene
