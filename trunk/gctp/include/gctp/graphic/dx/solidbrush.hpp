#ifndef _GCTP_GRAPHIC_DX_SOLIDBRUSH_HPP_
#define _GCTP_GRAPHIC_DX_SOLIDBRUSH_HPP_
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

	/// 非スキンHLSLシェーダーブラシ基底クラス
	class SolidShaderBrush : public Brush {
	public:
		SolidShaderBrush(Model &target) : Brush(target) {}
		
		/// 描画準備
		HRslt begin(Handle<Shader> _shader, const Matrix &mat/**< モデル行列*/) const
		{
			Handle<dx::HLSLShader> shader = _shader;
			if(mesh_ && shader) {
				HRslt hr;
				dx::IDirect3DDevicePtr dev;
				hr = mesh_->GetDevice(&dev);
				if(!hr) return hr;
								
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
		HRslt draw(uint subset_no, const Matrix &mat/**< モデル行列*/) const
		{
			HRslt hr;
			Handle<dx::HLSLShader> shader = target_.mtrls[bonecb[subset_no].AttribId].shader;
			if(shader) {
				dx::IDirect3DDevicePtr dev;
				hr = mesh_->GetDevice(&dev);
				if(!hr) return hr;
				
				// Sum of all ambient and emissive contribution
				Color amb_emm = target_.mtrls[bonecb[subset_no].AttribId].ambient*getAmbient()+target_.mtrls[bonecb[subset_no].AttribId].emissive;
				hr = (*shader)->SetVector("MaterialAmbient", (D3DXVECTOR4*)&amb_emm);
				if(!hr) GCTP_TRACE(hr);
				// set material color properties 
				hr = (*shader)->SetVector("MaterialDiffuse", (D3DXVECTOR4*)&target_.mtrls[bonecb[subset_no].AttribId].diffuse);
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
	};

}}} // namespace gctp::graphic::dx
#endif // _GCTP_GRAPHIC_DX_SOLIDBRUSH_HPP_