/** @file
 * GameCatapult ワールドクラス
 *
 * update_signal(状態の更新),animate_signal(更新した情報でアニメーション更新（画面から大幅に外れたら実行されない。行列ツリーに積算堰き止め設定）),
 * predraw_signal(描画期間でなく、ワールド座標系を元に処理できるチャンス。衝突解決はここに移動),draw_signal
 * の四つに分けるべきかな
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/world.hpp>
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

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, World, Object);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, World)
		TUKI_METHOD(World, load)
		TUKI_METHOD(World, activate)
	TUKI_IMPLEMENT_END(World)

	World* World::current_ = NULL;	///< カレントステージ（そのステージのupdate、draw…などの間だけ有効）

	World::World()
	{
		update_slot.bind(this);
		strutum_tree.setUp();
	}

	void World::setUp(const _TCHAR *filename)
	{
		// XFileからLightNodeやCameraやEntityをセットアップする
		// なんだけど、標準のXFileでは、xfile=Entityにするしかないよなぁ(Animationが必ず１ツリーになっちゃうから）
		// gctp拡張がある場合のみ上手くいく、とするか
		// とりあえず、複数ヒエラルキーへの対応を１ツリーへの合成で行う、というのはWorldがXFileを受け付けられるようになれば
		// 不要なので止めよう
		// その代わり、EntityにXFileを渡すと1ツリーを要求する、複数あると２個目は無視、という退行仕様に
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

	/** シーン更新
	 *
	 * 更新シグナルと衝突解決シグナル発行
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/11 2:34:02
	 */	
	bool World::onUpdate(float delta)
	{
#ifdef _MT
		ScopedLock al(monitor_);
#endif
		return doOnUpdate(delta);
	}

	bool World::doOnUpdate(float delta)
	{
		begin();
			update_signal(delta);
			strutum_tree.setTransform();
			for(HandleList<Body>::iterator i = body_list.begin(); i != body_list.end(); ++i) {
				Pointer<Body> body = i->lock();
				if(body) body->update();
			}
			postupdate_signal(delta);
		end();
		return true;
	}

	/** 描画・更新開始
	 *
	 * ロックしてカレントにする。
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/11 2:34:02
	 */	
	void World::begin() const
	{
#ifdef _MT
		monitor_.lock();
#endif
		backup_current_ = const_cast<World *>(current_);
		current_ = const_cast<World *>(this);
#ifndef __ee__
		graphic::setAmbient(graphic::getAmbient());
		graphic::clearLight();
#endif
	}

	/** 描画・更新終了
	 *
	 * アンロックしてカレントからはずす。
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/11 2:34:02
	 */	
	void World::end() const
	{
		current_ = backup_current_;
#ifdef _MT
		monitor_.unlock();
#endif
	}
	
	void World::draw() const
	{
		GCTP_ASSERT(this == current_);
		World *self = const_cast<World *>(this);
		for(HandleList<Body>::iterator i = self->body_list.begin(); i != self->body_list.end();) {
			if(*i) {
				(*i)->draw(); // パスに関する情報をここで送るべき？
				++i;
			}
			else i = self->body_list.erase(i);
		}
	}

	bool World::setUp(luapp::Stack &L)
	{
		// Context:createで製作する
		return false;
	}

	void World::load(luapp::Stack &L)
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

	void World::activate(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			if(L[1].toBoolean()) app().update_signal.connectOnce(update_slot);
			else app().update_signal.disconnect(update_slot);
		}
		else app().update_signal.connectOnce(update_slot);
	}

}} // namespace gctp::scene
