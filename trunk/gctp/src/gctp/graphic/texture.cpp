/** @file
 * GameCatapult テクスチャクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 3:31:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/graphic.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/graphic/rsrc.hpp>
#include <gctp/graphic/texture.hpp>
#include <gctp/extension.hpp>
#include <gctp/buffer.hpp>
#include <gctp/turi.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace graphic {

	namespace {

		Ptr realizeTexture(BufferPtr buffer)
		{
			Pointer<Texture> ret = new Texture;
			if(ret) {
				if(ret->setUp(buffer->buf(), buffer->size())) {
					return ret;
				}
			}
			return Ptr();
		}

#define __REGISTER(ext) Extension ext##_realizer(_T(#ext), realizeTexture)
		__REGISTER(dds);
		__REGISTER(bmp);
		__REGISTER(dib);
		__REGISTER(png);
		__REGISTER(jpg);
		__REGISTER(ppm);
		__REGISTER(tga);
#undef __REGISTER

		template<class E, class T>
		std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, D3DXIMAGE_INFO const & info)
		{
			os<<"{"<<info.Width<<","<<info.Height<<","<<info.Depth<<","<<info.MipLevels<<","<<info.Format
			  <<","<<info.ResourceType<<","<<info.ImageFileFormat<<"}";
			return os;
		}
	}

	GCTP_IMPLEMENT_CLASS_NS2(gctp, graphic, Texture, Object);

	Texture::Texture(bool no_scaling) : type_(NONE), no_scaling_(no_scaling)
	{
	}

	/// 画像ファイルから読みこみ
	HRslt Texture::setUp(const _TCHAR *fname)
	{
		if(!fname) return E_INVALIDARG;
		HRslt hr;
		D3DXIMAGE_INFO info;
		hr = ::D3DXGetImageInfoFromFile(fname, &info);
		if(hr) {
			hr = setUp(NORMAL, info.Width, info.Height, info.Format, info.MipLevels);
			if(hr) {
				dx::IDirect3DSurfacePtr surf;
				ptr_->GetSurfaceLevel(0, &surf);
				if(no_scaling_) {
					RECT rect = {0, 0, info.Width, info.Height};
					hr = ::D3DXLoadSurfaceFromFile(surf, 0, &rect, fname, 0, D3DX_FILTER_NONE, 0, 0);
				}
				else {
					hr = ::D3DXLoadSurfaceFromFile(surf, 0, 0, fname, 0, (size() != originalSize() ? D3DX_DEFAULT : D3DX_FILTER_NONE), 0, 0);
				}
			}
			if(!hr) {
				ptr_ = 0;
				PRNN(_T("D3DXCreateTextureFromFileを使用します"));
				no_scaling_ = false;
				hr = ::D3DXCreateTextureFromFile(device().impl(), fname, &ptr_);
			}
		}
		else {
			PRNN(_T("Win32リソース検索 ") << TURI(fname).basename());
			hr = ::D3DXGetImageInfoFromResource(NULL, TURI(fname).basename().c_str(), &info);
			if(!hr) return hr;
			hr = setUp(NORMAL, info.Width, info.Height, info.Format, info.MipLevels);
			if(hr) {
				dx::IDirect3DSurfacePtr surf;
				ptr_->GetSurfaceLevel(0, &surf);
				if(no_scaling_) {
					RECT rect = {0, 0, info.Width, info.Height};
					hr = ::D3DXLoadSurfaceFromResource(surf, 0, &rect, NULL, TURI(fname).basename().c_str(), 0, D3DX_FILTER_NONE, 0, 0);
				}
				else {
					hr = ::D3DXLoadSurfaceFromResource(surf, 0, 0, NULL, TURI(fname).basename().c_str(), 0, (size() != originalSize() ? D3DX_DEFAULT : D3DX_FILTER_NONE), 0, 0);
				}
			}
			if(!hr) {
				ptr_ = 0;
				PRNN(_T("D3DXCreateTextureFromResourceを使用します"));
				no_scaling_ = false;
				hr = ::D3DXCreateTextureFromResource(device().impl(), NULL, TURI(fname).basename().c_str(), &ptr_);
			}
		}
		if(!hr) GCTP_TRACE(hr<<" / fname:"<<fname);
		if(ptr_) {
			D3DSURFACE_DESC desc;
			hr = ptr_->GetLevelDesc(0, &desc);
			if(!hr) PRNN(hr);
			type_ = NORMAL;
			org_width_ = info.Width;
			org_height_ = info.Height;
			org_format_ = (int)info.Format;
			org_miplevel_ = info.MipLevels;
			//PRNN("Texture "<<fname<<" "<<info<<" Desc {"<<desc.Width<<","<<desc.Height<<","<<desc.Format<<"}");
		}
		else type_ = NONE;
		return hr;
	}

	/// メモリから読みこみ
	HRslt Texture::setUp(const void *memory, size_t size)
	{
		if(!memory || size <= 0) return E_INVALIDARG;
		HRslt hr;
		D3DXIMAGE_INFO info;
		hr = ::D3DXGetImageInfoFromFileInMemory(memory, (UINT)size, &info);
		if(hr) {
			hr = setUp(NORMAL, info.Width, info.Height, info.Format, info.MipLevels);
			if(hr) {
				dx::IDirect3DSurfacePtr surf;
				ptr_->GetSurfaceLevel(0, &surf);
				if(no_scaling_) {
					RECT rect = {0, 0, info.Width, info.Height};
					hr = ::D3DXLoadSurfaceFromFileInMemory(surf, 0, &rect, memory, (UINT)size, 0, D3DX_FILTER_NONE, 0, 0);
				}
				else {
					hr = ::D3DXLoadSurfaceFromFileInMemory(surf, 0, 0, memory, (UINT)size, 0, (this->size() != originalSize() ? D3DX_DEFAULT : D3DX_FILTER_NONE), 0, 0);
				}
			}
			if(!hr) {
				ptr_ = 0;
				PRNN(_T("D3DXCreateTextureFromInMemoryを使用します"));
				no_scaling_ = false;
				hr = ::D3DXCreateTextureFromFileInMemory(device().impl(), memory, (UINT)size, &ptr_);
			}
		}
		if(!hr) GCTP_TRACE(hr<<" / memory");
		if(ptr_) {
			D3DSURFACE_DESC desc;
			hr = ptr_->GetLevelDesc(0, &desc);
			if(!hr) PRNN(hr);
			type_ = NORMAL;
			org_width_ = info.Width;
			org_height_ = info.Height;
			org_format_ = (int)info.Format;
			org_miplevel_ = info.MipLevels;
			//PRNN("Texture memory "<<info<<" Desc {"<<desc.Width<<","<<desc.Height<<","<<desc.Format<<"}");
		}
		else type_ = NONE;
		return hr;
	}

	/// 領域だけ確保
	HRslt Texture::setUp(Type type, int width, int height, int format, int miplevel)
	{
		if(type == NONE || width == 0 || height == 0 || format == 0) return E_INVALIDARG;
		org_width_ = width;
		org_height_ = height;
		org_format_ = format;
		org_miplevel_ = miplevel;
		ptr_ = 0;

		DWORD usage;
		D3DPOOL pool;
		switch(type) {
		case NORMAL:
			usage = 0;
			pool = D3DPOOL_MANAGED;
			break;
		case WORK:
			usage = D3DUSAGE_DYNAMIC;
			pool = D3DPOOL_DEFAULT;
			break;
		case RENDERTARGET:
			usage = D3DUSAGE_RENDERTARGET;
			pool = D3DPOOL_DEFAULT;
			break;
		case OFFSCREEN:
			usage = 0;
			pool = D3DPOOL_SYSTEMMEM;
			break;
		}

		dx::D3DCAPS caps;
		HRslt hr = device().impl()->GetDeviceCaps( &caps );
		if(!hr) {
			GCTP_TRACE(hr);
			return hr;
		}
		if(type == WORK && !(caps.Caps2 & D3DCAPS2_DYNAMICTEXTURES)) return E_FAIL;
		if(caps.TextureCaps & (D3DPTEXTURECAPS_POW2|D3DPTEXTURECAPS_NONPOW2CONDITIONAL)) {
			// これを考慮すべきかどうか…
			// D3DPTEXTURECAPS_POW2 && D3DPTEXTURECAPS_NONPOW2CONDITIONALっていう
			// チップ、ほとんど無いみたい…
		}
		if(caps.TextureCaps & D3DPTEXTURECAPS_POW2) {
			width = 2;
			height = 2;
			while( width < org_width_ ) width <<= 1;
			while( height < org_height_ ) height <<= 1;
		}
		width = min(width, (int)caps.MaxTextureWidth);
		height = min(height, (int)caps.MaxTextureHeight);
		if(caps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY) {
			int sqsize = max(width, height);
			if(sqsize > (int)caps.MaxTextureWidth || sqsize > (int)caps.MaxTextureHeight) {
				sqsize = min(width, height);
			}
			width = height = sqsize;
		}
		//if(!device().impl().checkFormat(usage, D3DRTYPE_TEXTURE, format)) {
		// 非常に面倒だったので、適合する別のカラーフォーマットを探す、はオミットで…
		//}

		hr = device().impl()->CreateTexture(width, height, miplevel, usage, (D3DFORMAT)format, pool, &ptr_, NULL);
		if(!hr && hr != E_OUTOFMEMORY) {
			hr = ::D3DXCreateTexture(device().impl(), width, height, miplevel, usage, (D3DFORMAT)format, pool, &ptr_);
		}
		if(ptr_) {
			type_ = type;
			D3DSURFACE_DESC desc;
			hr = ptr_->GetLevelDesc(0, &desc);
			if(!hr) GCTP_TRACE(hr);
			//PRNN("Texture {"<<org_width_<<","<<org_height_<<","<<(D3DFORMAT)org_format_<<"}->Desc{"<<desc.Width<<","<<desc.Height<<","<<desc.Format<<"}");
		}
		return hr;
	}

	int Texture::format() const
	{
		if(ptr_) {
			D3DSURFACE_DESC desc;
			ptr_->GetLevelDesc(0, &desc);
			return desc.Format;
		}
		return -1;
	}

	Point2 Texture::size() const
	{
		D3DSURFACE_DESC desc;
		if(ptr_) {
			ptr_->GetLevelDesc(0, &desc);
			return Point2C(desc.Width, desc.Height);
		}
		return Point2C(-1,-1);
	}

	HRslt Texture::restore()
	{
		if(type_ == WORK || type_ == RENDERTARGET) return setUp(type_, org_width_, org_height_, org_format_, org_miplevel_);
		return S_OK;
	}

	void Texture::cleanUp()
	{
		if(type_ == WORK || type_ == RENDERTARGET) ptr_ = 0;
	}

	/// システムに設定（要するにSetTexture）
	HRslt Texture::setCurrent(uint index) const
	{
		return device().impl()->SetTexture(index, this ? ptr_ : NULL);
	}

	bool Texture::isCurrent(uint index) const
	{
		dx::IDirect3DBaseTexturePtr _w, _x;
		_x = ptr_;
		device().impl()->GetTexture(index, &_w);
		return _w == _x;
	}

	HRslt Texture::unset(uint index)
	{
		return device().impl()->SetTexture(index, NULL);
	}

	Texture::LockedRect Texture::lock(uint miplevel, const Rect &rc, uint32_t flag)
	{
		D3DLOCKED_RECT lr;
		HRslt hr = ptr_->LockRect(miplevel, &lr, rc, flag);
		if(hr) return LockedRect(lr.Pitch, lr.pBits);
		GCTP_TRACE(hr);
		return LockedRect(0, 0);
	}

	Texture::LockedRect Texture::lock(uint miplevel, uint32_t flag)
	{
		D3DLOCKED_RECT lr;
		HRslt hr = ptr_->LockRect(miplevel, &lr, 0, flag);
		if(hr) return LockedRect(lr.Pitch, lr.pBits);
		GCTP_TRACE(hr);
		return LockedRect(0, 0);
	}

	HRslt Texture::unlock(uint miplevel)
	{
		return ptr_->UnlockRect(miplevel);
	}

}} // namespace gctp
