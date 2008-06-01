#ifndef _GCTP_SCENE_FLESH_HPP_
#define _GCTP_SCENE_FLESH_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult モデルインスタンスクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/28 12:42:22
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/sphere.hpp>
#include <gctp/strutumnode.hpp>
#include <gctp/class.hpp>
#include <gctp/scene/drawpacket.hpp>

namespace gctp {
	class Skeleton;
	namespace graphic {
		class Model;
	}
}

namespace gctp { namespace scene {

	class ModelMixer;
	/** モデルインスタンス
	 *
	 * Modelを描画可能にしたもの
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/11/21 17:55:36
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Flesh : public Object {
	public:
		/// セットアップ
		HRslt setUp(Handle<graphic::Model> model, Handle<StrutumNode> node);
		/// セットアップ
		HRslt setUp(Handle<graphic::Model> model, Handle<Skeleton> skl);
		/// セットアップ
		HRslt setUp(Handle<graphic::Model> model, Handle<Skeleton> skl, Handle<StrutumNode> node);

		/// 描画
		bool draw() const;
		/// 描画パケット登録
		void push(DrawPacketVector &packets) const;

		/// （マテリアルとは別の）透明度設定。0で非表示。（シェーダーによっては、働かないかも）
		float setDissolveRate(float dissolve_rate) { std::swap(dissolve_rate, dissolve_rate_); return dissolve_rate; }
		/// （マテリアルとは別の）透明度設定値。0で非表示。
		float dissolveRate() const { return dissolve_rate_; }

		/// 参照しているモデルへのポインタ
		Handle<graphic::Model> model() const { return model_ ; }
		/// アタッチしているノード
		Handle<StrutumNode> node() const { return node_; }
		/// 所属するスケルトン
		Handle<Skeleton> skeleton() const { return skl_; }

		/// 境界球
		const Sphere &bs() const { return bs_; }
		/// 境界球の更新
		bool update();

	GCTP_DECLARE_CLASS
	private:
		void calcBS();
		Pointer<ModelMixer>		modelmixer_;
		Handle<graphic::Model>	model_;
		Handle<StrutumNode>		node_;
		Handle<Skeleton>		skl_;
		Sphere bs_;
		float dissolve_rate_;
	};

}} //namespace gctp::scene

#endif //_GCTP_SCENE_FLESH_HPP_