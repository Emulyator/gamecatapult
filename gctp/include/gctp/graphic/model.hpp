#ifndef _GCTP_GRAPHIC_MODEL_HPP_
#define _GCTP_GRAPHIC_MODEL_HPP_
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

	/** D3DXメッシュ頂点データのロックと自動開放
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 15:38:20
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class MeshVertexLock {
		ID3DXMeshPtr mesh_;
		void *base_;
		void *p_;
	public:
		MeshVertexLock(ID3DXMeshPtr mesh)
		{
			if(mesh && HRslt(mesh->LockVertexBuffer(0, &p_)) && p_) {
				base_ = p_;
				mesh_ = mesh;
			}
		}
		~MeshVertexLock()
		{
			if(mesh_) mesh_->UnlockVertexBuffer();
		}
		void step()
		{
			p_ = ((uchar *)p_) + D3DXGetFVFVertexSize(mesh_->GetFVF());
		}

		Vector &operator[](ushort i)
		{
			return *reinterpret_cast<Vector *>(((uchar *)base_) + D3DXGetFVFVertexSize(mesh_->GetFVF())*i);
		}
		const Vector &operator[](ushort i) const
		{
			return *reinterpret_cast<const Vector *>(((const uchar *)base_) + D3DXGetFVFVertexSize(mesh_->GetFVF())*i);
		}
		void *get() { return p_; }
		const void *get() const { return p_; }
		operator Vector *() { return reinterpret_cast<Vector *>(p_); }
		operator const Vector *() const { return reinterpret_cast<Vector *>(p_); }
	};

	/** D3DXメッシュインデックスデータのロックと自動開放
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 15:38:20
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class MeshIndexLock {
		ID3DXMeshPtr mesh_;
		void *base_;
		void *p_;
	public:
		MeshIndexLock(ID3DXMeshPtr mesh)
		{
			if(mesh && HRslt(mesh->LockIndexBuffer(0, &p_)) && p_) {
				base_ = p_;
				mesh_ = mesh;
			}
		}
		~MeshIndexLock()
		{
			if(mesh_) mesh_->UnlockIndexBuffer();
		}
		void step()
		{
			p_ = ((uchar *)p_) + sizeof(ushort);
		}

		ushort &operator[](std::size_t i)
		{
			return *reinterpret_cast<ushort *>(((uchar *)base_) + sizeof(ushort)*i);
		}
		const ushort &operator[](std::size_t i) const
		{
			return *reinterpret_cast<const ushort *>(((const uchar *)base_) + sizeof(ushort)*i);
		}
		void *get() { return p_; }
		const void *get() const { return p_; }
		operator ushort *() { return reinterpret_cast<ushort *>(p_); }
		operator const ushort *() const { return reinterpret_cast<ushort *>(p_); }
	};

	//class VertexBuffer;
	//class IndexBuffer;
	/** ワイヤフレームモデル（線リスト）
	 *
	 * LWで言うところの線ポリゴンの集まり
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/11/21 13:04:28
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class WireMesh : public Object {
	public:
		/// すでにセットアップされていたら、レストア
		HRslt setUp(const void *data, const void *mtrllist, ID3DXBufferPtr mtrls, ulong mtrl_num);
		
		/// 描画
		HRslt draw() const;
		HRslt draw(int subset) const;

		float calcRadius(const Vector &center) const;

		/// AABBを計測
		AABox getAABB() const;
		/// ワールド座標系でのAABBを返す
		AABox getAABB(const Matrix &mat) const;

	protected:
		Pointer<VertexBuffer>	vbuf_;
		Pointer<IndexBuffer>	ibuf_;
		uint					num_;
		std::vector< std::pair<int, int> > subset_;
		uint					subsetnum_;
	};

	class Model;
	/**モデル詳細基底クラス
	 *
	 * 今のところスキンモデルばかり
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 16:44:46
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class ModelDetail : public Object {
	public:
		ModelDetail(Model &owner) : owner_(owner) {}
		virtual ~ModelDetail() {}
		virtual HRslt draw(const Skeleton &skl) const = 0;
		virtual HRslt draw(const Skeleton &skl, int mtrlno) const = 0;
		//virtual HRslt begin(int mtrlno) const = 0;
		//virtual HRslt drawFragment(const Skeleton &skel) const = 0;
		//virtual HRslt end() const = 0;
		Model &owner_;
	};

	/** モデルクラス
	 *
	 * @todo どうにかして、DX固有の部分を追い出したいが…
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 15:10:10
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Model : public Object {
		friend class ModelDetail;
	public:
		/// すでにセットアップされていたら、レストア
		void setUp(CStr name, ID3DXMeshPtr mesh, ID3DXSkinInfoPtr skin, ID3DXBufferPtr adjc);
		/// すでにセットアップされていたら、レストア
		HRslt setUpWire(CStr name, const void *data, const void *mlist, ID3DXBufferPtr mtrls, ulong mtrl_num);
		
		/// 描画
		HRslt draw(const Matrix &mat) const;
		/// 描画
		HRslt draw(const Skeleton &skel) const;

		/// マテリアルを指定して描画、の準備
		HRslt begin() const;
		/// マテリアルを指定して描画
		HRslt drawSubset(const Matrix &mat, int mtrlno) const;
		/// マテリアルを指定して描画
		HRslt drawSubset(const Skeleton &skl, int mtrlno) const;
		/// マテリアルを指定して描画終了
		HRslt end() const;

		/// マテリアルリスト
		std::vector< Material >	mtrls;
		/// 面接続情報
		const ulong *adjacency() const { return reinterpret_cast<ulong*>(adjc_->GetBufferPointer()); }
		/// メッシュ断片情報
		const std::vector<D3DXATTRIBUTERANGE> &subsets() const { return subsets_; }

		Model() : bs_(VectorC(0,0,0),0), offset_(0) { vb_.deleteGuard(); ib_.deleteGuard(); }

		/// 境界球
		const Sphere &bs() { return bs_; }

		/// 境界球を再計算
		void updateBS()
		{
			// 境界球を計算
			bs_.c = calcCenter();
			if(wire_) {
				bs_.r = wire_->calcRadius(bs_.c);
			}
			else {
				bs_.r = calcRadius(bs_.c);
			}
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
		/// 使用するgctp::Shaderを返す
		Handle<Shader> shader() const { return shader_; }
		/// 使用するgctp::Shaderを設定する
		void setShader(Handle<Shader> shader);

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

	protected:
		CStr								name_;		// シーンファイル上での名前
		ID3DXMeshPtr						mesh_;      // ファイルから読み込んだままののメッシュ
		ID3DXSkinInfoPtr					skin_;		// スキン情報
		ID3DXBufferPtr						adjc_;		// 面の接続情報
		std::vector<D3DXATTRIBUTERANGE>		subsets_;	// 断片情報
		std::vector<D3DVERTEXELEMENT9>		vbinfo_;
		mutable VertexBuffer				vb_;
		mutable IndexBuffer					ib_;
		dx::IDirect3DVertexDeclarationPtr	decl_;

		Vector calcCenter() const;
		float calcRadius(const Vector &center) const;

	private:
		Pointer<ModelDetail> detail_;
		Pointer<WireMesh>    wire_; // あくまで表示物はModel、という設計を守るための暫定処置…
		mutable Handle<Shader> shader_;
		Sphere bs_;
		int offset_;
	};

}} //namespace gctp

#endif //_GCTP_GRAPHIC_MODEL_HPP_