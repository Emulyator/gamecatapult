#ifndef _GCTP_GRAPHIC_DX_SKINBRUSH_HPP_
#define _GCTP_GRAPHIC_DX_SKINBRUSH_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult スキンメッシュクラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: model.cpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include <gctp/graphic/brush.hpp>
#include <gctp/graphic/rsrc.hpp>
#include <gctp/graphic/light.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/graphic/dx/hlslshader.hpp>
#include <gctp/context.hpp>

namespace gctp { namespace graphic { namespace dx {

	/// スキンブラシ基底クラス
	class SkinBrush : public Brush {
	public:
		SkinBrush(Model &target) : Brush(target) {}

		void setUp()
		{
			ulong size;
			mesh_->GetAttributeTable(NULL, &size);
			subsets_.resize(size);
			std::vector<D3DXATTRIBUTERANGE> attrinfo;
			attrinfo.resize(size);
			mesh_->GetAttributeTable(&attrinfo[0], &size);
			for(DWORD i = 0; i < size; i++) {
				subsets_[i].material_no = attrinfo[i].AttribId;
				subsets_[i].index_offset = attrinfo[i].FaceStart*3;
				subsets_[i].primitive_num = attrinfo[i].FaceCount;
				subsets_[i].vertex_offset = attrinfo[i].VertexStart;
				subsets_[i].vertex_num = attrinfo[i].VertexCount;
			}
		}

		void setUp(D3DXBONECOMBINATION *bonecb, ulong bonecb_len)
		{
			subsets_.resize(bonecb_len);
			for(DWORD i = 0; i < bonecb_len; i++) {
				subsets_[i].material_no = bonecb[i].AttribId;
				subsets_[i].index_offset = bonecb[i].FaceStart*3;
				subsets_[i].primitive_num = bonecb[i].FaceCount;
				subsets_[i].vertex_offset = bonecb[i].VertexStart;
				subsets_[i].vertex_num = bonecb[i].VertexCount;
			}
		}

		virtual const std::vector<SubsetInfo> &subsets() const { return subsets_; }

	protected:
		std::vector<SubsetInfo>			subsets_;	// 断片情報
		ID3DXMeshPtr					mesh_;
	};

	/// ソフトウェアスキンモデルクラス
	class SoftwareSkinBrush : public SkinBrush {
	public:
		SoftwareSkinBrush(Model &target) : SkinBrush(target), bone_matricies_(NULL) {}
		~SoftwareSkinBrush()
		{
			delete[] bone_matricies_;
		}

		HRslt setUp()
		{
			HRslt hr;
			dx::IDirect3DDevicePtr dev;
			hr = target_.mesh()->GetDevice(&dev);
			if(!hr) return hr;
			hr = target_.mesh()->CloneMeshFVF(D3DXMESH_MANAGED, target_.mesh()->GetFVF(), dev, &mesh_);
			if(!hr) return hr;

			SkinBrush::setUp();

			if(bone_matricies_) delete[] bone_matricies_;
			bone_matricies_ = new D3DXMATRIXA16[target_.boneNum()];
			return hr;
		}

		/// 描画設定
		virtual HRslt begin(Handle<Shader> shader, const Skeleton &skl/**< モーションがセットされたスケルトン*/) const
		{
			HRslt hr;
			if(mesh_) {
				dx::IDirect3DDevicePtr dev;
				mesh_->GetDevice(&dev);
				dev->SetVertexShader(NULL);

				uint bonenum  = target_.boneNum();
				// set up bone transforms
				for(uint i = 0; i < bonenum; i++) {
					Pointer<Skeleton::NodeType> node = skl.get(target_.bonename(i));
					if(node) D3DXMatrixMultiply(const_cast<D3DXMATRIXA16 *>(&bone_matricies_[i]), *target_.bone(i), node->val.wtm());
				}

				setWorld(MatrixC(true));

				VertexBuffer::ScopedLock src_vbl(target_.vertexbuffer());
				dx::IDirect3DVertexBufferPtr pdst_vb;
				mesh_->GetVertexBuffer(&pdst_vb);
				VertexBuffer dst_vb;
				dst_vb.setUp(pdst_vb);
				VertexBuffer::ScopedLock dst_vbl(dst_vb);
				// generate skinned mesh
				hr = target_.skin()->UpdateSkinnedMesh(&bone_matricies_[0], NULL, src_vbl.buf, dst_vbl.buf);
				if(!hr) return hr;
			}
			return hr;
		}
		/// 描画
		virtual HRslt draw(uint subset_no, const Skeleton &skl/**< モーションがセットされたスケルトン*/) const
		{
			HRslt hr;
			if(mesh_) {
				device().setMaterial(target_.mtrls[subsets_[subset_no].material_no]);
				hr = mesh_->DrawSubset(subset_no);
				if(!hr) return hr;
			}
			return hr;
		}

	protected:
		D3DXMATRIXA16					*bone_matricies_;
	};

	/// 頂点ブレンドスキンモデルクラス
	class BlendedSkinBrush : public SkinBrush {
	public:
		BlendedSkinBrush(Model &target)
			: SkinBrush(target), max_face_infl_(0), split_(0), bonecb_len_(0)
		{}

		HRslt setUp()
		{
			HRslt hr;
			mesh_ = 0; bonecb_ = 0; bonecb_len_ = 0; split_ = 0; max_face_infl_ = 0;
			
			dx::IDirect3DDevicePtr dev;
			hr = target_.mesh()->GetDevice(&dev);
			if(!hr) return hr;
			
			// Make sure we get HW caps
			hr = dev->SetSoftwareVertexProcessing(FALSE);

			dx::D3DCAPS caps;
			hr = dev->GetDeviceCaps(&caps);
			if(!hr) return hr;
			hr = target_.skin()->ConvertToBlendedMesh(
				target_.mesh(),
				D3DXMESH_MANAGED|D3DXMESHOPT_VERTEXCACHE/*これ使われて無いらしいぞ？*/,
				target_.adjacency(),
				NULL,//pAdjacencyOut
				NULL,//pFaceRemap
				NULL,//ppVertexRemap
				&max_face_infl_,//pMaxFaceInfl
				&bonecb_len_,//pNumBoneCombinations
				&bonecb_,
				&mesh_);
			if(!hr) return hr;

			if(mesh_) {
				ID3DXBufferPtr message;
				HRslt hr = D3DXValidMesh(mesh_, target_.adjacency(), &message);
				if(!hr) PRNN(hr << " : " << reinterpret_cast<char *>(message->GetBufferPointer()));
			}

			/* If the graphic can only do 2 matrix blends, ConvertToBlendedMesh cannot approximate all mesh_es to it
			   Thus we split the mesh_ in two parts: The part that uses at most 2 matrices and the rest. The first is
			   drawn using the graphic's HW vertex_ processing and the rest is drawn using SW vertex_ processing. */
			if(caps.MaxVertexBlendMatrices <= 2) {
				// calculate the index of the attribute table to split on
				D3DXBONECOMBINATION *bonecb = reinterpret_cast<D3DXBONECOMBINATION*>(bonecb_->GetBufferPointer());

				for(split_ = 0; split_ < bonecb_len_; split_++) {
					ulong infl = 0;
					for(ulong winfl = 0; winfl < max_face_infl_; winfl++) {
						if(bonecb[split_].BoneId[winfl] != UINT_MAX) infl++;
					}
					if(infl > caps.MaxVertexBlendMatrices) break;
				}

				// if there is both HW and SW, add the Software Processing flag
				if(split_ < bonecb_len_) {
					ID3DXMeshPtr temp_mesh;
					hr = mesh_->CloneMeshFVF(D3DXMESH_SOFTWAREPROCESSING|mesh_->GetOptions(), mesh_->GetFVF(), dev, &temp_mesh);
					if(!hr) return hr;
					mesh_ = temp_mesh;
				}
			}
			else split_ = bonecb_len_;

			SkinBrush::setUp(reinterpret_cast<D3DXBONECOMBINATION*>(bonecb_->GetBufferPointer()), bonecb_len_);
			return hr;
		}

		/// 描画開始
		HRslt begin(Handle<Shader> shader, const Skeleton &tree/**< モーションがセットされたスケルトン*/) const
		{
			HRslt hr;
			dx::IDirect3DDevicePtr dev;
			hr = mesh_->GetDevice(&dev);
			if(!hr) return hr;
			return dev->SetVertexShader(NULL);
		}

		/// 描画終了
		HRslt end() const
		{
			HRslt hr;
			dx::IDirect3DDevicePtr dev;
			hr = mesh_->GetDevice(&dev);
			if(!hr) return hr;
			return dev->SetRenderState(D3DRS_VERTEXBLEND, 0);
		}

		/// 描画
		HRslt draw(uint subset_no, const Skeleton &tree/**< モーションがセットされたスケルトン*/) const
		{
			HRslt hr;
			dx::IDirect3DDevicePtr dev;
			hr = mesh_->GetDevice(&dev);
			if(!hr) return hr;
			
			// Make sure we get HW caps
			hr = dev->SetSoftwareVertexProcessing(FALSE);

			dx::D3DCAPS caps;
			hr = dev->GetDeviceCaps(&caps);
			if(!hr) return hr;
			
			D3DXBONECOMBINATION *bonecb = reinterpret_cast<D3DXBONECOMBINATION*>(bonecb_->GetBufferPointer());
			device().setMaterial(target_.mtrls[bonecb[subset_no].AttribId]);
			if(subset_no < bonecb_len_) {
				ulong blend_num = 0;
				for(ulong j = 0; j < max_face_infl_; j++) {
					if(bonecb[subset_no].BoneId[j] != UINT_MAX) blend_num = j;
				}

				if(caps.MaxVertexBlendMatrices >= blend_num + 1) {
					for(ulong j = 0; j < max_face_infl_; j++) {
						ulong matid = bonecb[subset_no].BoneId[j];
						if(matid != UINT_MAX) {
							Pointer<Skeleton::NodeType> node = tree.get(target_.bonename(matid));
							if(node) {
								dev->SetTransform(D3DTS_WORLDMATRIX(j), node->val.wtm());
								dev->MultiplyTransform(D3DTS_WORLDMATRIX(j), *target_.bone(matid));
							}
						}
					}

					dev->SetRenderState(D3DRS_VERTEXBLEND, blend_num);

					hr = mesh_->DrawSubset(subset_no);
					if(!hr) return hr;
				}
			}

			// If necessary, draw parts that HW could not handle using SW
			if(subset_no >= split_ && split_ < bonecb_len_) {
				dev->SetSoftwareVertexProcessing(TRUE);
				ulong blend_num = 0;
				for(ulong j = 0; j < max_face_infl_; j++) {
					if(bonecb[subset_no].BoneId[j] != UINT_MAX) blend_num = j;
				}

				if(caps.MaxVertexBlendMatrices < blend_num + 1) {
					for(ulong j = 0; j < max_face_infl_; j++) {
						ulong matid = bonecb[subset_no].BoneId[j];
						if(matid != UINT_MAX) {
							dev->SetTransform(D3DTS_WORLDMATRIX(j), tree.get(target_.bonename(matid))->val.wtm());
							dev->MultiplyTransform(D3DTS_WORLDMATRIX(j), *target_.bone(matid));
						}
					}

					dev->SetRenderState(D3DRS_VERTEXBLEND, blend_num);

					hr = mesh_->DrawSubset(subset_no);
					if(!hr) return hr;
				}
				dev->SetSoftwareVertexProcessing(FALSE);
			}
			return hr;
		}

	protected:
		ulong					max_face_infl_;
		ulong					split_;
		ulong					bonecb_len_;
		ID3DXBufferPtr			bonecb_; // ボーンの組み合わせデータ
	};

	/// インデックス式頂点ブレンドスキンモデルクラス
	class IndexedSkinBrush : public SkinBrush {
	public:
		IndexedSkinBrush(Model &target) : SkinBrush(target), is_use_sw_(false), pal_size_(0), max_face_infl_(0), bonecb_len_(0)
		{
		}

		HRslt setUp()
		{
			HRslt hr;
			mesh_ = 0; bonecb_ = 0; bonecb_len_ = 0; pal_size_ = 0; max_face_infl_ = 0;
			
			dx::IDirect3DDevicePtr dev;
			hr = target_.mesh()->GetDevice(&dev);
			if(!hr) return hr;

			// Make sure we get HW caps
			hr = dev->SetSoftwareVertexProcessing(FALSE);

			dx::D3DCAPS caps;
			hr = dev->GetDeviceCaps(&caps);
			if(!hr) return hr;
			
			dx::IDirect3DIndexBufferPtr ib;
			hr = target_.mesh()->GetIndexBuffer(&ib);
			if(!hr) return hr;
			DWORD _max_face_infl;
			hr = target_.skin()->GetMaxFaceInfluences(ib, target_.mesh()->GetNumFaces(), &_max_face_infl);

			// 12 entry palette guarantees that any triangle (4 independent influences per vertex of a tri)
			// can be handled
			_max_face_infl = (std::min)((long)_max_face_infl, (long)12);

			DWORD flags = D3DXMESHOPT_VERTEXCACHE;
			if(caps.MaxVertexBlendMatrixIndex + 1 < _max_face_infl) {
				// HW does not support indexed vertex_ blending. Use SW instead
				pal_size_ = (std::min)((long)256, (long)target_.boneNum());
				is_use_sw_ = true;
				flags |= D3DXMESH_SYSTEMMEM;
			}
			else {
				pal_size_ = (std::min)((long)((caps.MaxVertexBlendMatrixIndex+1)/2), (long)target_.boneNum());
				is_use_sw_ = false;
				flags |= D3DXMESH_MANAGED;
			}

			hr = target_.skin()->ConvertToIndexedBlendedMesh(
				target_.mesh(),
				flags,
				pal_size_,
				target_.adjacency(),
				NULL,//pAdjacencyOut
				NULL,//pFaceRemap
				NULL,//ppVertexRemap
				&max_face_infl_,
				&bonecb_len_,
				&bonecb_,
				&mesh_);
			if(!hr) return hr;
			SkinBrush::setUp(reinterpret_cast<D3DXBONECOMBINATION*>(bonecb_->GetBufferPointer()), bonecb_len_);
			return hr;
		}

		/// 描画
		HRslt begin(Handle<Shader> shader, const Skeleton &tree/**< モーションがセットされたスケルトン*/) const
		{
			HRslt hr;
			if(mesh_) {
				dx::IDirect3DDevicePtr dev;
				hr = mesh_->GetDevice(&dev);
				if(!hr) return hr;

				if(is_use_sw_) dev->SetSoftwareVertexProcessing(TRUE);
				else dev->SetSoftwareVertexProcessing(FALSE);
				if(max_face_infl_ == 1) dev->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_0WEIGHTS);
				else dev->SetRenderState(D3DRS_VERTEXBLEND, max_face_infl_-1);
				if(max_face_infl_) dev->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE);
				dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
				dev->SetVertexShader(NULL);
			}
			return hr;
		}

		HRslt end() const
		{
			HRslt hr;
			if(mesh_) {
				dx::IDirect3DDevicePtr dev;
				hr = mesh_->GetDevice(&dev);
				if(!hr) return hr;

				dev->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
				dev->SetRenderState(D3DRS_VERTEXBLEND, 0);
				if(is_use_sw_) dev->SetSoftwareVertexProcessing(FALSE);
			}
			return hr;
		}
		/// 描画
		HRslt draw(uint subset_no, const Skeleton &tree/**< モーションがセットされたスケルトン*/) const
		{
			if(mesh_) {
				HRslt hr;
				dx::IDirect3DDevicePtr dev;
				hr = mesh_->GetDevice(&dev);
				if(!hr) return hr;

				D3DXBONECOMBINATION *bonecb = reinterpret_cast<D3DXBONECOMBINATION*>(bonecb_->GetBufferPointer());
				for(uint j = 0; j < pal_size_; j++) {
					uint matid = bonecb[subset_no].BoneId[j];
					if(matid != UINT_MAX) {
						Pointer<Skeleton::NodeType> node = tree.get(target_.bonename(matid));
						if(node) {
							dev->SetTransform(D3DTS_WORLDMATRIX(j), node->val.wtm());
							dev->MultiplyTransform(D3DTS_WORLDMATRIX(j), *target_.bone(matid));
						}
					}
				}
				device().setMaterial(target_.mtrls[bonecb[subset_no].AttribId]);
				hr = mesh_->DrawSubset(subset_no);
				if(!hr) PRNN(hr);
				return hr;
			}
			return E_POINTER;
		}

	protected:
		bool					is_use_sw_;
		ulong					pal_size_;
		ulong					max_face_infl_;
		ulong					bonecb_len_;
		ID3DXBufferPtr			bonecb_; // ボーンの組み合わせデータ
	};

	class SkinningVertexShader : public Rsrc
	{
	public:
		HRslt setUp()
		{
			HRslt hr;

			// Make sure we get HW caps
			hr = device().impl()->SetSoftwareVertexProcessing(FALSE);

			dx::D3DCAPS caps;
			hr = device().impl()->GetDeviceCaps(&caps);
			if(!hr) return hr;

			static const _TCHAR *fnames[] = {_T("skinmesh1.vsh"), _T("skinmesh2.vsh"), _T("skinmesh3.vsh"), _T("skinmesh4.vsh")};

			ulong flags = D3DUSAGE_SOFTWAREPROCESSING;
			if(caps.VertexShaderVersion >= D3DVS_VERSION(1, 1)) flags = 0;

			for(ulong i = 0; i < 4; i++) {
				ID3DXBufferPtr code;
				// Assemble the vertex shader file
				hr = D3DXAssembleShaderFromFile(fnames[i], NULL, NULL, 0, &code, NULL);
				if(!hr) {
					hr = D3DXAssembleShaderFromResource(NULL, fnames[i], NULL, NULL, 0, &code, NULL);
					if(!hr) {
						hr = D3DXAssembleShader(asm_shaders[i], asm_shader_sizes[i], NULL, NULL, 0, &code, NULL);
						if(!hr) {
							GCTP_TRACE(hr);
							return hr;
						}
					}
				}
				// Create the vertex shader
				hr = device().impl()->CreateVertexShader(reinterpret_cast<DWORD*>(code->GetBufferPointer()), &shader_[i]);
				if(!hr) return hr;
			}

			return hr;
		}

		static const char asm_shaders_1[];
		static const char asm_shaders_2[];
		static const char asm_shaders_3[];
		static const char asm_shaders_4[];
		static const char *asm_shaders[];
		static const int asm_shader_sizes[];

		dx::IDirect3DVertexShaderPtr shader_[4];
	};

	/// 頂点シェーダー式スキンモデルクラス
	class VertexShaderSkinBrush : public SkinBrush
	{
	public:
		enum { MAX_MATRICIES = 28 };

		VertexShaderSkinBrush(Model &target)
			: SkinBrush(target), is_use_sw_(false), pal_size_(0), max_face_infl_(0), bonecb_len_(0)
		{
		}

		HRslt setUp()
		{
			if(!vs_) {
				vs_ = new SkinningVertexShader;
				if(vs_) vs_->setUp();
			}

			HRslt hr;
			mesh_ = 0; bonecb_ = 0; bonecb_len_ = 0; pal_size_ = 0; max_face_infl_ = 0;
			
			dx::IDirect3DDevicePtr dev;
			hr = target_.mesh()->GetDevice(&dev);
			if(!hr) return hr;

			// Make sure we get HW caps
			hr = dev->SetSoftwareVertexProcessing(FALSE);

			dx::D3DCAPS caps;
			hr = dev->GetDeviceCaps(&caps);
			if(!hr) return hr;
			
			DWORD flags = D3DXMESHOPT_VERTEXCACHE;
			if(caps.VertexShaderVersion >= D3DVS_VERSION(1, 1)) {
				is_use_sw_ = false;
				flags |= D3DXMESH_MANAGED;
			}
			else {
				is_use_sw_ = true;
				flags |= D3DXMESH_SYSTEMMEM;
			}

			// 実際のモードで再測定
			if(is_use_sw_) {
				hr = dev->SetSoftwareVertexProcessing(TRUE);
				if(!hr) return hr;
				hr = dev->GetDeviceCaps(&caps);
				if(!hr) return hr;
			}
			// 残りの定数領域をすべてボーンマトリクスに割り当てる
//			max_matricies_ = (caps.MaxVertexShaderConst-9)/3;
//			if(max_matricies_ >= caps.MaxVertexShaderConst) return E_FAIL; // あんま意味ないみたい
			
			// Get palette size
			pal_size_ = (std::min)((ulong)MAX_MATRICIES, (ulong)target_.boneNum());
//			PRNN("bone num : "<<target_.boneNum());

			hr = target_.skin()->ConvertToIndexedBlendedMesh(
				target_.mesh(),
				flags,
				pal_size_,
				target_.adjacency(),
				NULL,//pAdjacencyOut
				NULL,//pFaceRemap
				NULL,//ppVertexRemap
				&max_face_infl_,
				&bonecb_len_,
				&bonecb_,
				&mesh_);
			if(!hr) return hr;

			if((mesh_->GetFVF() & D3DFVF_POSITION_MASK) != D3DFVF_XYZ) max_face_infl_ = ((mesh_->GetFVF() & D3DFVF_POSITION_MASK) - D3DFVF_XYZRHW) / 2;
			else max_face_infl_ = 1;

			// FVF has to match our declarator. Vertex shaders are not as forgiving as FF pipeline
			DWORD new_fvf = (mesh_->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;
			if(new_fvf != mesh_->GetFVF()) {
				ID3DXMeshPtr temp_mesh;
				hr = mesh_->CloneMeshFVF(mesh_->GetOptions(), new_fvf, dev, &temp_mesh);
				if(hr) mesh_ = temp_mesh;
			}

			dx::D3DVERTEXELEMENT pDecl[MAX_FVF_DECL_SIZE];
			dx::D3DVERTEXELEMENT *pDeclCur;
			hr = mesh_->GetDeclaration(pDecl);
			if(!hr) return hr;

			// the vertex shader is expecting to interpret the UBYTE4 as a D3DCOLOR, so update the type 
			//   NOTE: this cannot be done with CloneMesh, that would convert the UBYTE4 data to float and then to D3DCOLOR
			//          this is more of a "cast" operation
			pDeclCur = pDecl;
			while(pDeclCur->Stream != 0xff) {
				if((pDeclCur->Usage == D3DDECLUSAGE_BLENDINDICES) && (pDeclCur->UsageIndex == 0))
					pDeclCur->Type = D3DDECLTYPE_D3DCOLOR;
				pDeclCur++;
			}

			hr = mesh_->UpdateSemantics(pDecl);
			if(!hr) return hr;
			
			SkinBrush::setUp(reinterpret_cast<D3DXBONECOMBINATION*>(bonecb_->GetBufferPointer()), bonecb_len_);
			return hr;
		}

		/// 描画
		HRslt begin(Handle<Shader> shader, const Skeleton &tree/**< モーションがセットされたスケルトン*/) const
		{
			if(mesh_ && vs_) {
				HRslt hr;
				dx::IDirect3DDevicePtr dev;
				hr = mesh_->GetDevice(&dev);
				if(!hr) return hr;

				if(is_use_sw_) dev->SetSoftwareVertexProcessing(TRUE);

				hr = dev->SetVertexShader(vs_->shader_[max_face_infl_-1]);
				if(!hr) return hr;

				// Set Light for vertex shader
				// ライトスタック上の、最初のディレクショナルライトを採用
				for(uint i = 0; i < lightNum(); i++) {
					DirectionalLight light;
					if(getLight(i, light)) {
						device().impl()->SetVertexShaderConstantF(1, Vector4C(-light.dir, 0), 1);
						break;
					}
				}

				{
					D3DXMATRIXA16 transposed_proj_mat;
					dev->GetTransform(D3DTS_PROJECTION, &transposed_proj_mat);
					D3DXMatrixTranspose(&transposed_proj_mat, &transposed_proj_mat);
					dev->SetVertexShaderConstantF(2, transposed_proj_mat, 4);
				}
				return hr;
			}
			return E_POINTER;
		}

		/// 描画
		HRslt end() const
		{
			if(mesh_) {
				HRslt hr;
				if(is_use_sw_) {
					dx::IDirect3DDevicePtr dev;
					hr = mesh_->GetDevice(&dev);
					if(hr) hr = dev->SetSoftwareVertexProcessing(FALSE);
				}
				return hr;
			}
			return E_POINTER;
		}

		/// 描画
		HRslt draw(uint subset_no, const Skeleton &tree/**< モーションがセットされたスケルトン*/) const
		{
			if(mesh_) {
				HRslt hr;
				dx::IDirect3DDevicePtr dev;
				hr = mesh_->GetDevice(&dev);
				if(!hr) return hr;
				// Use COLOR instead of UBYTE4 since Geforce3 does not support it
				// _const.w should be 3, but due to about hack, mul by 255 and add epsilon
				Vector4 _const = { 1.0f, 0.0f, 0.0f, 765.01f };

				D3DXMATRIXA16 view_mat;
				dev->GetTransform(D3DTS_VIEW, &view_mat);
				D3DXBONECOMBINATION *bonecb = reinterpret_cast<D3DXBONECOMBINATION*>(bonecb_->GetBufferPointer());

				for(ulong j = 0; j < pal_size_; j++) {
					DWORD matid = bonecb[subset_no].BoneId[j];
					if(matid != UINT_MAX) {
						Pointer<Skeleton::NodeType> node = tree.get(target_.bonename(matid));
						if(node) {
							D3DXMATRIXA16 mat;
							D3DXMatrixMultiply(&mat, *target_.bone(matid), node->val.wtm());
							D3DXMatrixMultiplyTranspose(&mat, &mat, &view_mat);
							dev->SetVertexShaderConstantF(j*3 + 9, mat, 3);
						}
					}
				}

				// Sum of all ambient and emissive contribution
				Color amb_emm = target_.mtrls[bonecb[subset_no].AttribId].ambient*getAmbient()+target_.mtrls[bonecb[subset_no].AttribId].emissive;
				dev->SetVertexShaderConstantF(7, amb_emm, 1);
				dev->SetVertexShaderConstantF(8, target_.mtrls[bonecb[subset_no].AttribId].diffuse, 1);
				_const.y = target_.mtrls[bonecb[subset_no].AttribId].power;
				dev->SetVertexShaderConstantF(0, _const, 1);

				device().setMaterial(target_.mtrls[bonecb[subset_no].AttribId]);
				hr = mesh_->DrawSubset(subset_no);
				return hr;
			}
			return E_POINTER;
		}

	protected:
		bool					is_use_sw_;
		ulong					pal_size_;
		ulong					max_face_infl_;
		ulong					bonecb_len_;
		ID3DXBufferPtr			bonecb_; // ボーンの組み合わせデータ

		static Pointer<SkinningVertexShader> vs_;
	};
	
	/// HLSL式スキンモデルクラス
	class ShaderSkinBrush : public SkinBrush
	{
	public:
		enum { MAX_MATRICIES = 26 }; // 実際はもっと使えるんだけど…
		// asm型に比べて不便かも

		ShaderSkinBrush(Model &target)
			: SkinBrush(target), is_use_sw_(false), pal_size_(0), max_face_infl_(0), bonecb_len_(0)
		{
		}

		HRslt setUp()
		{
			HRslt hr;
			mesh_ = 0; bonecb_ = 0; bonecb_len_ = 0; pal_size_ = 0; max_face_infl_ = 0;
			
			dx::IDirect3DDevicePtr dev;
			hr = target_.mesh()->GetDevice(&dev);
			if(!hr) return hr;

			// Make sure we get HW caps
			hr = dev->SetSoftwareVertexProcessing(FALSE);

			dx::D3DCAPS caps;
			hr = dev->GetDeviceCaps(&caps);
			if(!hr) return hr;

			DWORD flags = D3DXMESHOPT_VERTEXCACHE;
			if(caps.VertexShaderVersion >= D3DVS_VERSION(1, 1)) {
				is_use_sw_ = false;
				flags |= D3DXMESH_MANAGED;
			}
			else {
				is_use_sw_ = true;
				flags |= D3DXMESH_SYSTEMMEM;
			}
			
			// Get palette size
			pal_size_ = (std::min)((long)MAX_MATRICIES, (long)target_.boneNum());
//			PRNN("bone num : "<<target_.boneNum());

			hr = target_.skin()->ConvertToIndexedBlendedMesh(
				target_.mesh(),
				flags,
				pal_size_,
				target_.adjacency(),
				NULL,//pAdjacencyOut
				NULL,//pFaceRemap
				NULL,//ppVertexRemap
				&max_face_infl_,
				&bonecb_len_,
				&bonecb_,
				&mesh_);
			if(!hr) return hr;

			if((mesh_->GetFVF() & D3DFVF_POSITION_MASK) != D3DFVF_XYZ) max_face_infl_ = ((mesh_->GetFVF() & D3DFVF_POSITION_MASK) - D3DFVF_XYZRHW) / 2;
			else max_face_infl_ = 1;

			// FVF has to match our declarator. Vertex shaders are not as forgiving as FF pipeline
			DWORD new_fvf = (mesh_->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;
			if(new_fvf != mesh_->GetFVF()) {
				ID3DXMeshPtr temp_mesh;
				hr = mesh_->CloneMeshFVF(mesh_->GetOptions(), new_fvf, dev, &temp_mesh);
				if(hr) mesh_ = temp_mesh;
			}
			
			dx::D3DVERTEXELEMENT pDecl[MAX_FVF_DECL_SIZE];
			dx::D3DVERTEXELEMENT *pDeclCur;
			hr = mesh_->GetDeclaration(pDecl);
			if(!hr) return hr;

			// the vertex shader is expecting to interpret the UBYTE4 as a D3DCOLOR, so update the type 
			//   NOTE: this cannot be done with CloneMesh, that would convert the UBYTE4 data to float and then to D3DCOLOR
			//          this is more of a "cast" operation
			pDeclCur = pDecl;
			while(pDeclCur->Stream != 0xff) {
				if((pDeclCur->Usage == D3DDECLUSAGE_BLENDINDICES) && (pDeclCur->UsageIndex == 0))
					pDeclCur->Type = D3DDECLTYPE_D3DCOLOR;
				pDeclCur++;
			}

			hr = mesh_->UpdateSemantics(pDecl);
			if(!hr) return hr;

			bone_matricies_.resize(pal_size_);

			Handle<dx::HLSLShader> shader;
			for(ulong i = 0; i < bonecb_len_; i++) {
				D3DXBONECOMBINATION *bonecb = reinterpret_cast<D3DXBONECOMBINATION*>(bonecb_->GetBufferPointer());
				if(!target_.mtrls[bonecb[i].AttribId].shader) {
					shader = context()[_T("skinned.fx")];
					if(!shader) {
						shader = context()[_T("HLSL_SKINNEDMESH")];
						if(!shader) {
							Pointer<dx::HLSLShader> skinshader = new dx::HLSLShader;
							skinshader->setUp(skinned_fx, skinned_fx_size);
							context().insert(skinshader, _T("HLSL_SKINNEDMESH"));
							shader = skinshader;
						}
					}
					target_.mtrls[bonecb[i].AttribId].shader = shader;
				}
				/*for(ulong j = 0; j < pal_size_; j++) {
					DWORD matid = bonecb[i].BoneId[j];
					if(matid != UINT_MAX) {
						const char *name = target_.bonename(matid);
						if(name) PRNN("bonename "<<i<<","<<bonecb[i].AttribId<<","<<j<<","<<matid<<" "<<name);
						else PRNN("bonename "<<i<<","<<bonecb[i].AttribId<<","<<j<<","<<matid<<" ???");
					}
				}*/
			}
			SkinBrush::setUp(reinterpret_cast<D3DXBONECOMBINATION*>(bonecb_->GetBufferPointer()), bonecb_len_);
			return hr;
		}

		/// 描画
		HRslt begin(Handle<Shader> _shader, const Skeleton &tree/**< モーションがセットされたスケルトン*/) const
		{
			Handle<dx::HLSLShader> shader = _shader;
			if(mesh_ && shader) {
				HRslt hr;
				dx::IDirect3DDevicePtr dev;
				hr = mesh_->GetDevice(&dev);
				if(!hr) return hr;

				if(is_use_sw_) {
					// If hw or pure hw vertex processing is forced, we can't render the
					// mesh, so just exit out.  Typical applications should create
					// a device with appropriate vertex processing capability for this
					// skinning method.
	                //if( g_dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING ) return;
					dev->SetSoftwareVertexProcessing(TRUE);
				}

				// Set Light for vertex shader
				// ライトスタック上の、最初のディレクショナルライトを採用
				for(uint i = 0; i < lightNum(); i++) {
					DirectionalLight light;
					if(getLight(i, light)) {
						hr = (*shader)->SetVector("lhtDir", Vector4C(-light.dir, 0));
						if(!hr) GCTP_TRACE(hr);
						hr = (*shader)->SetVector("lightDiffuse", (D3DXVECTOR4*)&light.diffuse);
						if(!hr) GCTP_TRACE(hr);
						break;
					}
				}

				{
					D3DXMATRIXA16 mat;
					D3DXMATRIXA16 view_proj_mat;
					dev->GetTransform(D3DTS_VIEW, &view_proj_mat);
					dev->GetTransform(D3DTS_PROJECTION, &mat);
					D3DXMatrixMultiply(&view_proj_mat, &view_proj_mat, &mat);
					hr = (*shader)->SetMatrix("mViewProj", &view_proj_mat);
					if(!hr) GCTP_TRACE(hr);
				}
				return hr;
			}
			return E_POINTER;
		}

		/// 描画
		HRslt end() const
		{
			if(mesh_) {
				HRslt hr;
				if(is_use_sw_) {
					dx::IDirect3DDevicePtr dev;
					hr = mesh_->GetDevice(&dev);
					if(hr) hr = dev->SetSoftwareVertexProcessing(FALSE);
				}
				return hr;
			}
			return E_POINTER;
		}

		/// 描画
		HRslt draw(uint subset_no, const Skeleton &tree/**< モーションがセットされたスケルトン*/) const
		{
			HRslt hr;
			D3DXBONECOMBINATION *bonecb = reinterpret_cast<D3DXBONECOMBINATION*>(bonecb_->GetBufferPointer());
			Handle<dx::HLSLShader> shader = target_.mtrls[bonecb[subset_no].AttribId].shader;
			if(mesh_ && shader) {
				dx::IDirect3DDevicePtr dev;
				hr = mesh_->GetDevice(&dev);
				if(!hr) return hr;

				D3DXMATRIX *bm_buf = const_cast< D3DXMATRIX * >(&bone_matricies_[0]);
				for(ulong j = 0; j < pal_size_; j++)
				{
					DWORD matid = bonecb[subset_no].BoneId[j];
					if(matid != UINT_MAX) {
						Pointer<Skeleton::NodeType> node = tree.get(target_.bonename(matid));
						if(node) D3DXMatrixMultiply(&bm_buf[j], *target_.bone(matid), node->val.wtm());
					}
				}
				hr = (*shader)->SetMatrixArray("mWorldMatrixArray", &bone_matricies_[0], pal_size_);
				if(!hr) {
					PRNN(pal_size_);
					GCTP_TRACE(hr);
				}

				// Sum of all ambient and emissive contribution
				Color amb_emm = target_.mtrls[bonecb[subset_no].AttribId].ambient*getAmbient()+target_.mtrls[bonecb[subset_no].AttribId].emissive;
				hr = (*shader)->SetVector("MaterialAmbient", (D3DXVECTOR4*)&amb_emm);
				if(!hr) GCTP_TRACE(hr);
				// set material color properties 
				hr = (*shader)->SetVector("MaterialDiffuse", (D3DXVECTOR4*)&target_.mtrls[bonecb[subset_no].AttribId].diffuse);
				if(!hr) GCTP_TRACE(hr);

				hr = (*shader)->SetInt( "CurNumBones", max_face_infl_-1);
				if(!hr) GCTP_TRACE(hr);

				(*shader)->CommitChanges();
				device().setMaterial(target_.mtrls[bonecb[subset_no].AttribId]);
				hr = mesh_->DrawSubset(subset_no);
				if(!hr) {
					GCTP_TRACE(hr);
					PRNN("subset_no = "<<subset_no<<";passnum = "<<shader->passnum());
				}
			}
			return hr;
		}

	protected:
		bool						is_use_sw_;
		ulong						pal_size_;
		ulong						max_face_infl_;
		ulong						bonecb_len_;
		ID3DXBufferPtr				bonecb_; // ボーンの組み合わせデータ
		std::vector<D3DXMATRIX>		bone_matricies_;

	private:
		static const char skinned_fx[];
		static const int skinned_fx_size;
	};

}}} // namespace gctp::graphic::dx
#endif // _GCTP_GRAPHIC_DX_SKINBRUSH_HPP_