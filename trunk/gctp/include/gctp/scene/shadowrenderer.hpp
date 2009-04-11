#ifndef _GCTP_SCENE_SHADOWRENDERER_HPP_
#define _GCTP_SCENE_SHADOWRENDERER_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult シャドウマップ描画ノードクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/tuki.hpp>
#include <gctp/handlelist.hpp>
#include <gctp/scene/renderer.hpp>
#include <gctp/scene/drawpacket.hpp>

namespace gctp {
	namespace graphic {
		class RenderTarget;
		namespace dx {
			class StateBlockRsrc;
		}
	}
}

namespace gctp { namespace scene {

	class WorldSorter;
	
	/** シャドウマップ描画ノードクラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 1:05:32
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class ShadowWorldRenderer : public Renderer
	{
	public:
		ShadowWorldRenderer();

		virtual bool onReach(float delta) const;
		virtual bool onLeave(float delta) const;

		void attach(Handle<WorldSorter> target)
		{
			target_ = target;
		}

	protected:
		Handle<WorldSorter> target_;

		bool LuaCtor(luapp::Stack &L);
		void attach(luapp::Stack &L);
		int texture(luapp::Stack &L);
	
	private:
		Pointer<graphic::dx::StateBlockRsrc> sb_;
		mutable Pointer<graphic::RenderTarget> rt_;

		GCTP_DECLARE_CLASS;
		TUKI_DECLARE(ShadowWorldRenderer);
	};

	/** シャドウマップを使った不透明体描画ノードクラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 1:05:32
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class OpequeShadowedWorldRenderer : public Renderer
	{
	public:
		OpequeShadowedWorldRenderer();

		virtual bool onReach(float delta) const;
		virtual bool onLeave(float delta) const;

		void attach(Handle<WorldSorter> target)
		{
			target_ = target;
		}

	protected:
		Handle<WorldSorter> target_;

		bool LuaCtor(luapp::Stack &L);
		void attach(luapp::Stack &L);
	
	private:
		Pointer<graphic::dx::StateBlockRsrc> sb_;

		GCTP_DECLARE_CLASS;
		TUKI_DECLARE(OpequeShadowedWorldRenderer);
	};

	/** シャドウマップを使った半透明体描画ノードクラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 1:05:32
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class TranslucentShadowedWorldRenderer : public Renderer
	{
	public:
		TranslucentShadowedWorldRenderer();

		virtual bool onReach(float delta) const;
		virtual bool onLeave(float delta) const;

		void attach(Handle<WorldSorter> target)
		{
			target_ = target;
		}

	protected:
		Handle<WorldSorter> target_;

		bool LuaCtor(luapp::Stack &L);
		void attach(luapp::Stack &L);
	
	private:
		Pointer<graphic::dx::StateBlockRsrc> sb_;

		GCTP_DECLARE_CLASS;
		TUKI_DECLARE(TranslucentShadowedWorldRenderer);
	};

}} // namespace gctp::scene

#endif // _GCTP_SCENE_SHADOWRENDERER_HPP_