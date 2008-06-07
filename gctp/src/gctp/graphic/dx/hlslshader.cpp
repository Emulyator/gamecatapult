/** @file
 * GameCatapult HLSLシェーダーリソースクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 3:31:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/graphic.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/graphic/dx/hlslshader.hpp>
#include <gctp/graphic/rsrc.hpp>
#include <gctp/extension.hpp>
#include <gctp/buffer.hpp>
#include <gctp/dxcomptrs.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace graphic { namespace dx {

	namespace {

		Ptr realizefx(Ptr self, BufferPtr buffer)
		{
			Pointer<HLSLShader> ret;
			if(self) ret = self;
			else ret = new HLSLShader;
			if(ret && buffer) {
				if(ret->setUp(buffer)) {
					return ret;
				}
				return Ptr();
			}
			return ret;
		}

		Extension ext_fx_realizer(_T("fx"), realizefx);

	}

	GCTP_IMPLEMENT_CLASS_NS3(gctp, graphic, dx, HLSLShader, Shader);
	
	/// シェーダーファイルから読みこみ
	HRslt HLSLShader::setUp(const _TCHAR *fname)
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
				GCTP_ERRORINFO("\n"<<(const char *)err->GetBufferPointer());
			}
		}
		if(!hr) {
			{
				ID3DXBufferPtr err;
				hr = D3DXCreateEffectFromResource( device().impl(), NULL, fname, NULL, NULL, dwShaderFlags, NULL, &ptr_, &err );
				if(!hr && err) {
					GCTP_ERRORINFO("\n"<<(const char *)err->GetBufferPointer());
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
		GCTP_ERRORINFO(hr);
		return hr;
	}

	/// シェーダースクリプトを直指定
	HRslt HLSLShader::setUp(const char *source, uint source_len)
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
			hr = D3DXCreateEffect( device().impl(), source, source_len, NULL, NULL, dwShaderFlags, NULL, &ptr_, &err );
			if(!hr && err) {
				GCTP_ERRORINFO("\n"<<(const char *)err->GetBufferPointer());
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
		GCTP_ERRORINFO(hr);
		return hr;
	}

	/// メモリ内シェーダーファイルから読みこみ
	HRslt HLSLShader::setUp(BufferPtr buffer)
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
				GCTP_ERRORINFO("\n"<<(const char *)err->GetBufferPointer());
			}
		}
		GCTP_ERRORINFO(hr);
		return hr;
	}

	bool HLSLShader::hasTechnique(const char *name) const
	{
		D3DXHANDLE prev = 0;
		D3DXHANDLE tech;
		while(true) {
			HRslt hr = ptr_->FindNextValidTechnique(prev, &tech);
			if(hr) {
				if(strncmp(tech, name, 256) == 0) return true;
				prev = tech;
			}
			else break;
		}
		return false;
	}

	HRslt HLSLShader::restore()
	{
		if( ptr_ ) return ptr_->OnResetDevice();
		return S_OK;
	}

	void HLSLShader::cleanUp()
	{
		if( ptr_ ) ptr_->OnLostDevice();
	}

	HRslt HLSLShader::begin(const char *technique, uint passno) const
	{
		if(ptr_) {
			HRslt hr;
			if(technique && hasTechnique(technique)) {
				hr = ptr_->SetTechnique(technique);
				if(!hr) {
					GCTP_TRACE(hr);
					return hr;
				}
			}
			hr = ptr_->Begin(&passnum_, 0/*D3DXFX_DONOTSAVESTATE*/);
			if(hr) {
				if(passno < passnum_) {
					hr = ptr_->BeginPass(passno);
					if(!hr) ptr_->End();
				}
				else {
					ptr_->End();
					return E_INVALIDARG;
				}
			}
			return hr;
		}
		return E_POINTER;
	}

	HRslt HLSLShader::end() const
	{
		passnum_ = 0;
		if(ptr_) {
			ptr_->EndPass();
			return ptr_->End();
		}
		return E_POINTER;
	}

}}} // namespace gctp
