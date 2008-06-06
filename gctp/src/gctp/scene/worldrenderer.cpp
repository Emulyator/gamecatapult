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

	////////////////////////
	// WorldSorter
	// 
	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, WorldSorter, Renderer);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, WorldSorter)
		TUKI_METHOD(WorldSorter, add)
		TUKI_METHOD(WorldSorter, remove)
	TUKI_IMPLEMENT_END(WorldSorter)

	WorldSorter::WorldSorter()
	{
		packets.reserve(1024);
	}

	void WorldSorter::add(Handle<World> world)
	{
		if(world) {
			worlds_.push_back(world);
			worlds_.unique();
		}
	}

	void WorldSorter::remove(Handle<World> world)
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
	bool WorldSorter::onReach(float delta) const
	{
		WorldSorter *self = const_cast<WorldSorter *>(this);
		packets.clear();
		for(HandleList<World>::iterator world = self->worlds_.begin(); world != self->worlds_.end();) {
			if(*world) {
				(*world)->begin();
				for(HandleList<Body>::iterator i = (*world)->body_list.begin(); i != (*world)->body_list.end();) {
					if(*i) {
						//(*i)->draw(); // �p�X�Ɋւ�����������ő���ׂ��H
						(*i)->pushPackets(packets);
						++i;
					}
					else i = (*world)->body_list.erase(i);
				}
				(*world)->end();
				++world;
			}
			else world = self->worlds_.erase(world);
		}
		packets.sort();
		return false;
	}

	bool WorldSorter::setUp(luapp::Stack &L)
	{
		// Context:create�Ő��삷��
		return false;
	}

	void WorldSorter::add(luapp::Stack &L)
	{
		if(L.top() >=1) {
			Pointer<World> world = tuki_cast<World>(L[1]);
			if(world) add(world);
		}
	}

	void WorldSorter::remove(luapp::Stack &L)
	{
		if(L.top() >=1) {
			Pointer<World> world = tuki_cast<World>(L[1]);
			if(world) remove(world);
		}
	}

	namespace {
		/* �ʏ�̂R�c�A�v���Ƃ��Ĉ�ʓI�ȃf�o�C�X�ݒ���s��
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
	// WorldRenderer
	// 
	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, WorldRenderer, WorldSorter);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, WorldRenderer)
		TUKI_METHOD(WorldRenderer, add)
		TUKI_METHOD(WorldRenderer, remove)
	TUKI_IMPLEMENT_END(WorldRenderer)

	WorldRenderer::WorldRenderer()
	{
		sb_ = new DefaultSB();
		sb_->setUp();
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
		WorldSorter::onReach(delta);
#ifndef __ee__
		graphic::setAmbient(graphic::getAmbient());
		graphic::clearLight();
#endif
		sb_->begin();
		packets.draw();
		return false;
	}

	bool WorldRenderer::onLeave(float delta) const
	{
		sb_->end();
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
		sb_ = new DefaultSB();
		sb_->setUp();
	}

	bool OpequeWorldRenderer::setUp(luapp::Stack &L)
	{
		// Context:create�Ő��삷��
		return false;
	}

	void OpequeWorldRenderer::attach(luapp::Stack &L)
	{
		if(L.top() >=1) {
			Pointer<WorldSorter> target = tuki_cast<WorldSorter>(L[1]);
			if(target) target_ = target;
		}
	}

	/** ���[���h�`��
	 *
	 * �����_�����O�c���[�ɕ`�惁�b�Z�[�W�𑗂�
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/11 2:34:02
	 */	
	bool OpequeWorldRenderer::onReach(float delta) const
	{
#ifndef __ee__
		graphic::setAmbient(graphic::getAmbient());
		graphic::clearLight();
#endif
		if(target_) {
			sb_->begin();
			target_->packets.drawOpeque();
		}
		return false;
	}

	bool OpequeWorldRenderer::onLeave(float delta) const
	{
		if(target_) sb_->end();
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
		sb_ = new DefaultSB();
		sb_->setUp();
	}

	bool TranslucentWorldRenderer::setUp(luapp::Stack &L)
	{
		// Context:create�Ő��삷��
		return false;
	}

	void TranslucentWorldRenderer::attach(luapp::Stack &L)
	{
		if(L.top() >=1) {
			Pointer<WorldSorter> target = tuki_cast<WorldSorter>(L[1]);
			if(target) target_ = target;
		}
	}

	/** ���[���h�`��
	 *
	 * �����_�����O�c���[�ɕ`�惁�b�Z�[�W�𑗂�
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/11 2:34:02
	 */	
	bool TranslucentWorldRenderer::onReach(float delta) const
	{
#ifndef __ee__
		graphic::setAmbient(graphic::getAmbient());
		graphic::clearLight();
#endif
		if(target_) {
			sb_->begin();
			target_->packets.drawTranslucent();
		}
		return false;
	}

	bool TranslucentWorldRenderer::onLeave(float delta) const
	{
		if(target_) sb_->end();
		return true;
	}

}} // namespace gctp::scene