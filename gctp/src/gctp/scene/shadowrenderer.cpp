/** @file
 * GameCatapult シャドウマップ描画レンダラークラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/shadowrenderer.hpp>
#include <gctp/scene/worldrenderer.hpp>
#include <gctp/scene/camera.hpp>
#include <gctp/scene/body.hpp>
#include <gctp/scene/light.hpp>
#include <gctp/scene/world.hpp>
#include <gctp/graphic.hpp>
#include <gctp/graphic/shader.hpp>
#include <gctp/graphic/texture.hpp>
#include <gctp/graphic/rendertarget.hpp>
#include <gctp/graphic/dx/stateblock.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/dbgout.hpp>
#include <gctp/app.hpp> // for gctp::Profiling

using namespace std;

namespace gctp { namespace scene {

	namespace {
		/* 通常の３Ｄアプリとして一般的なデバイス設定を行う
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/14 22:53:32
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		class DefaultSB : public graphic::dx::StateBlockRsrc
		{
		public:
			void record()
			{
				HRslt hr;
				graphic::device().impl()->SetRenderState( D3DRS_CLIPPING, TRUE );
				graphic::device().impl()->SetRenderState( D3DRS_DITHERENABLE, TRUE );
				graphic::device().impl()->SetRenderState( D3DRS_ZENABLE, D3DZB_USEW/* D3DZB_TRUE */ );
				graphic::device().impl()->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESS );
				graphic::device().impl()->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
				graphic::device().impl()->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
				graphic::device().impl()->SetRenderState( D3DRS_COLORVERTEX, FALSE );
#ifdef GCTP_COORD_RH
				graphic::device().impl()->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
#else
				graphic::device().impl()->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
#endif
				graphic::device().impl()->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
				graphic::device().impl()->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
				graphic::device().impl()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
				graphic::device().impl()->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
				graphic::device().impl()->SetRenderState( D3DRS_ALPHAREF,         0x80 );
				graphic::device().impl()->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATER );

				hr = graphic::device().impl()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
				if(!hr) graphic::device().impl()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
				hr = graphic::device().impl()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );
				if(!hr) graphic::device().impl()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
				hr = graphic::device().impl()->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
				if(!hr) graphic::device().impl()->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );

				graphic::device().impl()->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
				graphic::device().impl()->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				graphic::device().impl()->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
				graphic::device().impl()->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
				graphic::device().impl()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
				graphic::device().impl()->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
				graphic::device().impl()->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
				graphic::device().impl()->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
				graphic::device().impl()->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
				graphic::device().impl()->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
			}
		};
	}

	////////////////////////
	// ShadowWorldRenderer
	// 
	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, ShadowWorldRenderer, Renderer);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, ShadowWorldRenderer)
		TUKI_METHOD(ShadowWorldRenderer, attach)
		TUKI_METHOD(ShadowWorldRenderer, texture)
	TUKI_IMPLEMENT_END(ShadowWorldRenderer)

	ShadowWorldRenderer::ShadowWorldRenderer()
	{
		sb_ = new DefaultSB();
		sb_->setUp();
		rt_ = new graphic::RenderTarget;
		HRslt hr = rt_->setUp(graphic::RenderTarget::NORMAL, 512, 512, D3DFMT_G16R16);
		if(!hr) GCTP_ERRORINFO(hr);
	}

	bool ShadowWorldRenderer::LuaCtor(luapp::Stack &L)
	{
		// Context:createで製作する
		return false;
	}

	void ShadowWorldRenderer::attach(luapp::Stack &L)
	{
		if(L.top() >=1) {
			Pointer<WorldSorter> target = tuki_cast<WorldSorter>(L[1]);
			if(target) target_ = target;
		}
	}

	int ShadowWorldRenderer::texture(luapp::Stack &L)
	{
		TukiRegister::push(L, Hndl(&(rt_->texture())));
		return 1;
	}

	/** ワールド描画
	 *
	 * レンダリングツリーに描画メッセージを送る
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/11 2:34:02
	 */	
	bool ShadowWorldRenderer::onReach(float delta) const
	{
		if(target_) {
			sb_->begin();
			rt_->begin();
			target_->packets.drawOpeque();
		}
		return false;
	}

	bool ShadowWorldRenderer::onLeave(float delta) const
	{
		if(target_) {
			rt_->end();
			sb_->end();
			rt_->resolve();
		}
		return true;
	}

	////////////////////////
	// OpequeShadowedWorldRenderer
	// 
	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, OpequeShadowedWorldRenderer, Renderer);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, OpequeShadowedWorldRenderer)
		TUKI_METHOD(OpequeShadowedWorldRenderer, attach)
	TUKI_IMPLEMENT_END(OpequeShadowedWorldRenderer)

	OpequeShadowedWorldRenderer::OpequeShadowedWorldRenderer()
	{
		sb_ = new DefaultSB();
		sb_->setUp();
	}

	bool OpequeShadowedWorldRenderer::LuaCtor(luapp::Stack &L)
	{
		// Context:createで製作する
		return false;
	}

	void OpequeShadowedWorldRenderer::attach(luapp::Stack &L)
	{
		if(L.top() >=1) {
			Pointer<WorldSorter> target = tuki_cast<WorldSorter>(L[1]);
			if(target) target_ = target;
		}
	}

	/** ワールド描画
	 *
	 * レンダリングツリーに描画メッセージを送る
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/11 2:34:02
	 */	
	bool OpequeShadowedWorldRenderer::onReach(float delta) const
	{
		if(target_) {
			sb_->begin();
			target_->packets.drawOpeque();
		}
		return false;
	}

	bool OpequeShadowedWorldRenderer::onLeave(float delta) const
	{
		if(target_) sb_->end();
		return true;
	}

	////////////////////////////
	// TranslucentShadowedWorldRenderer
	// 
	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, TranslucentShadowedWorldRenderer, Renderer);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, TranslucentShadowedWorldRenderer)
		TUKI_METHOD(TranslucentShadowedWorldRenderer, attach)
	TUKI_IMPLEMENT_END(TranslucentShadowedWorldRenderer)

	TranslucentShadowedWorldRenderer::TranslucentShadowedWorldRenderer()
	{
		sb_ = new DefaultSB();
		sb_->setUp();
	}

	bool TranslucentShadowedWorldRenderer::LuaCtor(luapp::Stack &L)
	{
		// Context:createで製作する
		return false;
	}

	void TranslucentShadowedWorldRenderer::attach(luapp::Stack &L)
	{
		if(L.top() >=1) {
			Pointer<WorldSorter> target = tuki_cast<WorldSorter>(L[1]);
			if(target) target_ = target;
		}
	}

	/** ワールド描画
	 *
	 * レンダリングツリーに描画メッセージを送る
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/11 2:34:02
	 */
	bool TranslucentShadowedWorldRenderer::onReach(float delta) const
	{
		if(target_) {
			sb_->begin();
			target_->packets.drawTranslucent();
		}
		return false;
	}

	bool TranslucentShadowedWorldRenderer::onLeave(float delta) const
	{
		if(target_) sb_->end();
		return true;
	}

}} // namespace gctp::scene
