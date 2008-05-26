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
#include <gctp/graphic/shader.hpp>

namespace gctp { namespace graphic {

	/// ソフトウェアスキンモデルクラス
	class SoftwareSkinBrush : public Brush {
	public:
		SoftwareSkinBrush(Model &target) : Brush(target), bone_matricies_(NULL) {}
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

			ulong attr_num;
			hr = mesh_->GetAttributeTable(NULL, &attr_num);
			if(!hr) return hr;
			attr_tbl_.resize(attr_num);
			hr = mesh_->GetAttributeTable(&attr_tbl_[0], NULL);
			if(!hr) return hr;

			if(bone_matricies_) delete[] bone_matricies_;
			bone_matricies_ = new D3DXMATRIXA16[target_.boneNum()];
			return hr;
		}

		/// 描画
		HRslt draw(const Skeleton &tree/**< モーションがセットされたスケルトン*/) const
		{
			return drawLow(tree, -1);
		}
		/// 描画
		HRslt draw(const Skeleton &tree/**< モーションがセットされたスケルトン*/, int mtrlno) const
		{
			return drawLow(tree, mtrlno);
		}

	protected:
		/// 描画
		HRslt drawLow(const Skeleton &tree/**< モーションがセットされたスケルトン*/, int mtrlno) const
		{
			HRslt hr;
			if(mesh_) {
				dx::IDirect3DDevicePtr dev;
				mesh_->GetDevice(&dev);

				dev->SetVertexShader(NULL);
				uint bonenum  = target_.boneNum();
				// set up bone transforms
				for(uint i = 0; i < bonenum; i++) {
					Pointer<Skeleton::NodeType> node = tree.get(target_.bonename(i));
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

				for(uint i = 0; i < attr_tbl_.size(); i++) {
					if(mtrlno != -1 && attr_tbl_[i].AttribId != static_cast<uint>(mtrlno)) break;
					device().setMaterial(target_.mtrls[attr_tbl_[i].AttribId]);
					
					Pointer<Shader> shader = target_.mtrls[attr_tbl_[i].AttribId].shader.get();
					if(shader && (hr = shader->begin())) {
						for(uint ipass = 0; ipass < shader->passnum(); ipass++) {
							hr = shader->beginPass( ipass );
							if(!hr) GCTP_TRACE(hr);
							hr = mesh_->DrawSubset(attr_tbl_[i].AttribId);
							if(!hr) GCTP_TRACE(hr);
							hr = shader->endPass();
							if(!hr) GCTP_TRACE(hr);
						}
						hr = shader->end();
						if(!hr) GCTP_TRACE(hr);
					}
					else hr = mesh_->DrawSubset(attr_tbl_[i].AttribId);

					if(!hr) return hr;
				}
			}
			return hr;
		}

		std::vector<D3DXATTRIBUTERANGE>	attr_tbl_;
		D3DXMATRIXA16					*bone_matricies_;
		ID3DXMeshPtr					mesh_;
	};

	/// 頂点ブレンドスキンモデルクラス
	class BlendedSkinBrush : public Brush {
	public:
		BlendedSkinBrush(Model &target)
			: Brush(target), max_face_infl_(0), attr_split_(0), attr_num_(0)
		{}

		HRslt setUp()
		{
			HRslt hr;
			mesh_ = 0; bonecb_ = 0; attr_split_ = 0; attr_num_ = 0; max_face_infl_ = 0;
			
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
				&attr_num_,//pNumBoneCombinations
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

				for(attr_split_ = 0; attr_split_ < attr_num_; attr_split_++) {
					ulong infl = 0;
					for(ulong winfl = 0; winfl < max_face_infl_; winfl++) {
						if(bonecb[attr_split_].BoneId[winfl] != UINT_MAX) infl++;
					}
					if(infl > caps.MaxVertexBlendMatrices) break;
				}

				// if there is both HW and SW, add the Software Processing flag
				if(attr_split_ < attr_num_) {
					ID3DXMeshPtr temp_mesh;
					hr = mesh_->CloneMeshFVF(D3DXMESH_SOFTWAREPROCESSING|mesh_->GetOptions(), mesh_->GetFVF(), dev, &temp_mesh);
					if(!hr) return hr;
					mesh_ = temp_mesh;
				}
			}
			else attr_split_ = attr_num_;
			return hr;
		}

		/// 描画
		HRslt draw(const Skeleton &tree/**< モーションがセットされたスケルトン*/) const
		{
			return drawLow(tree, -1);
		}
		/// 描画
		HRslt draw(const Skeleton &tree/**< モーションがセットされたスケルトン*/, int mtrlno) const
		{
			return drawLow(tree, mtrlno);
		}

	protected:
		/// 描画
		HRslt drawLow(const Skeleton &tree/**< モーションがセットされたスケルトン*/, int mtrlno) const
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
			
			dev->SetVertexShader(NULL);
			uint bonenum  = target_.boneNum();
			ulong attr_id_prev = UNUSED32;
			D3DXBONECOMBINATION *bonecb = reinterpret_cast<D3DXBONECOMBINATION*>(bonecb_->GetBufferPointer());
			// Draw using default vtx processing of the graphic (typically HW)
			for(ulong i = 0; i < attr_num_; i++) {
				if(mtrlno != -1 && bonecb[i].AttribId != static_cast<uint>(mtrlno)) break;
				ulong blend_num = 0;
				for(ulong j = 0; j < max_face_infl_; j++) {
					if(bonecb[i].BoneId[j] != UINT_MAX) blend_num = j;
				}

				if(caps.MaxVertexBlendMatrices >= blend_num + 1) {
					for(ulong j = 0; j < max_face_infl_; j++) {
						ulong matid = bonecb[i].BoneId[j];
						if(matid != UINT_MAX) {
							Pointer<Skeleton::NodeType> node = tree.get(target_.bonename(matid));
							if(node) {
								dev->SetTransform(D3DTS_WORLDMATRIX(j), node->val.wtm());
								dev->MultiplyTransform(D3DTS_WORLDMATRIX(j), *target_.bone(matid));
							}
						}
					}

					dev->SetRenderState(D3DRS_VERTEXBLEND, blend_num);

					if((attr_id_prev != bonecb[i].AttribId) || (attr_id_prev == UNUSED32)) {
						device().setMaterial(target_.mtrls[bonecb[i].AttribId]);
						attr_id_prev  = bonecb[i].AttribId;
					}

					hr = mesh_->DrawSubset(i);
					if(!hr) return hr;
				}
			}

			// If necessary, draw parts that HW could not handle using SW
			if(attr_split_ < attr_num_) {
				attr_id_prev = UNUSED32;
				dev->SetSoftwareVertexProcessing(TRUE);
				for(ulong i = attr_split_; i < attr_num_; i++) {
					if(mtrlno != -1 && bonecb[i].AttribId != static_cast<uint>(mtrlno)) break;
					ulong blend_num = 0;
					for(ulong j = 0; j < max_face_infl_; j++) {
						if(bonecb[i].BoneId[j] != UINT_MAX) blend_num = j;
					}

					if(caps.MaxVertexBlendMatrices < blend_num + 1) {
						for(ulong j = 0; j < max_face_infl_; j++) {
							ulong matid = bonecb[i].BoneId[j];
							if(matid != UINT_MAX) {
								dev->SetTransform(D3DTS_WORLDMATRIX(j), tree.get(target_.bonename(matid))->val.wtm());
								dev->MultiplyTransform(D3DTS_WORLDMATRIX(j), *target_.bone(matid));
							}
						}

						dev->SetRenderState(D3DRS_VERTEXBLEND, blend_num);

						if((attr_id_prev != bonecb[i].AttribId) || (attr_id_prev == UNUSED32)) {
							device().setMaterial(target_.mtrls[bonecb[i].AttribId]);
							attr_id_prev = bonecb[i].AttribId;
						}

						hr = mesh_->DrawSubset(i);
						if(!hr) return hr;
					}
				}
				dev->SetSoftwareVertexProcessing(FALSE);
			}
			dev->SetRenderState(D3DRS_VERTEXBLEND, 0);
			return hr;
		}

		ulong					max_face_infl_;
		ulong					attr_split_;
		ulong					attr_num_;
		ID3DXBufferPtr			bonecb_; // ボーンの組み合わせデータ
		ID3DXMeshPtr			mesh_;
	};

	/// インデックス式頂点ブレンドスキンモデルクラス
	class IndexedSkinBrush : public Brush {
	public:
		IndexedSkinBrush(Model &target) : Brush(target), is_use_sw_(false), pal_size_(0), max_face_infl_(0), attr_num_(0)
		{
		}

		HRslt setUp()
		{
			HRslt hr;
			mesh_ = 0; bonecb_ = 0; pal_size_ = 0; attr_num_ = 0; max_face_infl_ = 0;
			
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
				&attr_num_,
				&bonecb_,
				&mesh_);
			if(!hr) return hr;
			return hr;
		}

		/// 描画
		HRslt draw(const Skeleton &tree/**< モーションがセットされたスケルトン*/) const
		{
			return drawLow(tree, -1);
		}
		/// 描画
		HRslt draw(const Skeleton &tree/**< モーションがセットされたスケルトン*/, int mtrlno) const
		{
			return drawLow(tree, mtrlno);
		}

	protected:
		/// 描画
		HRslt drawLow(const Skeleton &tree/**< モーションがセットされたスケルトン*/, int mtrlno) const
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
				D3DXBONECOMBINATION *bonecb = reinterpret_cast<D3DXBONECOMBINATION*>(bonecb_->GetBufferPointer());
				dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
				dev->SetVertexShader(NULL);
				for(uint i = 0; i < attr_num_; i++) {
					if(mtrlno != -1 && bonecb[i].AttribId != static_cast<uint>(mtrlno)) break;
					for(uint j = 0; j < pal_size_; j++) {
						uint matid = bonecb[i].BoneId[j];
						if(matid != UINT_MAX) {
							Pointer<Skeleton::NodeType> node = tree.get(target_.bonename(matid));
							if(node) {
								dev->SetTransform(D3DTS_WORLDMATRIX(j), node->val.wtm());
								dev->MultiplyTransform(D3DTS_WORLDMATRIX(j), *target_.bone(matid));
							}
						}
					}
					device().setMaterial(target_.mtrls[bonecb[i].AttribId]);
					hr = mesh_->DrawSubset(i);
					if(!hr) PRNN(hr);
				}
				dev->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
				dev->SetRenderState(D3DRS_VERTEXBLEND, 0);
				if(is_use_sw_) dev->SetSoftwareVertexProcessing(FALSE);
			}
			return hr;
		}

		bool					is_use_sw_;
		ulong					pal_size_;
		ulong					max_face_infl_;
		ulong					attr_num_;
		ID3DXBufferPtr			bonecb_; // ボーンの組み合わせデータ
		ID3DXMeshPtr			mesh_;
	};

	namespace {

		// Infl 1
		const char asm_shaders_1[] =
			"vs.1.1\n"
			";------------------------------------------------------------------------------\n"
			"; v0 = position\n"
			"; v1 = blend weights\n"
			"; v2 = blend indices\n"
			"; v3 = normal\n"
			"; v4 = texture coordinates\n"
			";------------------------------------------------------------------------------\n"
			";------------------------------------------------------------------------------\n"
			"; r0.w = Last blend weight\n"
			"; r1 = Blend indices\n"
			"; r2 = Temp position\n"
			"; r3 = Temp normal\n"
			"; r4 = Blended position in camera space\n"
			"; r5 = Blended normal in camera space\n"
			";------------------------------------------------------------------------------\n"
			";------------------------------------------------------------------------------\n"
			"; Constants specified by the app;\n"
			";\n"
			"; c9-c95 = world-view matrix palette\n"
			"; c8	  = diffuse * light.diffuse\n"
			"; c7	  = ambient color\n"
			"; c2-c5   = projection matrix\n"
			"; c1	  = light direction\n"
			"; c0	  = {1, power, 0, 1020.01};\n"
			";------------------------------------------------------------------------------\n"
			";------------------------------------------------------------------------------\n"
			"; oPos	  = Output position\n"
			"; oD0	  = Diffuse\n"
			"; oD1	  = Specular\n"
			"; oT0	  = texture coordinates\n"
			";------------------------------------------------------------------------------\n"
			"dcl_position v0;\n"
			"dcl_blendweight v1;\n"
			"dcl_blendindices v2;\n"
			"dcl_normal v3;\n"
			"dcl_texcoord0 v4;\n"
			"// Compensate for lack of UBYTE4 on Geforce3\n"
			"mul r1,v2.zyxw,c0.wwww\n"
			"//mul r1,v2,c0.wwww\n"
			"//Set 1\n"
			"mov a0.x,r1.x\n"
			"m4x3 r4.xyz,v0,c[a0.x + 9];\n"
			"m3x3 r5.xyz,v3,c[a0.x + 9]; \n"
			"//compute position\n"
			"mov r4.w,c0.x\n"
			"m4x4 oPos,r4,c2\n"
			"// normalize normals\n"
			"dp3 r5.w, r5, r5;\n"
			"rsq r5.w, r5.w;\n"
			"mul r5, r5, r5.w;\n"
			"; Do the lighting calculation\n"
			"dp3 r1.x, r5, c1      ; normal dot light\n"
			"lit r1, r1\n"
			"mul r0, r1.y, c8      ; Multiply with diffuse\n"
			"add r0, r0, c7        ; Add in ambient\n"
			"min oD0, r0, c0.x     ; clamp if > 1\n"
			"mov oD1, c0.zzzz      ; output specular\n"
			"; Copy texture coordinate\n"
			"mov oT0, v4\n";
		
		// Infl 2
		const char asm_shaders_2[] =
			"vs.1.1\n"
			"dcl_position v0;\n"
			"dcl_blendweight v1;\n"
			"dcl_blendindices v2;\n"
			"dcl_normal v3;\n"
			"dcl_texcoord0 v4;\n"
			"// Compensate for lack of UBYTE4 on Geforce3\n"
			"mul r1,v2.zyxw,c0.wwww\n"
			"//mul r1,v2,c0.wwww\n"
			"//first compute the last blending weight\n"
			"dp3 r0.w,v1.xyz,c0.xzz; \n"
			"add r0.w,-r0.w,c0.x\n"
			"//Set 1\n"
			"mov a0.x,r1.x\n"
			"m4x3 r4.xyz,v0,c[a0.x + 9];\n"
			"m3x3 r5.xyz,v3,c[a0.x + 9];\n"
			"//blend them\n"
			"mul r4.xyz,r4.xyz,v1.xxxx\n"
			"mul r5.xyz,r5.xyz,v1.xxxx\n"
			"//Set 2\n"
			"mov a0.x,r1.y\n"
			"m4x3 r2.xyz,v0,c[a0.x + 9];\n"
			"m3x3 r3.xyz,v3,c[a0.x + 9];\n"
			"//add them in\n"
			"mad r4.xyz,r2.xyz,r0.wwww,r4;\n"
			"mad r5.xyz,r3.xyz,r0.wwww,r5;\n"
			"//compute position\n"
			"mov r4.w,c0.x\n"
			"m4x4 oPos,r4,c2\n"
			"// normalize normals\n"
			"dp3 r5.w, r5, r5;\n"
			"rsq r5.w, r5.w;\n"
			"mul r5, r5, r5.w;\n"
			"; Do the lighting calculation\n"
			"dp3 r1.x, r5, c1      ; normal dot light\n"
			"lit r1, r1\n"
			"mul r0, r1.y, c8      ; Multiply with diffuse\n"
			"add r0, r0, c7        ; Add in ambient\n"
			"min oD0, r0, c0.x     ; clamp if > 1\n"
			"mov oD1, c0.zzzz      ; output specular\n"
			"; Copy texture coordinate\n"
			"mov oT0, v4\n";

		// Infl 3
		const char asm_shaders_3[] =
			"vs.1.1\n"
			"dcl_position v0;\n"
			"dcl_blendweight v1;\n"
			"dcl_blendindices v2;\n"
			"dcl_normal v3;\n"
			"dcl_texcoord0 v4;\n"
			"// Compensate for lack of UBYTE4 on Geforce3\n"
			"mul r1,v2.zyxw,c0.wwww\n"
			"//mul r1,v2,c0.wwww\n"
			"//first compute the last blending weight\n"
			"dp3 r0.w,v1.xyz,c0.xxz; \n"
			"add r0.w,-r0.w,c0.x\n"
			"//Set 1\n"
			"mov a0.x,r1.x\n"
			"m4x3 r4.xyz,v0,c[a0.x + 9];\n"
			"m3x3 r5.xyz,v3,c[a0.x + 9];\n"
			"//blend them\n"
			"mul r4.xyz,r4.xyz,v1.xxxx\n"
			"mul r5.xyz,r5.xyz,v1.xxxx\n"
			"//Set 2\n"
			"mov a0.x,r1.y\n"
			"m4x3 r2.xyz,v0,c[a0.x + 9];\n"
			"m3x3 r3.xyz,v3,c[a0.x + 9];\n"
			"//add them in\n"
			"mad r4.xyz,r2.xyz,v1.yyyy,r4;\n"
			"mad r5.xyz,r3.xyz,v1.yyyy,r5;\n"
			"//Set 3\n"
			"mov a0.x,r1.z\n"
			"m4x3 r2.xyz,v0,c[a0.x + 9];\n"
			"m3x3 r3.xyz,v3,c[a0.x + 9];\n"
			"//add them in\n"
			"mad r4.xyz,r2.xyz,r0.wwww,r4;\n"
			"mad r5.xyz,r3.xyz,r0.wwww,r5;\n"
			"//compute position\n"
			"mov r4.w,c0.x\n"
			"m4x4 oPos,r4,c2\n"
			"// normalize normals\n"
			"dp3 r5.w, r5, r5;\n"
			"rsq r5.w, r5.w;\n"
			"mul r5, r5, r5.w;\n"
			"; Do the lighting calculation\n"
			"dp3 r1.x, r5, c1      ; normal dot light\n"
			"lit r1, r1\n"
			"mul r0, r1.y, c8      ; Multiply with diffuse\n"
			"add r0, r0, c7        ; Add in ambient\n"
			"min oD0, r0, c0.x     ; clamp if > 1\n"
			"mov oD1, c0.zzzz      ; output specular\n"
			"; Copy texture coordinate\n"
			"mov oT0, v4\n";
		
		// Infl 4
		const char asm_shaders_4[] =
			"vs.1.1\n"
			"dcl_position v0;\n"
			"dcl_blendweight v1;\n"
			"dcl_blendindices v2;\n"
			"dcl_normal v3;\n"
			"dcl_texcoord0 v4;\n"
			"// Compensate for lack of UBYTE4 on Geforce3\n"
			"mul r1,v2.zyxw,c0.wwww\n"
			"//mul r1,v2,c0.wwww\n"
			"//first compute the last blending weight\n"
			"dp3 r0.w,v1.xyz,c0.xxx;\n"
			"add r0.w,-r0.w,c0.x\n"
			"//Set 1\n"
			"mov a0.x,r1.x\n"
			"m4x3 r4.xyz,v0,c[a0.x + 9];\n"
			"m3x3 r5.xyz,v3,c[a0.x + 9]; \n"
			"//blend them\n"
			"mul r4.xyz,r4.xyz,v1.xxxx\n"
			"mul r5.xyz,r5.xyz,v1.xxxx\n"
			"//Set 2\n"
			"mov a0.x,r1.y\n"
			"m4x3 r2.xyz,v0,c[a0.x + 9];\n"
			"m3x3 r3.xyz,v3,c[a0.x + 9];\n"
			"//add them in\n"
			"mad r4.xyz,r2.xyz,v1.yyyy,r4;\n"
			"mad r5.xyz,r3.xyz,v1.yyyy,r5;\n"
			"//Set 3\n"
			"mov a0.x,r1.z\n"
			"m4x3 r2.xyz,v0,c[a0.x + 9];\n"
			"m3x3 r3.xyz,v3,c[a0.x + 9];\n"
			"//add them in\n"
			"mad r4.xyz,r2.xyz,v1.zzzz,r4;\n"
			"mad r5.xyz,r3.xyz,v1.zzzz,r5;\n"
			"//Set 4\n"
			"mov a0.x,r1.w\n"
			"m4x3 r2.xyz,v0,c[a0.x + 9];\n"
			"m3x3 r3.xyz,v3,c[a0.x + 9];\n"
			"//add them in\n"
			"mad r4.xyz,r2.xyz,r0.wwww,r4;\n"
			"mad r5.xyz,r3.xyz,r0.wwww,r5;\n"
			"//compute position\n"
			"mov r4.w,c0.x\n"
			"m4x4 oPos,r4,c2\n"
			"// normalize normals\n"
			"dp3 r5.w, r5, r5;\n"
			"rsq r5.w, r5.w;\n"
			"mul r5, r5, r5.w;\n"
			"; Do the lighting calculation\n"
			"dp3 r1.x, r5, c1      ; normal dot light\n"
			"lit r1, r1\n"
			"mul r0, r1.y, c8      ; Multiply with diffuse\n"
			"add r0, r0, c7        ; Add in ambient\n"
			"min oD0, r0, c0.x     ; clamp if > 1\n"
			"mov oD1, c0.zzzz      ; output specular\n"
			"; Copy texture coordinate\n"
			"mov oT0, v4\n";
		
		const char *asm_shaders[] = {
			asm_shaders_1,
			asm_shaders_2,
			asm_shaders_3,
			asm_shaders_4,
		};
		
		const int asm_shader_sizes[] = {
			sizeof(asm_shaders_1),
			sizeof(asm_shaders_2),
			sizeof(asm_shaders_3),
			sizeof(asm_shaders_4),
		};
	}

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

		dx::IDirect3DVertexShaderPtr shader_[4];
	};

	/// 頂点シェーダー式スキンモデルクラス
	class VertexShaderSkinBrush : public Brush
	{
	public:
		enum { MAX_MATRICIES = 28 };

		VertexShaderSkinBrush(Model &target)
			: Brush(target), is_use_sw_(false), pal_size_(0), max_face_infl_(0), attr_num_(0)
		{
		}

		HRslt setUp()
		{
			if(!vs_) {
				vs_ = new SkinningVertexShader;
				if(vs_) vs_->setUp();
			}

			HRslt hr;
			mesh_ = 0; bonecb_ = 0; pal_size_ = 0; attr_num_ = 0; max_face_infl_ = 0;
			
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
				&attr_num_,
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
			return hr;
		}

		/// 描画
		HRslt draw(const Skeleton &tree/**< モーションがセットされたスケルトン*/) const
		{
			return drawLow(tree, -1);
		}
		/// 描画
		HRslt draw(const Skeleton &tree/**< モーションがセットされたスケルトン*/, int mtrlno) const
		{
			return drawLow(tree, mtrlno);
		}

	protected:
		/// 描画
		HRslt drawLow(const Skeleton &tree/**< モーションがセットされたスケルトン*/, int mtrlno) const
		{
			HRslt hr;
			if(mesh_ && vs_) {
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

				// Use COLOR instead of UBYTE4 since Geforce3 does not support it
				// _const.w should be 3, but due to about hack, mul by 255 and add epsilon
				Vector4 _const = { 1.0f, 0.0f, 0.0f, 765.01f };

				D3DXMATRIXA16 view_mat;
				dev->GetTransform(D3DTS_VIEW, &view_mat);
				D3DXBONECOMBINATION *bonecb = reinterpret_cast<D3DXBONECOMBINATION*>(bonecb_->GetBufferPointer());
				for(ulong i = 0; i < attr_num_; i++) {
					if(mtrlno != -1 && bonecb[i].AttribId != static_cast<uint>(mtrlno)) break;
					for(ulong j = 0; j < pal_size_; j++) {
						DWORD matid = bonecb[i].BoneId[j];
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
					Color amb_emm = target_.mtrls[bonecb[i].AttribId].ambient*getAmbient()+target_.mtrls[bonecb[i].AttribId].emissive;
					dev->SetVertexShaderConstantF(7, amb_emm, 1);
					dev->SetVertexShaderConstantF(8, target_.mtrls[bonecb[i].AttribId].diffuse, 1);
					_const.y = target_.mtrls[bonecb[i].AttribId].power;
					dev->SetVertexShaderConstantF(0, _const, 1);

					Pointer<Texture> tex = target_.mtrls[bonecb[i].AttribId].tex.get();
					if(tex) dev->SetTexture(0, *tex);
					else dev->SetTexture(0, NULL);

					hr = mesh_->DrawSubset(i);
					if(!hr) return hr;
				}

				if(is_use_sw_) dev->SetSoftwareVertexProcessing(FALSE);
			}
			return hr;
		}

		bool					is_use_sw_;
		ulong					pal_size_;
		ulong					max_face_infl_;
		ulong					attr_num_;
		ID3DXBufferPtr			bonecb_; // ボーンの組み合わせデータ
		ID3DXMeshPtr			mesh_;

		static Pointer<SkinningVertexShader> vs_;
	};
	
	/// HLSL式スキンモデルクラス
	class ShaderSkinBrush : public Brush
	{
	public:
		enum { MAX_MATRICIES = 26 }; // 実際はもっと使えるんだけど…
		// asm型に比べて不便かも

		ShaderSkinBrush(Model &target)
			: Brush(target), is_use_sw_(false), pal_size_(0), max_face_infl_(0), attr_num_(0)
		{
		}

		HRslt setUp()
		{
			HRslt hr;
			mesh_ = 0; bonecb_ = 0; pal_size_ = 0; attr_num_ = 0; max_face_infl_ = 0;
			
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
				&attr_num_,
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

			for(ulong i = 0; i < attr_num_; i++) {
				D3DXBONECOMBINATION *bonecb = reinterpret_cast<D3DXBONECOMBINATION*>(bonecb_->GetBufferPointer());
				for(ulong j = 0; j < pal_size_; j++) {
					DWORD matid = bonecb[i].BoneId[j];
					if(matid != UINT_MAX) {
						const char *name = target_.bonename(matid);
						if(name) PRNN("bonename "<<i<<","<<bonecb[i].AttribId<<","<<j<<","<<matid<<" "<<name);
						else PRNN("bonename "<<i<<","<<bonecb[i].AttribId<<","<<j<<","<<matid<<" ???");
					}
				}
			}

			hr = mesh_->UpdateSemantics(pDecl);
			if(!hr) return hr;

			bone_matricies_.resize(pal_size_);

			if(!shader_) shader_ = graphic::createOnDB<dx::HLSLShader>(_T("skinnedmesh.fx"));
			return hr;
		}

		/// 描画
		HRslt draw(const Skeleton &tree/**< モーションがセットされたスケルトン*/) const
		{
			return draw(tree, 0);
		}
		/// 描画
		HRslt draw(const Skeleton &tree/**< モーションがセットされたスケルトン*/, int mtrlno) const
		{
			D3DXBONECOMBINATION *bonecb = reinterpret_cast<D3DXBONECOMBINATION*>(bonecb_->GetBufferPointer());
			//for(ulong i = 0; i < attr_num_; i++) {
			//	if(bonecb[i].AttribId != static_cast<uint>(mtrlno)) return S_FALSE;
			//}
			HRslt hr;
			//Handle<dx::HLSLShader> shader = target_.mtrls[mtrlno].shader;
			Handle<dx::HLSLShader> shader = shader_;
			if(mesh_ && shader) {
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
				D3DXMATRIX *bm_buf = const_cast< D3DXMATRIX * >(&bone_matricies_[0]);

				for(ulong i = 0; i < attr_num_; i++) {
					if(hr = shader->begin()) {
						for(ulong j = 0; j < pal_size_; j++)
						{
							DWORD matid = bonecb[i].BoneId[j];
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
						Color amb_emm = target_.mtrls[bonecb[i].AttribId].ambient*getAmbient()+target_.mtrls[bonecb[i].AttribId].emissive;
						hr = (*shader)->SetVector("MaterialAmbient", (D3DXVECTOR4*)&amb_emm);
						if(!hr) GCTP_TRACE(hr);
						// set material color properties 
						hr = (*shader)->SetVector("MaterialDiffuse", (D3DXVECTOR4*)&target_.mtrls[bonecb[i].AttribId].diffuse);
						if(!hr) GCTP_TRACE(hr);

						hr = (*shader)->SetInt( "CurNumBones", max_face_infl_-1);
						if(!hr) GCTP_TRACE(hr);

						Pointer<Texture> tex = target_.mtrls[bonecb[i].AttribId].tex.lock();
						if(tex) dev->SetTexture(0, *tex);
						else dev->SetTexture(0, NULL);

						for(uint ipass = 0; ipass < shader->passnum(); ipass++) {
							hr = shader->beginPass( ipass );
							if(!hr) GCTP_TRACE(hr);
							hr = mesh_->DrawSubset(i);
							if(!hr) {
								GCTP_TRACE(hr);
								PRNN("i = "<<i<<";"<<"ipass = "<<ipass<<";"<<"passnum = "<<shader_->passnum());
							}
							hr = shader->endPass();
							if(!hr) GCTP_TRACE(hr);
						}

						hr = shader->end();
						if(!hr) GCTP_TRACE(hr);
					}
					else GCTP_TRACE(hr);
				}

				if(is_use_sw_) dev->SetSoftwareVertexProcessing(FALSE);
			}
			return hr;
		}

	protected:
		bool						is_use_sw_;
		ulong						pal_size_;
		ulong						max_face_infl_;
		ulong						attr_num_;
		ID3DXBufferPtr				bonecb_; // ボーンの組み合わせデータ
		ID3DXMeshPtr				mesh_;
		Pointer<dx::HLSLShader>		shader_;
		std::vector<D3DXMATRIX>		bone_matricies_;
	};

}} // namespace gctp::graphic
#endif // _GCTP_GRAPHIC_BRUSH_HPP_