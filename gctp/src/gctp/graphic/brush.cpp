/** @file
 * GameCatapult シェーダーリソースクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 3:31:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/graphic.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/graphic/rsrc.hpp>
#include <gctp/graphic/brush.hpp>
#include <gctp/extension.hpp>
#include <gctp/buffer.hpp>
#include <gctp/dxcomptrs.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace graphic {

	namespace {

		GCTP_REGISTER_REALIZER(fx, Brush);
		
		// SkinnedMesh.fx
		const char skinned_mesh_fx[] =
			"//\n"
			"// Skinned Mesh Effect file \n"
			"// Copyright (c) 2000-2002 Microsoft Corporation. All rights reserved.\n"
			"//\n"
			"\n"
			"float4 lhtDir = {0.0f, 0.0f, -1.0f, 1.0f};    //light Direction \n"
			"float4 lightDiffuse = {0.6f, 0.6f, 0.6f, 1.0f}; // Light Diffuse\n"
			"float4 MaterialAmbient : MATERIALAMBIENT = {0.1f, 0.1f, 0.1f, 1.0f};\n"
			"float4 MaterialDiffuse : MATERIALDIFFUSE = {0.8f, 0.8f, 0.8f, 1.0f};\n"
			"\n"
			"// Matrix Pallette\n"
			"static const int MAX_MATRICES = 26;\n"
			"float4x3    mWorldMatrixArray[MAX_MATRICES] : WORLDMATRIXARRAY;\n"
			"float4x4    mViewProj : VIEWPROJECTION;\n"
			"\n"
			"///////////////////////////////////////////////////////\n"
			"struct VS_INPUT\n"
			"{\n"
			"    float4  Pos             : POSITION;\n"
			"    float4  BlendWeights    : BLENDWEIGHT;\n"
			"    float4  BlendIndices    : BLENDINDICES;\n"
			"    float3  Normal          : NORMAL;\n"
			"    float3  Tex0            : TEXCOORD0;\n"
			"};\n"
			"\n"
			"struct VS_OUTPUT\n"
			"{\n"
			"    float4  Pos     : POSITION;\n"
			"    float4  Diffuse : COLOR;\n"
			"    float2  Tex0    : TEXCOORD0;\n"
			"};\n"
			"\n"
			"\n"
			"float3 Diffuse(float3 Normal)\n"
			"{\n"
			"    float CosTheta;\n"
			"    \n"
			"    // N.L Clamped\n"
			"    CosTheta = max(0.0f, dot(Normal, lhtDir.xyz));\n"
			"       \n"
			"    // propogate scalar result to vector\n"
			"    return (CosTheta);\n"
			"}\n"
			"\n"
			"\n"
			"VS_OUTPUT VShade(VS_INPUT i, uniform int NumBones)\n"
			"{\n"
			"    VS_OUTPUT   o;\n"
			"    float3      Pos = 0.0f;\n"
			"    float3      Normal = 0.0f;    \n"
			"    float       LastWeight = 0.0f;\n"
			"     \n"
			"    // Compensate for lack of UBYTE4 on Geforce3\n"
			"    int4 IndexVector = D3DCOLORtoUBYTE4(i.BlendIndices);\n"
			"\n"
			"    // cast the vectors to arrays for use in the for loop below\n"
			"    float BlendWeightsArray[4] = (float[4])i.BlendWeights;\n"
			"    int   IndexArray[4]        = (int[4])IndexVector;\n"
			"    \n"
			"    // calculate the pos/normal using the \"normal\" weights \n"
			"    //        and accumulate the weights to calculate the last weight\n"
			"    for (int iBone = 0; iBone < NumBones-1; iBone++)\n"
			"    {\n"
			"        LastWeight = LastWeight + BlendWeightsArray[iBone];\n"
			"        \n"
			"        Pos += mul(i.Pos, mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];\n"
			"        Normal += mul(i.Normal, mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];\n"
			"    }\n"
			"    LastWeight = 1.0f - LastWeight; \n"
			"\n"
			"    // Now that we have the calculated weight, add in the final influence\n"
			"    Pos += (mul(i.Pos, mWorldMatrixArray[IndexArray[NumBones-1]]) * LastWeight);\n"
			"    Normal += (mul(i.Normal, mWorldMatrixArray[IndexArray[NumBones-1]]) * LastWeight); \n"
			"    \n"
			"    // transform position from world space into view and then projection space\n"
			"    o.Pos = mul(float4(Pos.xyz, 1.0f), mViewProj);\n"
			"\n"
			"    // normalize normals\n"
			"    Normal = normalize(Normal);\n"
			"\n"
			"    // Shade (Ambient + etc.)\n"
			"    o.Diffuse.xyz = MaterialAmbient.xyz + Diffuse(Normal) * MaterialDiffuse.xyz;\n"
			"    o.Diffuse.w = 1.0f;\n"
			"\n"
			"    // copy the input texture coordinate through\n"
			"    o.Tex0  = i.Tex0.xy;\n"
			"\n"
			"    return o;\n"
			"}\n"
			"\n"
			"int CurNumBones = 2;\n"
			"VertexShader vsArray[4] = { compile vs_1_1 VShade(1), \n"
			"                            compile vs_1_1 VShade(2),\n"
			"                            compile vs_1_1 VShade(3),\n"
			"                            compile vs_1_1 VShade(4)\n"
			"                          };\n"
			"\n"
			"\n"
			"//////////////////////////////////////\n"
			"// Techniques specs follow\n"
			"//////////////////////////////////////\n"
			"technique t0\n"
			"{\n"
			"    pass p0\n"
			"    {\n"
			"        VertexShader = (vsArray[CurNumBones]);\n"
			"    }\n"
			"}\n";
		
		const char *shaders[] = {
			skinned_mesh_fx,
		};
		
		const int shader_sizes[] = {
			sizeof(skinned_mesh_fx),
		};
	}

	GCTP_IMPLEMENT_CLASS_NS(gctp, Brush, Object);

	/// シェーダーファイルから読みこみ
	HRslt Brush::setUp(const _TCHAR *fname)
	{
		DWORD dwShaderFlags = 0;
#ifdef GCTP_DEBUG
		dwShaderFlags |= D3DXSHADER_DEBUG|D3DXSHADER_SKIPVALIDATION;
//		dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
//		dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
#endif
		HRslt hr;
		{
			ID3DXBufferPtr err;
			hr = D3DXCreateEffectFromFile( device().impl(), fname, NULL, NULL, dwShaderFlags, NULL, &ptr_, &err );
			if(!hr && err) {
				GCTP_TRACE("\n"<<(const char *)err->GetBufferPointer());
			}
		}
		if(!hr) {
			{
				ID3DXBufferPtr err;
				hr = D3DXCreateEffectFromResource( device().impl(), NULL, fname, NULL, NULL, dwShaderFlags, NULL, &ptr_, &err );
				if(!hr && err) {
					GCTP_TRACE("\n"<<(const char *)err->GetBufferPointer());
				}
			}
			if(!hr) {
				if(TLStr(_T("skinnedmesh.fx")) == fname) {
					ID3DXBufferPtr err;
					hr = D3DXCreateEffect( device().impl(), shaders[0], shader_sizes[0], NULL, NULL, dwShaderFlags, NULL, &ptr_, &err );
					if(!hr && err) {
						GCTP_TRACE("\n"<<(const char *)err->GetBufferPointer());
					}
				}
			}
		}
		if(hr) {
			D3DXHANDLE tech;
			hr = ptr_->FindNextValidTechnique(NULL, &tech);
			if(hr) {
				ptr_->SetTechnique(tech);
				return hr;
			}
		}
		GCTP_TRACE(hr);
		return hr;
	}

	/// メモリ内シェーダーファイルから読みこみ
	HRslt Brush::setUp(BufferPtr buffer)
	{
		DWORD dwShaderFlags = 0;
#ifdef GCTP_DEBUG
		dwShaderFlags |= D3DXSHADER_DEBUG|D3DXSHADER_SKIPVALIDATION;
//		dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
//		dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
#endif
		HRslt hr;
		{
			ID3DXBufferPtr err;
			hr = D3DXCreateEffect( device().impl(), buffer->buf(), (UINT)buffer->size(), NULL, NULL, dwShaderFlags, NULL, &ptr_, &err );
			if(!hr && err) {
				GCTP_TRACE("\n"<<(const char *)err->GetBufferPointer());
			}
		}
		if(hr) {
			D3DXHANDLE tech;
			hr = ptr_->FindNextValidTechnique(NULL, &tech);
			if(hr) {
				ptr_->SetTechnique(tech);
				return hr;
			}
		}
		GCTP_TRACE(hr);
		return hr;
	}

	HRslt Brush::restore()
	{
		if( ptr_ ) return ptr_->OnResetDevice();
		return S_OK;
	}

	void Brush::cleanUp()
	{
		if( ptr_ ) ptr_->OnLostDevice();
	}

	HRslt Brush::begin(uint &passnum) const
	{
		if(ptr_) return ptr_->Begin(&passnum, 0/*D3DXFX_DONOTSAVESTATE*/);
		return E_POINTER;
	}

	HRslt Brush::end() const
	{
		if(ptr_) return ptr_->End();
		return E_POINTER;
	}

	HRslt Brush::beginPass(uint passno) const
	{
		if(ptr_) return ptr_->BeginPass(passno);
		return E_POINTER;
	}

	HRslt Brush::endPass() const
	{
		if(ptr_) return ptr_->EndPass();
		return E_POINTER;
	}

}} // namespace gctp
