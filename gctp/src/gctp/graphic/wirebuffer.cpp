/** @file
 * GameCatapult ワイヤバッファクラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/graphic/wirebuffer.hpp>
#include <gctp/graphic/texture.hpp>
#include <gctp/graphic/dx/device.hpp>

using namespace std;

namespace gctp { namespace graphic {

	GCTP_IMPLEMENT_CLASS_NS2(gctp, graphic, WireBuffer, Object);
	
	namespace {

		class WireSB : public dx::StateBlockRsrc {
		public:
			void record()
			{
				device().impl()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				device().impl()->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCCOLOR );
				device().impl()->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_ONE );
				device().impl()->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
				device().impl()->SetRenderState( D3DRS_ALPHAREF,         0x08 );
				device().impl()->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
				device().impl()->SetRenderState( D3DRS_FILLMODE,   D3DFILL_SOLID );
				device().impl()->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
				device().impl()->SetRenderState( D3DRS_ZENABLE,          D3DZB_USEW/* D3DZB_TRUE */ );
				device().impl()->SetRenderState( D3DRS_ZWRITEENABLE,     FALSE );
				device().impl()->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
				device().impl()->SetRenderState( D3DRS_CLIPPING,         TRUE );
				device().impl()->SetRenderState( D3DRS_ANTIALIASEDLINEENABLE,    FALSE );
				device().impl()->SetRenderState( D3DRS_CLIPPLANEENABLE,  FALSE );
				device().impl()->SetRenderState( D3DRS_VERTEXBLEND,      FALSE );
				device().impl()->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
				device().impl()->SetRenderState( D3DRS_LIGHTING, FALSE );
				device().impl()->SetRenderState( D3DRS_AMBIENT, 0xFFFFFFFF );
				device().impl()->SetRenderState( D3DRS_SPECULARENABLE,   TRUE );
				device().impl()->SetRenderState( D3DRS_FOGENABLE,        FALSE );
				device().impl()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
				device().impl()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
				device().impl()->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
				device().impl()->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
				device().impl()->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				device().impl()->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
				device().impl()->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
				device().impl()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
				device().impl()->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
				device().impl()->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
				device().impl()->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
				device().impl()->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
				device().impl()->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
				device().impl()->SetVertexShader( NULL );
				device().impl()->SetStreamSource(0, NULL, 0, 0);
				device().impl()->SetTexture( 0, NULL );
			}
		};

		enum {
			FVF_L  = (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_SPECULAR|D3DFVF_TEX1),
			_FORCE_DWORD_ = 0x7FFFFFFF
		};

	}

	struct WireBuffer::LVertex {
		D3DVECTOR p; D3DCOLOR color; D3DCOLOR high; float tu, tv;
		void set(Vector pos, Color32 col, Color32 hi, Point2f uv)
		{
			p = pos; color = col; high = hi; tu = uv.x; tv = uv.y;
		}
	};

	/** パーティクルバッファ制作
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 4:40:33
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt WireBuffer::setUp(uint maxnum)
	{
		HRslt hr;
		locked_ = 0;
		cur_ = 0;
		//sb_ = context().createAndSetUp<WireSB>(_T("WireSB"));
		// 共有をやめてみる
		sb_ = new WireSB;
		sb_->setUp();

		dx::FVF fvf(FVF_L);
		maxnum_ = maxnum;
		hr = VertexBuffer::setUp(VertexBuffer::DYNAMIC, fvf, maxnum*2);
		if(!hr) {
			GCTP_TRACE(hr);
			return hr;
		}

		return S_OK;
	}

	uint WireBuffer::maxnum()
	{
		return size()/fvf().stride()/2;
	}

	bool WireBuffer::isFull()
	{
		return (cur_+1)*2>freenum();
	}

	bool WireBuffer::hasSpace(uint num)
	{
		return (cur_+num)*2<=freenum();
	}

	/** バーテックスバッファに定義情報に従って頂点を書き込む
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/26 8:01:02
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	WireBuffer &WireBuffer::draw(const WireDesc &desc)
	{
		// Fill vertex buffer
		if(isFull()) { draw(); rewind(); }
		if(!locked_) locked_ = lock();
		LVertex *vtx = (LVertex *)locked_;
		vtx->set(desc.s, desc.color[0], desc.hilight[0], desc.uv[0]); vtx++;
		vtx->set(desc.e, desc.color[1], desc.hilight[1], desc.uv[1]); vtx++;
		cur_++;
		locked_ = vtx;
		if(isFull()) { draw(); rewind(); }
		return *this;
	}

	/** 現在とテクスチャが違ったら、いったん今までのを描画してから受け付ける
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/26 8:01:02
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	WireBuffer &WireBuffer::draw(const Texture &tex, const WireDesc &desc)
	{
		set(tex);
		return draw(desc);
	}

	/** 現在とテクスチャが違ったら、いったん今までのを描画してから受け付ける
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/26 8:01:02
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	WireBuffer &WireBuffer::set(const Texture &tex)
	{
		if(!tex.isCurrent(0)) {
			draw();
			tex.setCurrent(0);
		}
		return *this;
	}

	WireBuffer &WireBuffer::set(BlendMode blend)
	{
		switch(blend) {
		case OPEQUE:
			device().impl()->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
			device().impl()->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
			device().impl()->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
			device().impl()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			break;
		case TRANSLUCENT:
			device().impl()->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
			device().impl()->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
			device().impl()->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
			device().impl()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			break;
		case ADD:
			device().impl()->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCCOLOR );
			device().impl()->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_ONE );
			device().impl()->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
			device().impl()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			break;
		case SUB:
			device().impl()->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_INVDESTCOLOR );
			device().impl()->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_ZERO );
			device().impl()->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
			device().impl()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			break;
		case MUL:
			device().impl()->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_ZERO );
			device().impl()->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_SRCCOLOR );
			device().impl()->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
			device().impl()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			break;
		}
		return *this;
	}

	WireBuffer &WireBuffer::begin(bool do_filter)
	{
		draw();
		sb_->begin();
		setCurrent(0);
		// Set filter states
		if( do_filter ) {
			device().impl()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			device().impl()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		}
		Matrix view = device().getView().inverse();
		return *this;
	}

	WireBuffer &WireBuffer::begin(const Texture &tex, bool do_filter)
	{
		begin(do_filter);
		tex.setCurrent(0);
		return *this;
	}

	WireBuffer &WireBuffer::end()
	{
		draw();
		// Restore the modified renderstates
		sb_->end();
		return *this;
	}

	HRslt WireBuffer::draw()
	{
		if(locked_) {
			unlock();
			locked_ = NULL;
		}
		if(cur_ > 0) {
			HRslt hr;
			hr = VertexBuffer::draw(0, D3DPT_LINELIST, cur_);
			if(!hr) GCTP_TRACE(hr);
			cur_ = 0;
			return hr;
		}
		return S_FALSE;
	}

	uint WireBuffer::default_maxnum_ = 512;
}} // namespace gctp
