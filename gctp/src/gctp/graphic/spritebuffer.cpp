/** @file
 * GameCatapult スプライトクラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: spritebuffer.cpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/graphic/spritebuffer.hpp>
#include <gctp/graphic/texture.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <sstream>
#include <gctp/csv.hpp>
#include <boost/lexical_cast.hpp>
#include <assert.h>

using namespace std;

namespace gctp { namespace graphic {

	SpriteDesc &SpriteDesc::setUp(const Texture &tex)
	{
		Size2 size = tex.originalSize();
		setPos(RectC(0, 0, size.x, size.y));
		setUV(RectfC(0, 0, 1, 1));
		setColor(Color32(255, 255, 255));
		setHilight(Color32(0, 0, 0));
		return *this;
	}

	GCTP_IMPLEMENT_CLASS_NS(gctp, SpriteBuffer, Object);
	
	class SpriteSB : public dx::StateBlockRsrc {
	public:
		void record()
		{
			device().impl()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			device().impl()->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
			device().impl()->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
			device().impl()->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
			device().impl()->SetRenderState( D3DRS_ALPHAREF,         0x08 );
			device().impl()->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
			device().impl()->SetRenderState( D3DRS_FILLMODE,   D3DFILL_SOLID );
			device().impl()->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
			device().impl()->SetRenderState( D3DRS_ZENABLE,          FALSE );
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
			device().impl()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
			device().impl()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
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
			device().setView(MatrixC(true));
			device().setProjection(MatrixC(true));
			device().setWorld(MatrixC(true));
		}
	};

	namespace {

		enum {
			FVF_TL = (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_SPECULAR|D3DFVF_TEX1),
			FVF_L  = (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_SPECULAR|D3DFVF_TEX1),
			_FORCE_DWORD_ = 0x7FFFFFFF
		};

	}

	struct SpriteBuffer::TLVertex {
		D3DXVECTOR4 p; D3DCOLOR color; D3DCOLOR high; float tu, tv;
		void set(Point2f pos, Color32 col, Color32 hi, Point2f uv)
		{
			p.x = pos.x; p.y = pos.y; p.z = 0.9f; p.w = 1.0f; color = col; high = hi; tu = uv.x; tv = uv.y;
		}
	};

	struct SpriteBuffer::LVertex {
		D3DXVECTOR3 p; D3DCOLOR color; D3DCOLOR high; float tu, tv;
		void set(Point2f pos, Color32 col, Color32 hi, Point2f uv)
		{
			p.x = pos.x; p.y = pos.y; p.z = 0.0f; color = col; high = hi; tu = uv.x; tv = uv.y;
		}
	};

	/** スプライトバッファ制作
	 *
	 * 文字列によるパラメータを受け取る\n
	 * "sprite,100,1"\n
	 * なら、maxnum=100, is_TL=trueでsetUpを呼び出す。\n
	 * "sprite,0,0"\n
	 * なら、maxnum=default_maxnum_, is_TL=falseでsetUpを呼び出す。\n
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 4:40:33
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt SpriteBuffer::setUp(const char *name)
	{
		if(name) {
			stringstream ioss;
			ioss << name;
			CSVRow csv;
			ioss >> csv;
			if(csv.size() > 1) {
				bool is_TL = true;
				uint maxnum = boost::lexical_cast<uint>(csv[1]);
				if(maxnum==0) maxnum = default_maxnum_;
				if(csv.size() > 2) is_TL = boost::lexical_cast<bool>(csv[2]);
				return setUp(maxnum, is_TL);
			}
		}
		return setUp();
	}

	/** スプライトバッファ制作
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 4:40:33
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt SpriteBuffer::setUp(uint maxnum, bool is_TL)
	{
		HRslt hr;
		locked_ = 0;
		cur_ = 0;
		sb_ = createOnDB<SpriteSB>("SPRITESB");

		is_TL_ = is_TL;
		maxnum_ = maxnum;
		hr = VertexBuffer::setUp(VertexBuffer::DYNAMIC, is_TL_?dx::FVF(FVF_TL):dx::FVF(FVF_L), maxnum*6);
//		hr = VertexBuffer::setUp(VertexBuffer::DYNAMIC, fvf.stride()*maxnum*6);
		// STREAMOFFSETが指定できるとは限らない。
		// 結局、頂点オフセットのみでやるほうが安全で、その場合非ＦＶＦバッファは意味がない。
		// GeForce3やRADEON7500など、比較的最近のハードでも、またチップセット内蔵系も対応していない
		// 総じて、非ＦＶＦバッファは使わないほうがポータビリティーが高い
		if(!hr) {
			GCTP_TRACE(hr);
			return hr;
		}

		return S_OK;
	}

	uint SpriteBuffer::maxnum()
	{
		return size()/fvf().stride()/6;
	}

	bool SpriteBuffer::isFull()
	{
		lastfreenum_ = freenum();
		return (cur_+1)*6>lastfreenum_;
	}

	/** 描画
	 *
	 * バッファの使い方まずい。
	 * バッファが本当にいっぱいになるまで、DISCARDは控えるべき。
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/26 8:01:02
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	SpriteBuffer &SpriteBuffer::draw(const SpriteDesc &desc)
	{
		if(isFull()) {
			draw();
			rewind();
		}
		// Fill vertex buffer
		if(!locked_) locked_ = lock();
		if(is_TL_) {
			TLVertex *vtx = (TLVertex *)locked_;
			vtx->set(desc.pos[0], desc.color[0], desc.hilight[0], desc.uv[0]); vtx++;
			vtx->set(desc.pos[1], desc.color[1], desc.hilight[1], desc.uv[1]); vtx++;
			vtx->set(desc.pos[2], desc.color[2], desc.hilight[2], desc.uv[2]); vtx++;
			vtx->set(desc.pos[1], desc.color[1], desc.hilight[1], desc.uv[1]); vtx++;
			vtx->set(desc.pos[3], desc.color[3], desc.hilight[3], desc.uv[3]); vtx++;
			vtx->set(desc.pos[2], desc.color[2], desc.hilight[2], desc.uv[2]); vtx++;
			cur_++;
			locked_ = vtx;
		}
		else {
			LVertex *vtx = (LVertex *)locked_;
			vtx->set(desc.pos[0], desc.color[0], desc.hilight[0], desc.uv[0]); vtx++;
			vtx->set(desc.pos[1], desc.color[1], desc.hilight[1], desc.uv[1]); vtx++;
			vtx->set(desc.pos[2], desc.color[2], desc.hilight[2], desc.uv[2]); vtx++;
			vtx->set(desc.pos[1], desc.color[1], desc.hilight[1], desc.uv[1]); vtx++;
			vtx->set(desc.pos[3], desc.color[3], desc.hilight[3], desc.uv[3]); vtx++;
			vtx->set(desc.pos[2], desc.color[2], desc.hilight[2], desc.uv[2]); vtx++;
			cur_++;
			locked_ = vtx;
		}
		if(isFull()) {
			draw();
			rewind();
		}
		return *this;
	}

	/** 現在とテクスチャが違ったら、いったん今までのを描画してから受け付ける
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/26 8:01:02
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	SpriteBuffer &SpriteBuffer::draw(const Texture &tex, const SpriteDesc &desc)
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
	SpriteBuffer &SpriteBuffer::set(const Texture &tex)
	{
		if(!tex.isCurrent(0)) {
			draw();
			tex.setCurrent(0);
		}
		return *this;
	}

	SpriteBuffer &SpriteBuffer::begin(bool do_filter)
	{
		draw();
		sb_->setCurrent();
		setCurrent(0);
		// Set filter states
		if( do_filter ) {
			device().impl()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			device().impl()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		}
		return *this;
	}

	SpriteBuffer &SpriteBuffer::begin(const Texture &tex, bool do_filter)
	{
		begin(do_filter);
		tex.setCurrent(0);
		return *this;
	}

	SpriteBuffer &SpriteBuffer::begin(const Size2 &screen/**< 仮想スクリーンサイズ */, bool do_filter)
	{
		assert(is_TL_==true);
		begin(do_filter);
		graphic::setView(Matrix().setView(VectorC(1, 0, 0), VectorC(0, 0, -1), VectorC(0, -1, 0), VectorC(static_cast<float>(screen.x)/2.f, static_cast<float>(screen.y)/2.f, 0.f)));
		graphic::setProjection(Matrix().setOrtho(static_cast<float>(screen.x), static_cast<float>(screen.y), 0, 1));
		return *this;
	}

	SpriteBuffer &SpriteBuffer::begin(const Size2 &screen/**< 仮想スクリーンサイズ */, const class Texture &tex, bool do_filter)
	{
		begin(screen, do_filter);
		tex.setCurrent(0);
		return *this;
	}

	SpriteBuffer &SpriteBuffer::end()
	{
		draw();
		// Restore the modified renderstates
		sb_->unSet();
		return *this;
	}

	HRslt SpriteBuffer::draw()
	{
		if(locked_) {
			unlock();
			locked_ = NULL;
		}
		if(cur_ > 0) {
			HRslt hr;
/*			if(is_TL_) hr = VertexBuffer::draw(FVF(FVF_TL), 0, D3DPT_TRIANGLELIST, cur_*2);
			else hr = VertexBuffer::draw(FVF(FVF_L), 0, D3DPT_TRIANGLELIST, cur_*2);*/
			hr = VertexBuffer::draw(0, D3DPT_TRIANGLELIST, cur_*2);
			if(!hr) GCTP_TRACE(hr);
			cur_ = 0;
			return hr;
		}
		return S_FALSE;
	}

	uint SpriteBuffer::default_maxnum_ = 512;
}} // namespace gctp
