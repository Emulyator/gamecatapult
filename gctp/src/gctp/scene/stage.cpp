/** @file
 * GameCatapult ステージクラスクラス
 *
 * update_signal(状態の更新),animate_signal(更新した情報でアニメーション更新（画面から大幅に外れたら実行されない。行列ツリーに積算堰き止め設定）),
 * predraw_signal(描画期間でなく、ワールド座標系を元に処理できるチャンス。衝突解決はここに移動),draw_signal
 * の四つに分けるべきかな\n
 * とりあえず、detect_collision_signalって名前は用途を特定しすぎ｡単に描画期間以外でワールド座標系を元に処理できるチャンスだと強調すべき
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
#include <gctp/scene/body.hpp>
#include <gctp/scene/light.hpp>
#include <gctp/app.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace scene {

	namespace {
		/* 通常の３Ｄアプリとして一般的なデバイス設定を行う
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/14 22:53:32
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		class DefualtSB : public graphic::dx::StateBlockRsrc {
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

		// ルートレンダリングノード
		class RootRenderer :  public Renderer {
			DefualtSB sb_;
		public:
			RootRenderer()
			{
				sb_.setUp();
			}

			bool onEnter() const
			{
				sb_.setCurrent();
				return true;
			}
			bool onLeave() const
			{
				sb_.unSet();
				return true;
			}
		};
	}

	GCTP_IMPLEMENT_CLASS_NS(gctp, Stage, Object);
	TUKI_IMPLEMENT_BEGIN_NS(Scene, Stage)
		TUKI_METHOD(Stage, load)
	TUKI_IMPLEMENT_END(Stage)

	Stage* Stage::current_ = NULL;	///< カレントステージ（そのステージのupdate、draw…などの間だけ有効）

	Stage::Stage()
	{
		update_slot.bind(this);
		draw_slot.bind(this);
		strutum_tree.setUp();
		rendering_tree.setUp(new RootRenderer);
	}

	bool Stage::insert(const Hndl hndl, const char *key)
	{
		if(hndl) {
			if(key) return DB::insert(key, hndl);
			else return true;
		}
		return false;
	}

	void Stage::setUp(const char *filename)
	{
		// XFileからLightNodeやCameraやEntityをセットアップする
		// なんだけど、標準のXFileでは、xfile=Entityにするしかないよなぁ(Animationが必ず１ツリーになっちゃうから）
		// gctp拡張がある場合のみ上手くいく、とするか
		// とりあえず、複数ヒエラルキーへの対応を１ツリーへの合成で行う、というのはStageがXFileを受け付けられるようになれば
		// 不要なので止めよう
		// その代わり、EntityにXFileを渡すと1ツリーを要求する、複数あると２個目は無視、という退行仕様に
		if(filename) {
			Pointer<GraphFile> file = db()[filename].lock();
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

	/** シーン更新
	 *
	 * 更新シグナルと衝突解決シグナル発行
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/11 2:34:02
	 */	
	bool Stage::onUpdate(float delta)
	{
#ifdef _MT
		boost::mutex::scoped_lock al(monitor_);
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

	/** シーン描画
	 *
	 * レンダリングツリーに描画メッセージを送る
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/11 2:34:02
	 */	
	bool Stage::onDraw() const
	{
#ifdef _MT
		boost::mutex::scoped_lock al(monitor_);
#endif
		return doOnDraw();
	}
	
	bool Stage::doOnDraw() const
	{
		backup_current_ = const_cast<Stage *>(current_);
		current_ = const_cast<Stage *>(this);
#ifndef __ee__
		graphic::setAmbient(graphic::getAmbient());
		graphic::clearLight();
#endif
		rendering_tree.draw();
		current_ = backup_current_;
		return true;
	}

	/** 新規シーンノード製作
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 1:48:39
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Hndl Stage::newNode(Context &context, const char *classname, const char *name, const char *srcfilename)
	{
		if(srcfilename) context.load(srcfilename);
		Ptr ret = context.create(classname).lock();
		if(ret) {
			//if(srcfilename) ret->setUp(srcfilename);
			//ret->chain(*this);
			insert(ret, name);
		}
		return ret.get();
	}

	/** 新規シーンノード製作
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 1:48:39
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Hndl Stage::newNode(Context &context, const GCTP_TYPEINFO &typeinfo, const char *name, const char *srcfilename)
	{
		if(srcfilename) context.load(srcfilename);
		Ptr ret = context.create(typeinfo).lock();
		if(ret) {
			//if(srcfilename) ret->setUp(srcfilename);
			//ret->chain(*this);
			insert(ret, name);
		}
		return ret.get();
	}

	bool Stage::setUp(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			const char *fname = L[1].toCStr();
			if(fname) {
				context().load(fname);
				setUp(fname);
			}
		}
		app().update_signal.connectOnce(update_slot);
		app().draw_signal.connectOnce(draw_slot);
		return true;
	}

	void Stage::load(luapp::Stack &L)
	{
		if(L.top() >= 2) {
			if(L[2].toCStr()) setUp(L[2].toCStr());
		}
	}

	//////////////////////////////
	// StageRenderer
	//
	GCTP_IMPLEMENT_CLASS_NS(gctp, StageRenderer, Renderer);

	/// コンストラクタ
	StageRenderer::StageRenderer(Handle<Stage> stage) : stage_(stage)
	{
	}

	/// ノード到達時の処理
	bool StageRenderer::onEnter() const
	{
		Pointer<Stage> stage = stage_.lock();
		if(stage) {
			for(HandleList<Body>::iterator i = stage->body_list.begin(); i != stage->body_list.end();) {
				if(*i) {
					(*i)->draw(); // パスに関する情報をここで送るべき？
					++i;
				}
				else i = stage->body_list.erase(i);
			}
		}
		return true;
	}

}} // namespace gctp::scene
