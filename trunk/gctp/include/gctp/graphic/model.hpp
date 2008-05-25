#ifndef _GCTP_GRAPHIC_MODEL_HPP_
#define _GCTP_GRAPHIC_MODEL_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult モデルクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/28 12:42:22
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/graphic.hpp>
#include <gctp/line.hpp>
#include <gctp/sphere.hpp>
#include <gctp/aabox.hpp>
#include <gctp/skeleton.hpp>
#include <gctp/graphic/material.hpp>
#include <gctp/dxcomptrs.hpp>

#include <gctp/graphic/vertexbuffer.hpp>
#include <gctp/graphic/indexbuffer.hpp>

struct ID3DXMesh;
struct ID3DXSkinInfo;
namespace gctp {
	TYPEDEF_DXCOMPTR(ID3DXMesh);
	TYPEDEF_DXCOMPTR(ID3DXSkinInfo);
}

namespace gctp { namespace graphic {

	class Texture;
	class Shader;
	class Brush;

	struct SubsetInfo
	{
		uint material_no;
		uint index_offset;
		uint primitive_num;
		uint vertex_offset;
		uint vertex_num;
	};

	/** モデルクラス
	 *
	 * @todo どうにかして、DX固有の部分を追い出したいが…
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 15:10:10
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Model : public Object {
		friend class Brush;
	public:
		enum Type {
			TYPE_POLYGON,
			TYPE_LINE,
		};
		/// すでにセットアップされていたら、レストア
		void setUp(CStr name, ID3DXMeshPtr mesh, ID3DXSkinInfoPtr skin, ID3DXBufferPtr adjc);
		/// すでにセットアップされていたら、レストア
		HRslt setUpWire(CStr name, const void *data, const void *mlist, ID3DXBufferPtr mtrls, ulong mtrl_num);
		
		/// 描画
		HRslt draw(const Matrix &mat) const;
		/// 描画
		HRslt draw(const Skeleton &skel) const;

		/** マテリアルを指定して描画、の準備
		 *
		 * @param template_mtrlno セットアップするBrush/Shaderを代表するマテリアル番号。
		 * 同じBrush/Shaderの組をさすのであれば、どのマテリアルでもかまわない
		 */
		HRslt begin(int template_mtrlno, int passno) const;
		/// マテリアルを指定して描画
		HRslt draw(const Matrix &mat, int mtrlno) const;
		/// マテリアルを指定して描画
		HRslt draw(const Skeleton &skl, int mtrlno) const;
		/// マテリアルを指定して描画終了
		HRslt end() const;

		/// マテリアルリスト
		std::vector<Material> mtrls;
		/// 面接続情報
		const ulong *adjacency() const { return reinterpret_cast<ulong*>(adjc_->GetBufferPointer()); }
		/// メッシュ断片情報
		const std::vector<SubsetInfo> &subsets() const { return subsets_; }

		Model() : bs_(VectorC(0,0,0),0), offset_(0) { vb_.deleteGuard(); ib_.deleteGuard(); }

		/// 境界球
		const Sphere &bs() { return bs_; }

		/// 境界球を再計算
		void updateBS()
		{
			// 境界球を計算
			bs_.c = calcCenter();
			bs_.r = calcRadius(bs_.c);
		}

		/// AABBを返す
		AABox getAABB() const;
		/// ワールド座標系でのAABBを返す
		AABox getAABB(const Matrix &mat) const;
		/// 交差テスト
		bool hasIntersected(const RayLine &ray, ulong &idx, float &u, float &v, float &dist) const;
		/// 交差テスト
		bool hasIntersected(const Model &with, Vector &mytouch, Vector &histouch) const;

		/// 既存モデルから複写
		HRslt copy(const Model &src);

		/// スキンをアップデート
		HRslt update(const Model &src, const Matrix &mat);

		/// スキンモデルか？
		bool isSkin();

		/// スキンモデル化済みか？
		bool isSkinned();

		/// スキンモデルをやめる
		void solidize();
		/// ソフトウェアスキンモデル化
		HRslt useSoftware();
		/// 頂点ブレンドスキンモデル化
		HRslt useBlended();
		/// インデックス化頂点ブレンドスキンモデル化
		HRslt useIndexed();
		/// 頂点シェーダースキンモデル化
		HRslt useVS();
		/// HLSLによるスキンモデル化
		HRslt useShader();

		/// D3DXメッシュを返す
		ID3DXMeshPtr mesh() const { return mesh_; }
		/// D3DXスキン情報を返す
		ID3DXSkinInfoPtr skin() const { return skin_; }

		/// ボーン名
		const char *bonename(uint i) { if(skin_) return skin_->GetBoneName(i); else return NULL; }
		/// ボーンオフセット行列
		Matrix *bone(uint i) { if(skin_) return reinterpret_cast<Matrix *>(skin_->GetBoneOffsetMatrix(i)); else return NULL; }
		/// ボーン数
		uint boneNum() { if(skin_) return skin_->GetNumBones(); else return 0; }

		/// 名前
		const char *name() const { return name_.c_str(); }
		/// 名前をつける
		void setName(const char *str) { name_ = str; }
		/// （モーフ時の）オフセットを設定。差分モデルの場合これが必要
		void setOffset(int offset) { offset_ = offset; }
		/// （モーフ時の）オフセット
		int offset(int offset) { return offset_; }

		/// ブレンド
		void blend(const Model **models, Real *weights, int num);

		VertexBuffer &vertexbuffer() { return vb_; }
		const VertexBuffer &vertexbuffer() const { return vb_; }
		IndexBuffer &indexbuffer() { return ib_; }
		const IndexBuffer &indexbuffer() const { return ib_; }

	protected:
		CStr								name_;		// シーンファイル上での名前
		Type								type_;

		ID3DXMeshPtr						mesh_;      // ファイルから読み込んだままののメッシュ
		ID3DXSkinInfoPtr					skin_;		// スキン情報
		ID3DXBufferPtr						adjc_;		// 面の接続情報

		std::vector<SubsetInfo>				subsets_;	// 断片情報
		mutable VertexBuffer				vb_;
		mutable IndexBuffer					ib_;

		Vector calcCenter() const;
		float calcRadius(const Vector &center) const;

	private:
		mutable int current_template_mtrlno_;
		//std::vector< Pointer<Brush> > brushes_;
		Pointer<Brush> brush_;
		Sphere bs_;
		int offset_;
	};

}} //namespace gctp

#endif //_GCTP_GRAPHIC_MODEL_HPP_