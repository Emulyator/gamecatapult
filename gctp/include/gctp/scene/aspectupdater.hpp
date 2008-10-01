#ifndef _GCTP_SCENE_ASPECTUPDATER_HPP_
#define _GCTP_SCENE_ASPECTUPDATER_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 更新者基底クラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/signal.hpp>
#include <gctp/app.hpp>
#include <gctp/tuki.hpp>

namespace gctp { namespace scene {

	/** 更新者基底クラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2004/02/16 1:06:15
	 */
	template<class T>
	class AspectUpdater : public T
	{
	public:
		/// コンストラクタ
		AspectUpdater(int16_t priority = 0, int16_t mask = 0xFF)
		{
			update_slot.setMask(mask);
			update_slot.bind(this);
		}

		/// 更新
		bool update(float delta)
		{
			return doUpdate(delta);
		}
		/// 更新スロット
		MemberSlot1<AspectUpdater, float /*delta*/, &AspectUpdater::update> update_slot;

		/// Appのupdateシグナルに連結
		virtual void activate(bool yes)
		{
			if(yes) app().update_signal.connectOnce(update_slot);
			else app().update_signal.disconnect(update_slot);
		}

	protected:
		virtual bool doUpdate(float delta) = 0;

		void activate(luapp::Stack &L)
		{
			if(L.top() >= 1) {
				activate(L[1].toBoolean());
			}
		}
		void setPriority(luapp::Stack &L)
		{
			if(L.top() >= 1) {
				update_slot.setPriority((uint16_t)L[1].toInteger());
			}
		}
	};

}} // namespace gctp::scene

#define GCTP_SCENE_ASPECTUPDATER_TUKI_METHODS(_Class)	\
	TUKI_METHOD(_Class, activate)		\
	TUKI_METHOD(_Class, setPriority)

#endif //_GCTP_SCENE_ASPECTUPDATER_HPP_