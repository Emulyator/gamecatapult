#ifndef _GCTP_SCENE_UPDATER_HPP_
#define _GCTP_SCENE_UPDATER_HPP_
/** @file
 * GameCatapult 更新者基底クラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/signal.hpp>
#include <gctp/tuki.hpp>

namespace gctp { namespace scene {

	/** 更新者基底クラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2004/02/16 1:06:15
	 */
	class Updater : public Object
	{
	public:
		/// コンストラクタ
		Updater(int16_t priority = 0, int16_t mask = 0xFF);

		/// 更新
		bool update(float delta);
		/// 更新スロット
		MemberSlot1<Updater, float /*delta*/, &Updater::update> update_slot;

		/// Appのupdateシグナルに連結
		virtual void activate(bool yes);

	protected:
		virtual bool doUpdate(float delta) = 0;

		bool setUp(luapp::Stack &L);
		void activate(luapp::Stack &L);
		void setPriority(luapp::Stack &L);

		GCTP_DECLARE_TYPEINFO;
		TUKI_DECLARE(Updater);
	};

}} // namespace gctp::scene

#endif //_GCTP_SCENE_UPDATER_HPP_