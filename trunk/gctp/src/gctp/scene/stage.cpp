/** @file
 * GameCatapult �X�e�[�W�N���X�N���X
 *
 * update_signal(��Ԃ̍X�V),animate_signal(�X�V�������ŃA�j���[�V�����X�V�i��ʂ���啝�ɊO�ꂽ����s����Ȃ��B�s��c���[�ɐώZ�����~�ߐݒ�j),
 * predraw_signal(�`����ԂłȂ��A���[���h���W�n�����ɏ����ł���`�����X�B�Փˉ����͂����Ɉړ�),draw_signal
 * �̎l�ɕ�����ׂ�����
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/stage.hpp>
#include <gctp/graphic.hpp>
#include <gctp/graphic/dx/stateblock.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/context.hpp>
#include <gctp/scene/motion.hpp>
#include <gctp/scene/graphfile.hpp>
#include <gctp/scene/camera.hpp>
#include <gctp/scene/body.hpp>
#include <gctp/scene/light.hpp>
#include <gctp/app.hpp>
#include <gctp/dbgout.hpp>

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

	GCTP_IMPLEMENT_CLASS_NS(gctp, Stage, Object);
	TUKI_IMPLEMENT_BEGIN_NS(Scene, Stage)
		TUKI_METHOD(Stage, load)
		TUKI_METHOD(Stage, activate)
		TUKI_METHOD(Stage, show)
		TUKI_METHOD(Stage, hide)
	TUKI_IMPLEMENT_END(Stage)

	Stage* Stage::current_ = NULL;	///< �J�����g�X�e�[�W�i���̃X�e�[�W��update�Adraw�c�Ȃǂ̊Ԃ����L���j

	Stage::Stage()
	{
		update_slot.bind(this);
		draw_slot.bind(this);
		strutum_tree.setUp();
		dsb_ = new DefaultSB();
		dsb_->setUp();
	}

	void Stage::setUp(const _TCHAR *filename)
	{
		// XFile����LightNode��Camera��Entity���Z�b�g�A�b�v����
		// �Ȃ񂾂��ǁA�W����XFile�ł́Axfile=Entity�ɂ��邵���Ȃ���Ȃ�(Animation���K���P�c���[�ɂȂ����Ⴄ����j
		// gctp�g��������ꍇ�̂ݏ�肭�����A�Ƃ��邩
		// �Ƃ肠�����A�����q�G�����L�[�ւ̑Ή����P�c���[�ւ̍����ōs���A�Ƃ����̂�Stage��XFile���󂯕t������悤�ɂȂ��
		// �s�v�Ȃ̂Ŏ~�߂悤
		// ���̑���AEntity��XFile��n����1�c���[��v������A��������ƂQ�ڂ͖����A�Ƃ����ލs�d�l��
		if(filename) {
			Pointer<GraphFile> file = context()[filename].lock();
			if(file) {
				Pointer<Light> plight;
				Pointer<Body> pbody;
				for(GraphFile::iterator i = file->begin(); i != file->end(); ++i) {
					pbody = *i;
					if(pbody) {
						body_list.push_back(pbody);
						strutum_tree.root()->push(*pbody);
					}
					plight = *i;
					if(plight) {
					}
				}
			}
		}
	}

	/** �V�[���X�V
	 *
	 * �X�V�V�O�i���ƏՓˉ����V�O�i�����s
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/11 2:34:02
	 */	
	bool Stage::onUpdate(float delta)
	{
#ifdef _MT
		ScopedLock al(monitor_);
#endif
		return doOnUpdate(delta);
	}

	bool Stage::doOnUpdate(float delta)
	{
		backup_current_ = current_;
		current_ = this;
		update_signal(delta);
		strutum_tree.setTransform();
		for(HandleList<Body>::iterator i = body_list.begin(); i != body_list.end(); ++i) {
			Pointer<Body> body = i->lock();
			if(body) body->update();
		}
		postupdate_signal(delta);
		current_ = backup_current_;
		return true;
	}

	/** �V�[���`��
	 *
	 * �����_�����O�c���[�ɕ`�惁�b�Z�[�W�𑗂�
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/11 2:34:02
	 */	
	bool Stage::onDraw(float delta) const
	{
#ifdef _MT
		ScopedLock al(monitor_);
#endif
		return doOnDraw(delta);
	}
	
	bool Stage::doOnDraw(float delta) const
	{
		backup_current_ = const_cast<Stage *>(current_);
		current_ = const_cast<Stage *>(this);
#ifndef __ee__
		graphic::setAmbient(graphic::getAmbient());
		graphic::clearLight();
#endif
		dsb_->setCurrent();
		Stage *stage = const_cast<Stage *>(this);
		for(HandleList<Body>::iterator i = stage->body_list.begin(); i != stage->body_list.end();) {
			if(*i) {
				(*i)->draw(); // �p�X�Ɋւ�����������ő���ׂ��H
				++i;
			}
			else i = stage->body_list.erase(i);
		}
		dsb_->unset();
		current_ = backup_current_;
		return true;
	}

	bool Stage::setUp(luapp::Stack &L)
	{
		// Context:create�Ő��삷��
		return false;
	}

	void Stage::load(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			if(L[1].isString()) {
#ifdef UNICODE
				setUp(WCStr(L[1].toCStr()).c_str());
#else
				setUp(L[1].toCStr());
#endif
			}
		}
	}

	void Stage::activate(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			if(L[1].toBoolean()) app().update_signal.connectOnce(update_slot);
			else app().update_signal.disconnect(update_slot);
		}
		else app().update_signal.connectOnce(update_slot);
	}

	void Stage::show(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			Pointer<Camera> cam = tuki_cast<Camera>(L[1]);
			if(cam) cam->draw_signal.connectOnce(draw_slot);
		}
	}

	void Stage::hide(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			Pointer<Camera> cam = tuki_cast<Camera>(L[1]);
			if(cam) cam->draw_signal.disconnect(draw_slot);
		}
	}

}} // namespace gctp::scene
