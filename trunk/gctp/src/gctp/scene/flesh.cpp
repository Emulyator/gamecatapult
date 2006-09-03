/** @file
 * GameCatapult モデルインスタンスクラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: modeltag.cpp,v 1.3 2005/02/21 02:21:12 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/scene/flesh.hpp>
#include <gctp/graphic/model.hpp>
#include <gctp/scene/camera.hpp>

using namespace std;

namespace gctp { namespace scene {

	GCTP_IMPLEMENT_CLASS_NS(gctp, Flesh, Object);

	HRslt Flesh::setUp(Handle<graphic::Model> model, Handle<StrutumNode> node)
	{
		dissolve_rate_ = 1.0f;
		model_ = model;
		node_ = node;
		skl_ = 0;
		return S_OK;
	}

	HRslt Flesh::setUp(Handle<graphic::Model> model, Handle<Skeleton> skl)
	{
		dissolve_rate_ = 1.0f;
		model_ = model;
		if(skl) node_ = *skl.lock();
		skl_ = skl;
		return S_OK;
	}

	HRslt Flesh::setUp(Handle<graphic::Model> model, Handle<Skeleton> skl, Handle<StrutumNode> node)
	{
		dissolve_rate_ = 1.0f;
		model_ = model;
		node_ = node;
		skl_ = skl;
		return S_OK;
	}

	bool Flesh::draw() const
	{
		if(Camera::current().isVisible(bs_)) {
			if(model_ && dissolve_rate_ > 0.0f) {
				Pointer<graphic::Model> model = model_.lock();
				if(model->isSkinned()) {
					Pointer<Skeleton> skl = skl_.lock();
					if(skl) model->draw(*skl);
					// エラー表示したほうがいいのかな
				}
				else if(node_) {
					Pointer<StrutumNode> node = node_.lock();
					model->draw(node->val.wtm());
					// エラー表示したほうがいいのかな
				}
			}
			return true;
		}
		//PRNN("culled");
		return false;
	}

	/** バウンシングスフィアを更新が必要なら、再計算する
	 *
	 * @return 更新されたか？
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/04/01 3:15:37
	 */
	bool Flesh::update()
	{
		Pointer<StrutumNode> node = node_.lock();
		if(node && node->val.isUpdated()) {
			calcBS();
			return true;
		}
		return false;
	}

	void Flesh::calcBS()
	{
		Pointer<StrutumNode> node = node_.lock();
		Pointer<graphic::Model> model = model_.lock();
		if(node && model) {
			bs_.c = node->val.wtm() * model->bs().c;
			float scale = node->val.wtm().right().length();
			float scale2 = node->val.wtm().up().length();
			if(scale2 > scale) scale = scale2;
			scale2 = node->val.wtm().at().length();
			if(scale2 > scale) scale = scale2;
			bs_.r = model->bs().r * scale;
		}
	}

}} // namespace gctp::scene
