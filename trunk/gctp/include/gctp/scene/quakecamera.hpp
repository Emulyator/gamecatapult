#ifndef _GCTP_SCENE_QUAKECAMERA_HPP_
#define _GCTP_SCENE_QUAKECAMERA_HPP_
/** @file
 * GameCatapult Quake型カメラクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/signal.hpp>
#include <gctp/tuki.hpp>

namespace gctp { namespace scene {

	class Camera;
	class Stage;
	/** Quake型カメラ
	 *
	 * シーンカメラノードにアタッチするビヘイビア
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 8:10:04
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class QuakeCamera : public Object
	{
	public:
		float yaw_;
		float pitch_;
		float speed_;

		/// 更新
		bool update(float delta);
		/// 更新スロット
		MemberSlot1<QuakeCamera, float /*delta*/, &QuakeCamera::update> update_slot;

		QuakeCamera();

		Handle<Camera> &target() { return target_; }
		Handle<Camera> target() const { return target_; }

		void activate(bool yes);

	protected:
		Handle<Camera> target_;

		bool setUp(luapp::Stack &L);
		void attach(luapp::Stack &L);
		void activate(luapp::Stack &L);

	GCTP_DECLARE_CLASS
	TUKI_DECLARE(QuakeCamera)
	};

}} // namespace gctp::scene

#endif //_GCTP_SCENE_QUAKECAMERA_HPP_