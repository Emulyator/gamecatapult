/** @file
 * GameCatapult �ȈՃ��[���h�����_���[�N���X
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
#include <gctp/app.hpp> // for gctp::Profiling

using namespace std;

namespace gctp { namespace scene {

	/* �ʏ�̂R�c�A�v���Ƃ��Ĉ�ʓI�ȃf�o�C�X�ݒ���s��
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
			graphic::device().impl()->SetRenderState( D3DRS_ZENABLE, D3DZB_USEW/* D3DZB_TRUE */ );
			graphic::device().impl()->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESS );
			graphic::device().impl()->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
			graphic::device().impl()->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
			graphic::device().impl()->SetRenderState( D3DRS_COLORVERTEX, TRUE );
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

			graphic::device().impl()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
			graphic::device().impl()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );
			graphic::device().impl()->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
			/*graphic::device().impl()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
			graphic::device().impl()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
			graphic::device().impl()->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );*/

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
		packets_.reserve(1024);
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

	/** ���[���h�`��
	 *
	 * �����_�����O�c���[�ɕ`�惁�b�Z�[�W�𑗂�
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/11 2:34:02
	 */	
	bool WorldRenderer::onReach(float delta) const
	{
		gctp::Profiling prof("WR.onReach");
#ifndef __ee__
		graphic::setAmbient(graphic::getAmbient());
		graphic::clearLight();
#endif
		dsb_->setCurrent();
		WorldRenderer *self = const_cast<WorldRenderer *>(this);
		packets_.clear();
		{
			gctp::Profiling prof("WR.pushPack");
			for(HandleList<World>::iterator world = self->worlds_.begin(); world != self->worlds_.end();) {
				if(*world) {
					(*world)->begin();
					for(HandleList<Body>::iterator i = (*world)->body_list.begin(); i != (*world)->body_list.end();) {
						if(*i) {
							//(*i)->draw(); // �p�X�Ɋւ�����������ő���ׂ��H
							(*i)->pushPackets(packets_);
							++i;
						}
						else i = (*world)->body_list.erase(i);
					}
					(*world)->end();
					++world;
				}
				else world = self->worlds_.erase(world);
			}
		}
		{
			gctp::Profiling prof("WR.sort");
			packets_.sort();
		}
		packets_.draw();
		return false;
	}

	bool WorldRenderer::onLeave(float delta) const
	{
		dsb_->unset();
		return true;
	}

	bool WorldRenderer::setUp(luapp::Stack &L)
	{
		// Context:create�Ő��삷��
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
