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

	bool VisibilityTester::isVisible(const Sphere &bs) const
	{
		if(enable_frustum_test) return Camera::current().isVisible(bs);
		return true;
	}

	VisibilityTester &VisibilityTester::current()
	{
		static VisibilityTester instance;
		return instance;
	}

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, Flesh, Object);

	HRslt Flesh::setUp(Handle<graphic::Model> model, Handle<StrutumNode> node)
	{
		dissolve_rate_ = 1.0f;
		model_ = model;
		node_ = node;
		skl_ = 0;
		calcBS();
		return S_OK;
	}

	HRslt Flesh::setUp(Handle<graphic::Model> model, Handle<Skeleton> skl)
	{
		dissolve_rate_ = 1.0f;
		model_ = model;
		if(skl) node_ = skl->root();
		skl_ = skl;
		calcBS();
		return S_OK;
	}

	HRslt Flesh::setUp(Handle<graphic::Model> model, Handle<Skeleton> skl, Handle<StrutumNode> node)
	{
		dissolve_rate_ = 1.0f;
		model_ = model;
		node_ = node;
		skl_ = skl;
		calcBS();
		return S_OK;
	}

	bool Flesh::draw() const
	{
		if(VisibilityTester::current().isVisible(bs_)) {
			if(model_ && dissolve_rate_ > 0.0f) {
				Pointer<graphic::Model> model = model_.lock();
				if(model->isSkin()) {
					Pointer<Skeleton> skl = skl_.lock();
					if(skl) model->draw(*skl);
					// エラー表示したほうがいいのかな
				}
				else if(node_) {
					Pointer<StrutumNode> node = node_.lock();
					if(node) model->draw(node->val.wtm());
					// エラー表示したほうがいいのかな
				}
			}
			return true;
		}
		//PRNN("culled");
		return false;
	}

	void Flesh::pushPackets(DrawPacketVector &packets) const
	{
		if(VisibilityTester::current().isVisible(bs_)) {
			if(model_ && dissolve_rate_ > 0.0f) {
				DrawPacket packet;
				packet.model = model_;
				packet.flesh = const_cast<Flesh *>(this);
				Pointer<graphic::Model> model = model_.lock();
				for(uint i = 0; i < model->subsets().size(); i++) {
					packet.shader = model->mtrls[model->subsets()[i].material_no].shader;
					packet.subset_no = i;
					if(dissolve_rate_ >= 1.0f && model->mtrls[model->subsets()[i].material_no].blend == graphic::Material::OPEQUE && model->mtrls[model->subsets()[i].material_no].diffuse.a >= 1.0f) {
						packet.z = -1;
					}
					else {
						Vector c = Camera::current().viewprojection()*bs_.c;
						if(c.z > 1.0f) c.z = 1.0f;
						// とりあえず。。。
						packet.z = static_cast<uint>(c.z*USHRT_MAX);
					}
					packets.push_back(packet);
				}
			}
		}
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
