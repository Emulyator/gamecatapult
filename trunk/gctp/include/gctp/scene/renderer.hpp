#ifndef _GCTP_SCENE_RENDERER_HPP_
#define _GCTP_SCENE_RENDERER_HPP_
/** @file
 * GameCatapult レンダリング要素クラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 18:43:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/object.hpp>
#include <gctp/class.hpp>

namespace gctp { namespace scene {

	/** 描画処理クラス
	 *
	 * これいらない。Camera/Stageの実装が素直になった結果、Light系しか意味が無く、
	 * しかもLightなどの環境情報は各FreshがStageに問い合わせるしかないと思う。
	 */
	class Renderer : public Object
	{
	public:
		/// コンストラクタ
		Renderer() {}

		/// ノード到達時の処理(返り値は、子を実行するか否か)
		virtual bool onEnter() const { return true; }
		/// ノード離脱時の処理(返り値は、実行を続行するか否か)
		virtual bool onLeave() const { return true; }

	GCTP_DEFINE_TYPEINFO(Renderer, Object)
	};

}} // namespace gctp::scene

#endif //_GCTP_SCENE_RENDERER_HPP_