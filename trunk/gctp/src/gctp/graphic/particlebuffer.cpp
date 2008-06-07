/** @file
 * GameCatapult パーティクルバッファクラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/graphic/particlebuffer.hpp>
#include <gctp/graphic/texture.hpp>
#include <gctp/graphic/dx/device.hpp>

using namespace std;

namespace gctp { namespace graphic {

	GCTP_IMPLEMENT_CLASS_NS2(gctp, graphic, ParticleBuffer, Object);
	
	namespace {

		class ParticleSB : public dx::StateBlockRsrc {
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

	struct ParticleBuffer::LVertex {
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
	HRslt ParticleBuffer::setUp(uint maxnum)
	{
		HRslt hr;
		locked_ = 0;
		cur_ = 0;
		//sb_ = context().createAndSetUp<ParticleSB>(_T("PARTICLESB"));
		// 共有をやめてみる
		sb_ = new ParticleSB;
		sb_->setUp();

		dx::FVF fvf(FVF_L);
		maxnum_ = maxnum;
		hr = VertexBuffer::setUp(VertexBuffer::DYNAMIC, fvf, maxnum*6);
//		hr = VertexBuffer::setUp(VertexBuffer::DYNAMIC, fvf.stride()*maxnum*6);
		if(!hr) {
			GCTP_TRACE(hr);
			return hr;
		}

		return S_OK;
	}

	uint ParticleBuffer::maxnum()
	{
		return size()/fvf().stride()/6;
	}

	bool ParticleBuffer::isFull()
	{
		return (cur_+1)*3>freenum();
	}

	bool ParticleBuffer::hasSpace(uint num)
	{
		return (cur_+num)*3<=freenum();
	}

	/** バーテックスバッファに定義情報に従って頂点を書き込む
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/26 8:01:02
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	ParticleBuffer &ParticleBuffer::draw(const ParticleDesc &desc)
	{
		// Fill vertex buffer
		if(isFull()) { draw(); rewind(); }
		if(!locked_) locked_ = lock();
		LVertex *vtx = (LVertex *)locked_;
		Vector right = right_*(desc.size.x/2.0f);
		Vector up = up_*(desc.size.y/2.0f);
		vtx->set(desc.pos + up - right, desc.color[0], desc.hilight[0], desc.uv[0]); vtx++;
		vtx->set(desc.pos + up + right, desc.color[1], desc.hilight[1], desc.uv[1]); vtx++;
		vtx->set(desc.pos - up - right, desc.color[2], desc.hilight[2], desc.uv[2]); vtx++;
		cur_++;
		vtx->set(desc.pos + up + right, desc.color[1], desc.hilight[1], desc.uv[1]); vtx++;
		vtx->set(desc.pos - up + right, desc.color[3], desc.hilight[3], desc.uv[3]); vtx++;
		vtx->set(desc.pos - up - right, desc.color[2], desc.hilight[2], desc.uv[2]); vtx++;
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
	ParticleBuffer &ParticleBuffer::draw(const Texture &tex, const ParticleDesc &desc)
	{
		set(tex);
		return draw(desc);
	}

	/** バーテックスバッファに定義情報に従って頂点を書き込む
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/26 8:01:02
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	ParticleBuffer &ParticleBuffer::draw(const LineParticleDesc &desc)
	{
		// Fill vertex buffer
		if(desc.num<2) return *this;
		if(!hasSpace((desc.num+1)*2)) { draw(); rewind(); }
		float len = desc.size.x;
		for(uint i = 0; i < desc.num-1; i++) len += (desc.pos[i+1]-desc.pos[i]).length();
		Vector up;
		float l;
		Vector2 wuv[2];
		Color32 color[2];
		Color32 hilight[2];
		for(uint i = 0; i < desc.num; i++) {
			if(!locked_) locked_ = lock();
			LVertex *vtx = (LVertex *)locked_;
			if(i == 0) { // 最初
				l = desc.size.x/2.0f;
				Vector dir = (desc.pos[1]-desc.pos[0]).normal();
//				if(fabsf(dir*at_)<FLT_EPSILON*2) {
					Vector right = dir*(desc.size.x/2.0f);
					up = (dir%(vpos_-desc.pos[i]).normal()).normal()*(desc.size.y/2.0f);
					wuv[0] = desc.uv[1]; wuv[0].x *= (l/len);
					wuv[1] = desc.uv[3]; wuv[1].x *= (l/len);
					color[0].setLerp(desc.color[0], desc.color[1], l/len);
					color[1].setLerp(desc.color[2], desc.color[3], l/len);
					hilight[0].setLerp(desc.hilight[0], desc.hilight[1], l/len);
					hilight[1].setLerp(desc.hilight[2], desc.hilight[3], l/len);
					vtx->set(desc.pos[i] + up - right, desc.color[0], desc.hilight[0], desc.uv[0]); vtx++;
					vtx->set(desc.pos[i] + up        , color[0], hilight[0], wuv[0]); vtx++;
					vtx->set(desc.pos[i] - up - right, desc.color[2], desc.hilight[2], desc.uv[2]); vtx++;
					cur_++;
					vtx->set(desc.pos[i] + up        , color[0], hilight[0], wuv[0]); vtx++;
					vtx->set(desc.pos[i] - up        , color[1], hilight[1], wuv[1]); vtx++;
					vtx->set(desc.pos[i] - up - right, desc.color[2], desc.hilight[2], desc.uv[2]); vtx++;
					cur_++;
					locked_ = vtx;
					if(isFull()) { draw(); rewind(); }
//				}
			}
			if(i != 0) {
				Vector ndir = (desc.pos[i]-desc.pos[i-1]);
				l += ndir.length();
				ndir.normalize();
//				if(fabsf(dir1*at_)<FLT_EPSILON*2 && fabsf(dir2*at_)<FLT_EPSILON*2) {
					Vector nup = (ndir%(vpos_-desc.pos[i]).normal()).normal()*(desc.size.y/2.0f);
					Vector2 nwuv[2];
					Color32 ncolor[2];
					Color32 nhilight[2];
					nwuv[0] = desc.uv[1]; nwuv[0].x *= (l/len);
					nwuv[1] = desc.uv[3]; nwuv[1].x *= (l/len);
					ncolor[0].setLerp(desc.color[0], desc.color[1], l/len);
					ncolor[1].setLerp(desc.color[2], desc.color[3], l/len);
					nhilight[0].setLerp(desc.hilight[0], desc.hilight[1], l/len);
					nhilight[1].setLerp(desc.hilight[2], desc.hilight[3], l/len);
					vtx->set(desc.pos[i-1] +  up,  color[0],  hilight[0],  wuv[0]); vtx++;
					vtx->set(desc.pos[i  ] + nup, ncolor[0], nhilight[0], nwuv[0]); vtx++;
					vtx->set(desc.pos[i-1] -  up,  color[1],  hilight[1],  wuv[1]); vtx++;
					cur_++;
					vtx->set(desc.pos[i  ] + nup, ncolor[0], nhilight[0], nwuv[0]); vtx++;
					vtx->set(desc.pos[i  ] - nup, ncolor[1], nhilight[1], nwuv[1]); vtx++;
					vtx->set(desc.pos[i-1] -  up,  color[1],  hilight[1],  wuv[1]); vtx++;
					cur_++;
					locked_ = vtx;
					if(isFull()) { draw(); rewind(); }
					up = nup;
					wuv[0] = nwuv[0];
					wuv[1] = nwuv[1];
					color[0] = ncolor[0];
					color[1] = ncolor[1];
					hilight[0] = nhilight[0];
					hilight[1] = nhilight[1];
//				}
			}
			if(i == desc.num-1) { // 最後
				Vector dir = (desc.pos[i]-desc.pos[i-1]).normal();
				l += desc.size.x/2.0f;
//				if(fabsf(dir*at_)<FLT_EPSILON*2) {
					Vector right = dir*(desc.size.x/2.0f);
					vtx->set(desc.pos[i] + up        , color[0], hilight[0], wuv[0]); vtx++;
					vtx->set(desc.pos[i] + up + right, desc.color[1], desc.hilight[1], desc.uv[1]); vtx++;
					vtx->set(desc.pos[i] - up        , color[0], hilight[0], wuv[1]); vtx++;
					cur_++;
					vtx->set(desc.pos[i] + up + right, desc.color[1], desc.hilight[1], desc.uv[1]); vtx++;
					vtx->set(desc.pos[i] - up + right, desc.color[3], desc.hilight[3], desc.uv[3]); vtx++;
					vtx->set(desc.pos[i] - up        , color[1], hilight[1], wuv[1]); vtx++;
					cur_++;
					locked_ = vtx;
					if(isFull()) { draw(); rewind(); }
//				}
			}
		}
		return *this;
	}

	/** 現在とテクスチャが違ったら、いったん今までのを描画してから受け付ける
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/26 8:01:02
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	ParticleBuffer &ParticleBuffer::draw(const Texture &tex, const LineParticleDesc &desc)
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
	ParticleBuffer &ParticleBuffer::set(const Texture &tex)
	{
		if(!tex.isCurrent(0)) {
			draw();
			tex.setCurrent(0);
		}
		return *this;
	}

	ParticleBuffer &ParticleBuffer::set(BlendMode blend)
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

	ParticleBuffer &ParticleBuffer::begin(bool do_filter)
	{
		draw();
		sb_->begin();
		setCurrent(0);
		// Set filter states
		if( do_filter ) {
			device().impl()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			device().impl()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		}
		Matrix view = getView().inverse();
		right_ = view.right();
		up_    = view.up();
		vpos_  = view.position();
		return *this;
	}

	ParticleBuffer &ParticleBuffer::begin(const Texture &tex, bool do_filter)
	{
		begin(do_filter);
		tex.setCurrent(0);
		return *this;
	}

	ParticleBuffer &ParticleBuffer::end()
	{
		draw();
		// Restore the modified renderstates
		sb_->end();
		return *this;
	}

	HRslt ParticleBuffer::draw()
	{
		if(locked_) {
			unlock();
			locked_ = NULL;
		}
		if(cur_ > 0) {
			HRslt hr;
			hr = VertexBuffer::draw(0, D3DPT_TRIANGLELIST, cur_);
			if(!hr) GCTP_TRACE(hr);
			cur_ = 0;
			return hr;
		}
		return S_FALSE;
	}

	uint ParticleBuffer::default_maxnum_ = 512;
}} // namespace gctp
