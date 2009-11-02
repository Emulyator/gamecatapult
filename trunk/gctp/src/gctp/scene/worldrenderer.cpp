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
#include <gctp/scene/flesh.hpp>
#include <gctp/graphic.hpp>
#include <gctp/graphic/shader.hpp>
#include <gctp/graphic/dx/stateblock.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/dbgout.hpp>
#include <gctp/app.hpp> // for gctp::Profiling

using namespace std;

namespace gctp { namespace scene {

	////////////////////////
	// WorldSorter
	// 
	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, WorldSorter, Renderer);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, WorldSorter)
		TUKI_METHOD(WorldSorter, attach)
	TUKI_IMPLEMENT_END(WorldSorter)

	WorldSorter::WorldSorter()
	{
		packets.reserve(1024);
	}

	/** ワールド描画
	 *
	 * レンダリングツリーに描画メッセージを送る
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/11 2:34:02
	 */
	bool WorldSorter::onReach(float delta) const
	{
		packets.clear();
		if(target_) {
			target_->begin();
			for(HandleList<Body>::iterator i = target_->body_list.begin(); i != target_->body_list.end();) {
				if(*i) {
					//(*i)->draw(); // パスに関する情報をここで送るべき？
					(*i)->pushPackets(packets);
					++i;
				}
				else i = target_->body_list.erase(i);
			}
			packets.sort();
		}
		return false;
	}

	bool WorldSorter::onLeave(float delta) const
	{
		if(target_) target_->end();
		return true;
	}

	bool WorldSorter::LuaCtor(luapp::Stack &L)
	{
		// Context:createで製作する
		return false;
	}

	void WorldSorter::attach(luapp::Stack &L)
	{
		if(L.top() >=1) {
			Pointer<World> world = tuki_cast<World>(L[1]);
			attach(world);
		}
	}

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
				graphic::device().impl()->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
				graphic::device().impl()->SetRenderState( D3DRS_LIGHTING, TRUE );
				graphic::device().impl()->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
				graphic::device().impl()->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
				graphic::device().impl()->SetRenderState( D3DRS_COLORVERTEX, FALSE );
#ifdef GCTP_COORD_DX
				graphic::device().impl()->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
#else
				graphic::device().impl()->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
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

		/* 遠景描画用ステート
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/14 22:53:32
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		class DistantViewSB : public graphic::dx::StateBlockRsrc
		{
		public:
			void record()
			{
				HRslt hr;
				graphic::device().impl()->SetRenderState( D3DRS_CLIPPING, TRUE );
				graphic::device().impl()->SetRenderState( D3DRS_DITHERENABLE, TRUE );
				graphic::device().impl()->SetRenderState( D3DRS_ZENABLE, D3DZB_USEW/* D3DZB_TRUE */ );
				graphic::device().impl()->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
				graphic::device().impl()->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
				graphic::device().impl()->SetRenderState( D3DRS_LIGHTING, FALSE );
				graphic::device().impl()->SetRenderState( D3DRS_FOGENABLE, FALSE );
				//graphic::device().impl()->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
				graphic::device().impl()->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
				graphic::device().impl()->SetRenderState( D3DRS_COLORVERTEX, TRUE );
#ifdef GCTP_COORD_DX
				graphic::device().impl()->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
#else
				graphic::device().impl()->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
#endif
				graphic::device().impl()->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
				graphic::device().impl()->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
				graphic::device().impl()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
				graphic::device().impl()->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
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

		/* 不透明体描画用ステート
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/14 22:53:32
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		class OpequeSB : public graphic::dx::StateBlockRsrc
		{
		public:
			void record()
			{
				HRslt hr;
				graphic::device().impl()->SetRenderState( D3DRS_CLIPPING, TRUE );
				graphic::device().impl()->SetRenderState( D3DRS_DITHERENABLE, TRUE );
				graphic::device().impl()->SetRenderState( D3DRS_ZENABLE, D3DZB_USEW/* D3DZB_TRUE */ );
				graphic::device().impl()->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
				graphic::device().impl()->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
				graphic::device().impl()->SetRenderState( D3DRS_LIGHTING, TRUE );
				//graphic::device().impl()->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
				graphic::device().impl()->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
				graphic::device().impl()->SetRenderState( D3DRS_COLORVERTEX, FALSE );
#ifdef GCTP_COORD_DX
				graphic::device().impl()->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
#else
				graphic::device().impl()->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
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

		/* 透明体描画用ステート
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/14 22:53:32
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		class TranslucentSB : public graphic::dx::StateBlockRsrc
		{
		public:
			void record()
			{
				HRslt hr;
				graphic::device().impl()->SetRenderState( D3DRS_CLIPPING, TRUE );
				graphic::device().impl()->SetRenderState( D3DRS_DITHERENABLE, TRUE );
				graphic::device().impl()->SetRenderState( D3DRS_ZENABLE, D3DZB_USEW/* D3DZB_TRUE */ );
				graphic::device().impl()->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
				graphic::device().impl()->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
				graphic::device().impl()->SetRenderState( D3DRS_LIGHTING, TRUE );
				//graphic::device().impl()->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
				graphic::device().impl()->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
				graphic::device().impl()->SetRenderState( D3DRS_COLORVERTEX, FALSE );
#ifdef GCTP_COORD_DX
				graphic::device().impl()->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
#else
				graphic::device().impl()->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
#endif
				graphic::device().impl()->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
				graphic::device().impl()->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
				graphic::device().impl()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				graphic::device().impl()->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
				graphic::device().impl()->SetRenderState( D3DRS_ALPHAREF,         0x00 );
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
	// WorldRenderer
	// 
	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, WorldRenderer, WorldSorter);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, WorldRenderer)
		TUKI_METHOD(WorldRenderer, attach)
	TUKI_IMPLEMENT_END(WorldRenderer)

	WorldRenderer::WorldRenderer()
	{
		sb_ = new DefaultSB();
		sb_->setUp();
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
		WorldSorter::onReach(delta);
		sb_->begin();
		packets.draw();
		return false;
	}

	bool WorldRenderer::onLeave(float delta) const
	{
		sb_->end();
		WorldSorter::onLeave(delta);
		return true;
	}

	////////////////////////
	// MirrorWorldRenderer
	// 
	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, MirrorWorldRenderer, WorldSorter);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, MirrorWorldRenderer)
		TUKI_METHOD(MirrorWorldRenderer, attach)
	TUKI_IMPLEMENT_END(MirrorWorldRenderer)

	MirrorWorldRenderer::MirrorWorldRenderer()
	{
		sb_ = new DefaultSB();
		sb_->setUp();
	}

	/** 鏡像ワールド描画
	 *
	 * レンダリングツリーに描画メッセージを送る
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/11 2:34:02
	 */	
	bool MirrorWorldRenderer::onReach(float delta) const
	{
		WorldSorter::onReach(delta);
		sb_->begin();
#ifdef GCTP_COORD_DX
		graphic::device().impl()->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
		graphic::device().setCullingCCW(false);
#else
		graphic::device().impl()->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
		graphic::device().setCullingCCW(true);
#endif
		vmat_bak_ = graphic::device().getView();
		graphic::device().setView(vmat_bak_*MatrixC(true).scale(-1,1,1));
		packets.draw();
		return false;
	}

	bool MirrorWorldRenderer::onLeave(float delta) const
	{
#ifdef GCTP_COORD_DX
		graphic::device().setCullingCCW(true);
#else
		graphic::device().setCullingCCW(false);
#endif
		graphic::device().setView(vmat_bak_);
		sb_->end();
		WorldSorter::onLeave(delta);
		return true;
	}

	////////////////////////
	// OpequeWorldRenderer
	// 
	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, OpequeWorldRenderer, Renderer);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, OpequeWorldRenderer)
		TUKI_METHOD(OpequeWorldRenderer, attach)
	TUKI_IMPLEMENT_END(OpequeWorldRenderer)

	OpequeWorldRenderer::OpequeWorldRenderer()
	{
		sb_ = new OpequeSB();
		sb_->setUp();
	}

	bool OpequeWorldRenderer::LuaCtor(luapp::Stack &L)
	{
		// Context:createで製作する
		return false;
	}

	void OpequeWorldRenderer::attach(luapp::Stack &L)
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
	bool OpequeWorldRenderer::onReach(float delta) const
	{
		if(target_) {
			if(target_->target()) target_->target()->begin();
			sb_->begin();
			target_->packets.drawOpeque();
		}
		return false;
	}

	bool OpequeWorldRenderer::onLeave(float delta) const
	{
		if(target_) {
			sb_->end();
			if(target_->target()) target_->target()->end();
		}
		return true;
	}

	////////////////////////////
	// TranslucentWorldRenderer
	// 
	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, TranslucentWorldRenderer, Renderer);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, TranslucentWorldRenderer)
		TUKI_METHOD(TranslucentWorldRenderer, attach)
	TUKI_IMPLEMENT_END(TranslucentWorldRenderer)

	TranslucentWorldRenderer::TranslucentWorldRenderer()
	{
		sb_ = new TranslucentSB();
		sb_->setUp();
	}

	bool TranslucentWorldRenderer::LuaCtor(luapp::Stack &L)
	{
		// Context:createで製作する
		return false;
	}

	void TranslucentWorldRenderer::attach(luapp::Stack &L)
	{
		if(L.top() >= 1) {
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
	bool TranslucentWorldRenderer::onReach(float delta) const
	{
		if(target_) {
			if(target_->target()) target_->target()->begin();
			sb_->begin();
			target_->packets.drawTranslucent();
		}
		return false;
	}

	bool TranslucentWorldRenderer::onLeave(float delta) const
	{
		if(target_) {
			sb_->end();
			if(target_->target()) target_->target()->end();
		}
		return true;
	}

	////////////////////////
	// IsVisibleOperator
	// 
	void IsVisibleOperator::set(Handle<Camera> camera)
	{
		camera_ = camera;
	}

	void IsVisibleOperator::add(Handle<Flesh> flesh)
	{
		fleshies_.push_back(flesh);
		fleshies_.unique();
	}

	void IsVisibleOperator::remove(Handle<Flesh> flesh)
	{
		fleshies_.remove(flesh);
	}

	/** 描画判定
	 *
	 * 指定Fleshがすべて見えていなかったら描画をキャンセル
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/11 2:34:02
	 */	
	bool IsVisibleOperator::onReach(float delta) const
	{
		if(camera_) {
			for(HandleList<Flesh>::const_iterator i = fleshies_.begin(); i != fleshies_.end(); ++i) {
				if((*i) && camera_->isVisible((*i)->bs())) return true;
			}
		}
		return false;
	}

	bool IsVisibleOperator::LuaCtor(luapp::Stack &L)
	{
		// Context:createで製作する
		return false;
	}

	void IsVisibleOperator::set(luapp::Stack &L)
	{
		if(L.top() >=1) {
			set(tuki_cast<Camera>(L[1]));
		}
	}

	void IsVisibleOperator::add(luapp::Stack &L)
	{
		if(L.top() >=1) {
			add(tuki_cast<Flesh>(L[1]));
		}
	}

	void IsVisibleOperator::remove(luapp::Stack &L)
	{
		if(L.top() >=1) {
			remove(tuki_cast<Flesh>(L[1]));
		}
	}

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, IsVisibleOperator, Renderer);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, IsVisibleOperator)
		TUKI_METHOD(IsVisibleOperator, set)
		TUKI_METHOD(IsVisibleOperator, add)
		TUKI_METHOD(IsVisibleOperator, remove)
	TUKI_IMPLEMENT_END(IsVisibleOperator)


	////////////////////////
	// DistantViewRenderer
	// 
	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, DistantViewRenderer, Renderer);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, DistantViewRenderer)
		TUKI_METHOD(DistantViewRenderer, attach)
		TUKI_METHOD(DistantViewRenderer, setClip)
	TUKI_IMPLEMENT_END(DistantViewRenderer)

	DistantViewRenderer::DistantViewRenderer()
	{
		sb_ = new DistantViewSB();
		sb_->setUp();
	}

	/** ワールド描画
	 *
	 * レンダリングツリーに描画メッセージを送る
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/11 2:34:02
	 */
	bool DistantViewRenderer::onReach(float delta) const
	{
		if(target_) {
			vmat_bak_ = graphic::device().getView();
			pmat_bak_ = graphic::device().getProjection();
			view_port_bak_ = graphic::device().getViewPort();
			Matrix m;
			if(Camera::current().node()) m = Camera::current().node()->val.wtm().orthoNormal();
			graphic::device().setView(Matrix().setView(m.right(), m.up(), m.zaxis(), VectorC(0,0,0)));
			graphic::device().setProjection(Matrix().setFOV(Camera::current().fov(), Camera::current().aspectRatio(), Camera::current().renderRect().left, Camera::current().renderRect().top, Camera::current().renderRect().right, Camera::current().renderRect().bottom, nearclip_, farclip_));
			{
				graphic::ViewPort vp = view_port_bak_;
				vp.min_z = vp.max_z = 1.0f;
				graphic::device().setViewPort(vp);
			}
			graphic::device().setMtrlMinStateChange(true);
			target_->begin();
			sb_->begin();
			VisibilityTester::current().enable_frustum_test = false;
			target_->draw();
		}
		return false;
	}

	bool DistantViewRenderer::onLeave(float delta) const
	{
		if(target_) {
			VisibilityTester::current().enable_frustum_test = true;
			target_->end();
			sb_->end();
			graphic::device().setMtrlMinStateChange(false);
			graphic::device().setView(vmat_bak_);
			graphic::device().setProjection(pmat_bak_);
			graphic::device().setViewPort(view_port_bak_);
		}
		return true;
	}

	bool DistantViewRenderer::LuaCtor(luapp::Stack &L)
	{
		// Context:createで製作する
		return false;
	}

	void DistantViewRenderer::attach(luapp::Stack &L)
	{
		if(L.top() >=1) {
			Pointer<World> world = tuki_cast<World>(L[1]);
			attach(world);
		}
	}

	void DistantViewRenderer::setClip(luapp::Stack &L)
	{
		if(L.top() >=2) {
			nearclip_ = (float)L[1].toNumber();
			farclip_ = (float)L[2].toNumber();
		}
	}

}} // namespace gctp::scene
