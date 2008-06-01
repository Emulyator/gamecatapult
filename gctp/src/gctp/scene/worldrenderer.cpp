/** @file
 * GameCatapult 簡易ワールドレンダラークラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/worldrenderer.hpp>
#include <gctp/scene/camera.hpp>
#include <gctp/scene/body.hpp>
#include <gctp/scene/light.hpp>
#include <gctp/scene/world.hpp>
#include <gctp/graphic.hpp>
#include <gctp/graphic/shader.hpp>
#include <gctp/graphic/dx/stateblock.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace scene {

	/* 通常の３Ｄアプリとして一般的なデバイス設定を行う
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/14 22:53:32
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class DefaultSB : public graphic::dx::StateBlockRsrc {
	public:
		void record()
		{
			graphic::device().impl()->SetRenderState( D3DRS_CLIPPING, TRUE );
			graphic::device().impl()->SetRenderState( D3DRS_DITHERENABLE, TRUE );
			graphic::device().impl()->SetRenderState( D3DRS_ZENABLE, TRUE );
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
			graphic::device().impl()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			graphic::device().impl()->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
			graphic::device().impl()->SetRenderState( D3DRS_ALPHAREF,         0x08 );
			graphic::device().impl()->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
			graphic::device().impl()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
			graphic::device().impl()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
			graphic::device().impl()->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
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

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, WorldRenderer, Renderer);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, WorldRenderer)
		TUKI_METHOD(WorldRenderer, add)
		TUKI_METHOD(WorldRenderer, remove)
	TUKI_IMPLEMENT_END(WorldRenderer)

	WorldRenderer::WorldRenderer()
	{
		dsb_ = new DefaultSB();
		dsb_->setUp();
	}

	void WorldRenderer::add(Handle<World> world)
	{
		if(world) {
			worlds_.push_back(world);
			worlds_.unique();
		}
	}

	void WorldRenderer::remove(Handle<World> world)
	{
		if(world) worlds_.remove(world);
	}

	/** ワールド描画
	 *
	 * レンダリングツリーに描画メッセージを送る
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/11 2:34:02
	 */	
	bool WorldRenderer::onReach(float delta) const
	{
#ifndef __ee__
		graphic::setAmbient(graphic::getAmbient());
		graphic::clearLight();
#endif
		dsb_->setCurrent();
		WorldRenderer *self = const_cast<WorldRenderer *>(this);
		for(HandleList<World>::iterator world = self->worlds_.begin(); world != self->worlds_.end();) {
			if(*world) {
				(*world)->begin();
				for(HandleList<Body>::iterator i = (*world)->body_list.begin(); i != (*world)->body_list.end();) {
					if(*i) {
						(*i)->draw(); // パスに関する情報をここで送るべき？
						++i;
					}
					else i = (*world)->body_list.erase(i);
				}
				(*world)->end();
				++world;
			}
			else world = self->worlds_.erase(world);
		}
		// うーん、めんどいなぁ
		// いや、ほかはこれの派生にして、みんなこの部分を使えるようにすればいいんじゃ
		// Aspectにしてmixinにしたいなぁ
		return false;
	}

	bool WorldRenderer::onLeave(float delta) const
	{
		dsb_->unset();
		return true;
	}

	bool WorldRenderer::setUp(luapp::Stack &L)
	{
		// Context:createで製作する
		return false;
	}

	void WorldRenderer::add(luapp::Stack &L)
	{
		if(L.top() >=1) {
			Pointer<World> world = tuki_cast<World>(L[1]);
			if(world) add(world);
		}
	}

	void WorldRenderer::remove(luapp::Stack &L)
	{
		if(L.top() >=1) {
			Pointer<World> world = tuki_cast<World>(L[1]);
			if(world) remove(world);
		}
	}

}} // namespace gctp::scene
