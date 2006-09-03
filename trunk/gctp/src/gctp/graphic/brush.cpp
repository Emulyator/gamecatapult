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
#include <gctp/dxcomptrs.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace graphic {

	namespace {

		GCTP_REGISTER_REALIZER(fx, Brush);
		
		// SkinnedMesh.fx
		const _TCHAR skinned_mesh_fx[] =
			_T("//\n")
			_T("// Skinned Mesh Effect file \n")
			_T("// Copyright (c) 2000-2002 Microsoft Corporation. All rights reserved.\n")
			_T("//\n")
			_T("\n")
			_T("float4 lhtDir = {0.0f, 0.0f, -1.0f, 1.0f};    //light Direction \n")
			_T("float4 lightDiffuse = {0.6f, 0.6f, 0.6f, 1.0f}; // Light Diffuse\n")
			_T("float4 MaterialAmbient : MATERIALAMBIENT = {0.1f, 0.1f, 0.1f, 1.0f};\n")
			_T("float4 MaterialDiffuse : MATERIALDIFFUSE = {0.8f, 0.8f, 0.8f, 1.0f};\n")
			_T("\n")
			_T("// Matrix Pallette\n")
			_T("static const int MAX_MATRICES = 26;\n")
			_T("float4x3    mWorldMatrixArray[MAX_MATRICES] : WORLDMATRIXARRAY;\n")
			_T("float4x4    mViewProj : VIEWPROJECTION;\n")
			_T("\n")
			_T("///////////////////////////////////////////////////////\n")
			_T("struct VS_INPUT\n")
			_T("{\n")
			_T("    float4  Pos             : POSITION;\n")
			_T("    float4  BlendWeights    : BLENDWEIGHT;\n")
			_T("    float4  BlendIndices    : BLENDINDICES;\n")
			_T("    float3  Normal          : NORMAL;\n")
			_T("    float3  Tex0            : TEXCOORD0;\n")
			_T("};\n")
			_T("\n")
			_T("struct VS_OUTPUT\n")
			_T("{\n")
			_T("    float4  Pos     : POSITION;\n")
			_T("    float4  Diffuse : COLOR;\n")
			_T("    float2  Tex0    : TEXCOORD0;\n")
			_T("};\n")
			_T("\n")
			_T("\n")
			_T("float3 Diffuse(float3 Normal)\n")
			_T("{\n")
			_T("    float CosTheta;\n")
			_T("    \n")
			_T("    // N.L Clamped\n")
			_T("    CosTheta = max(0.0f, dot(Normal, lhtDir.xyz));\n")
			_T("       \n")
			_T("    // propogate scalar result to vector\n")
			_T("    return (CosTheta);\n")
			_T("}\n")
			_T("\n")
			_T("\n")
			_T("VS_OUTPUT VShade(VS_INPUT i, uniform int NumBones)\n")
			_T("{\n")
			_T("    VS_OUTPUT   o;\n")
			_T("    float3      Pos = 0.0f;\n")
			_T("    float3      Normal = 0.0f;    \n")
			_T("    float       LastWeight = 0.0f;\n")
			_T("     \n")
			_T("    // Compensate for lack of UBYTE4 on Geforce3\n")
			_T("    int4 IndexVector = D3DCOLORtoUBYTE4(i.BlendIndices);\n")
			_T("\n")
			_T("    // cast the vectors to arrays for use in the for loop below\n")
			_T("    float BlendWeightsArray[4] = (float[4])i.BlendWeights;\n")
			_T("    int   IndexArray[4]        = (int[4])IndexVector;\n")
			_T("    \n")
			_T("    // calculate the pos/normal using the \"normal\" weights \n")
			_T("    //        and accumulate the weights to calculate the last weight\n")
			_T("    for (int iBone = 0; iBone < NumBones-1; iBone++)\n")
			_T("    {\n")
			_T("        LastWeight = LastWeight + BlendWeightsArray[iBone];\n")
			_T("        \n")
			_T("        Pos += mul(i.Pos, mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];\n")
			_T("        Normal += mul(i.Normal, mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];\n")
			_T("    }\n")
			_T("    LastWeight = 1.0f - LastWeight; \n")
			_T("\n")
			_T("    // Now that we have the calculated weight, add in the final influence\n")
			_T("    Pos += (mul(i.Pos, mWorldMatrixArray[IndexArray[NumBones-1]]) * LastWeight);\n")
			_T("    Normal += (mul(i.Normal, mWorldMatrixArray[IndexArray[NumBones-1]]) * LastWeight); \n")
			_T("    \n")
			_T("    // transform position from world space into view and then projection space\n")
			_T("    o.Pos = mul(float4(Pos.xyz, 1.0f), mViewProj);\n")
			_T("\n")
			_T("    // normalize normals\n")
			_T("    Normal = normalize(Normal);\n")
			_T("\n")
			_T("    // Shade (Ambient + etc.)\n")
			_T("    o.Diffuse.xyz = MaterialAmbient.xyz + Diffuse(Normal) * MaterialDiffuse.xyz;\n")
			_T("    o.Diffuse.w = 1.0f;\n")
			_T("\n")
			_T("    // copy the input texture coordinate through\n")
			_T("    o.Tex0  = i.Tex0.xy;\n")
			_T("\n")
			_T("    return o;\n")
			_T("}\n")
			_T("\n")
			_T("int CurNumBones = 2;\n")
			_T("VertexShader vsArray[4] = { compile vs_1_1 VShade(1), \n")
			_T("                            compile vs_1_1 VShade(2),\n")
			_T("                            compile vs_1_1 VShade(3),\n")
			_T("                            compile vs_1_1 VShade(4)\n")
			_T("                          };\n")
			_T("\n")
			_T("\n")
			_T("//////////////////////////////////////\n")
			_T("// Techniques specs follow\n")
			_T("//////////////////////////////////////\n")
			_T("technique t0\n")
			_T("{\n")
			_T("    pass p0\n")
			_T("    {\n")
			_T("        VertexShader = (vsArray[CurNumBones]);\n")
			_T("    }\n")
			_T("}\n");
		
		const _TCHAR *shaders[] = {
			skinned_mesh_fx,
		};
		
		const int shader_sizes[] = {
			sizeof(skinned_mesh_fx),
		};
	}

	GCTP_IMPLEMENT_CLASS_NS(gctp, Brush, Object);

	/// シェーダーファイルから読みこみ
	HRslt Brush::setUp(const char *fname)
	{
		DWORD dwShaderFlags = 0;
#ifdef GCTP_DEBUG
		dwShaderFlags |= D3DXSHADER_DEBUG|D3DXSHADER_SKIPVALIDATION;
//		dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
//		dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
#endif
		HRslt hr;
		ID3DXBufferPtr err;
		hr = D3DXCreateEffectFromFile( device().impl(), fname, NULL, NULL, dwShaderFlags, NULL, &ptr_, &err );
		if(!hr) {
			hr = D3DXCreateEffectFromResource( device().impl(), NULL, fname, NULL, NULL, dwShaderFlags, NULL, &ptr_, &err );
			if(!hr) {
				if(string("skinnedmesh.fx") == fname) {
					hr = D3DXCreateEffect( device().impl(), shaders[0], shader_sizes[0], NULL, NULL, dwShaderFlags, NULL, &ptr_, &err );
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
		if(err) PRNN((const char *)err->GetBufferPointer());
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
