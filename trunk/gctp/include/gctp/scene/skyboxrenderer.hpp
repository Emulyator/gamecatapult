#ifndef _GCTP_SCENE_SKYBOXRENDERER_HPP_
#define _GCTP_SCENE_SKYBOXRENDERER_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult スカイボックスノードクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/tuki.hpp>
#include <gctp/pointer.hpp>
#include <gctp/graphic/vertexbuffer.hpp>
#include <gctp/graphic/indexbuffer.hpp>
#include <gctp/scene/renderer.hpp>

namespace gctp {
	namespace graphic {
		class CubeTexture;
		class Shader;
		namespace dx {
			class HLSLShader;
		}
	}
}

namespace gctp { namespace scene {

	/** スカイボックスノード
	 *
	 */
	class SkyBoxRenderer : public Renderer
	{
	public:
		SkyBoxRenderer();

		virtual bool onReach(float delta) const;
		virtual bool onLeave(float delta) const;

		/// キューブテクスチャ指定
		void setTexture(Handle<graphic::CubeTexture> texture)
		{
			texture_ = texture;
		}

		float radius;

	protected:
		graphic::VertexBuffer vb_;
		graphic::IndexBuffer ib_;
		Handle<graphic::Shader> shader_;
		Handle<graphic::CubeTexture> texture_;

		bool setUp(luapp::Stack &L);
		void setRadius(luapp::Stack &L);
		void load(luapp::Stack &L);

	GCTP_DECLARE_CLASS
	TUKI_DECLARE(SkyBoxRenderer)
	};

}} // namespace gctp::scene

#endif // _GCTP_SCENE_SKYBOXRENDERER_HPP_