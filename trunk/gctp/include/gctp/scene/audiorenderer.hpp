#ifndef _GCTP_SCENE_AUDIORENDERER_HPP_
#define _GCTP_SCENE_AUDIORENDERER_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 簡易効果ノードクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/tuki.hpp>
#include <gctp/scene/renderer.hpp>

namespace gctp { namespace scene {

	class World;
	
	/** Wolrd内の3Dサウンド更新ノードクラス
	 *
	 * 現在のカメラをリスナーとして3Dサウンドパラメータを更新する
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 1:05:32
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class AudioRenderer : public Renderer
	{
	public:
		virtual bool onReach(float delta) const;
		virtual bool onLeave(float delta) const;
	
		/// 描画対象設定
		void attach(Handle<World> target) { target_ = target; }
		/// ワールド取得
		Handle<World> target() const { return target_; }

	protected:
		Handle<World> target_;

		bool setUp(luapp::Stack &L);
		void attach(luapp::Stack &L);

		GCTP_DECLARE_CLASS;
		TUKI_DECLARE(AudioRenderer);
	};

}} // namespace gctp::scene

#endif // _GCTP_SCENE_AUDIORENDERER_HPP_