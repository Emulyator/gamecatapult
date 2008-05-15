/** @file
 * GameCatapult レンダーターゲット
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 3:31:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/graphic.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/graphic/rsrc.hpp>
#include <gctp/graphic/rendertarget.hpp>
#include <gctp/graphic/texture.hpp>
#include <gctp/dbgout.hpp>
#include <gctp/turi.hpp>
#include <gctp/buffer.hpp>
#include <gctp/dxcomptrs.hpp>

using namespace std;

namespace gctp { namespace graphic {

	GCTP_IMPLEMENT_CLASS_NS2(gctp, graphic, RenderTarget, Object);

	RenderTarget::RenderTarget() : index_for_system_(0)
	{
	}

	/// createOnDB用
	HRslt RenderTarget::setUp(const _TCHAR *fname)
	{
		assert("not implement.");
		return S_OK;
		//return setUp(width, height, format, miplevel);
	}

	/// 領域確保
	HRslt RenderTarget::setUp(Type type, int width, int height, int format, int miplevel)
	{
		org_width_ = width;
		org_height_ = height;
		org_format_ = format;
		org_miplevel_ = miplevel;
		type_ = type;
	
		HRslt hr;
		if(type_ == OFFSCREEN) {
			hr = device().impl()->CreateRenderTarget(width, height, (D3DFORMAT)format, D3DMULTISAMPLE_NONE, 0, TRUE, &ptr_, NULL);
			if(!hr) GCTP_TRACE(hr);
			else {
				tex_ = new Texture;
				hr = tex_->setUp(Texture::OFFSCREEN, width, height, format, miplevel);
				if(!hr) {
					ptr_ = 0;
					tex_ = 0;
					GCTP_TRACE(hr);
				}
			}
		}
		else {
			ptr_ = 0;
			tex_ = new Texture;
			hr = tex_->setUp(Texture::RENDERTARGET, width, height, format, miplevel);
			if(!hr) {
				tex_ = 0;
				GCTP_TRACE(hr);
			}
		}
		return hr;
	}

	Point2 RenderTarget::size() const
	{
		D3DSURFACE_DESC desc;
		if(ptr_) {
			ptr_->GetDesc(&desc);
			return Point2C(desc.Width, desc.Height);
		}
		return Point2C(-1,-1);
	}

	HRslt RenderTarget::restore()
	{
		if(type_ == OFFSCREEN) {
			if(tex_) return device().impl()->CreateRenderTarget(org_width_, org_height_, (D3DFORMAT)org_format_, D3DMULTISAMPLE_NONE, 0, TRUE, &ptr_, NULL);
		}
		return S_OK;
	}

	void RenderTarget::cleanUp()
	{
		backup_ = 0;
		ptr_ = 0;
	}

	/// システムに設定（要するにSetRenderTarget）
	HRslt RenderTarget::begin(uint index)
	{
		index_for_system_ = index;
		backup_ = 0;
		device().impl()->GetRenderTarget(index, &backup_);
		if(type_ == OFFSCREEN) {
			return device().impl()->SetRenderTarget(index, this ? ptr_ : NULL);
		}
		else {
			dx::IDirect3DSurfacePtr surf;
			tex_->get()->GetSurfaceLevel(0, &surf);
			return device().impl()->SetRenderTarget(index, surf);
		}
	}

	HRslt RenderTarget::end()
	{
		HRslt ret = device().impl()->SetRenderTarget(index_for_system_, backup_);
		backup_ = 0;
		return ret;
	}

	HRslt RenderTarget::resolve()
	{
		if(type_ == OFFSCREEN) {
			dx::IDirect3DSurfacePtr surface;
			tex_->get()->GetSurfaceLevel(0, &surface);
			return device().impl()->GetRenderTargetData(ptr_, surface);
		}
		return S_OK;
	}

	RenderTarget::LockedRect RenderTarget::lock(const Rect &rc, uint32_t flag)
	{
		D3DLOCKED_RECT lr;
		if(HRslt(ptr_->LockRect(&lr, rc, flag))) {
			return LockedRect(lr.Pitch, lr.pBits);
		}
		return LockedRect(0, 0);
	}

	RenderTarget::LockedRect RenderTarget::lock(uint32_t flag)
	{
		D3DLOCKED_RECT lr;
		if(HRslt(ptr_->LockRect(&lr, 0, flag))) {
			return LockedRect(lr.Pitch, lr.pBits);
		}
		return LockedRect(0, 0);
	}

	HRslt RenderTarget::unlock()
	{
		return ptr_->UnlockRect();
	}

	HRslt RenderTarget::save(const _TCHAR *fn)
	{
		TURI save_file(fn);
		basic_string<_TCHAR> ext = save_file.extension();
		D3DXIMAGE_FILEFORMAT format = D3DXIFF_BMP;
		if(ext == _T("jpg")) format = D3DXIFF_JPG;
		else if(ext == _T("tga")) format = D3DXIFF_TGA;
		else if(ext == _T("png")) format = D3DXIFF_PNG;
		else if(ext == _T("dds")) format = D3DXIFF_DDS;
		else if(ext == _T("ppm")) format = D3DXIFF_PPM;
		else if(ext == _T("dib")) format = D3DXIFF_DIB;
		else if(ext == _T("hdr")) format = D3DXIFF_HDR;
		else if(ext == _T("pfm")) format = D3DXIFF_PFM;
		return D3DXSaveSurfaceToFile(fn, format, ptr_, NULL, NULL);
	}

	HRslt RenderTarget::save(BufferPtr &ret, D3DXIMAGE_FILEFORMAT format)
	{
		ID3DXBufferPtr buf;
		HRslt r = D3DXSaveSurfaceToFileInMemory(&buf, format, ptr_, NULL, NULL);
		if(r && buf) {
			ret = new Buffer(buf->GetBufferSize());
			memcpy(ret->buf(), buf->GetBufferPointer(), ret->size());
		}
		return r;
	}

}} // namespace gctp
