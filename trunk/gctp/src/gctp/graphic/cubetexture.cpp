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
#include <gctp/graphic/cubetexture.hpp>
#include <gctp/extension.hpp>
#include <gctp/buffer.hpp>
#include <gctp/turi.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace graphic {

	namespace {

		Ptr realizeTexture(Ptr self, BufferPtr buffer)
		{
			Pointer<CubeTexture> ret;
			if(self) ret = self;
			else ret = new CubeTexture;
			if(ret && buffer) {
				if(ret->setUp(buffer->buf(), buffer->size())) {
					return ret;
				}
				return Ptr();
			}
			return ret;
		}

		Extension extcube_dds_realizer(_T("cube.dds"), realizeTexture);

		template<class E, class T>
		std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, D3DXIMAGE_INFO const & info)
		{
			os<<"{"<<info.Width<<","<<info.Height<<","<<info.Depth<<","<<info.MipLevels<<","<<info.Format
			  <<","<<info.ResourceType<<","<<info.ImageFileFormat<<"}";
			return os;
		}
	}

	GCTP_IMPLEMENT_CLASS_NS2(gctp, graphic, CubeTexture, Object);

	CubeTexture::CubeTexture() : type_(NONE)
	{
	}

	/// 画像ファイルから読みこみ
	HRslt CubeTexture::setUp(const _TCHAR *fname)
	{
		if(!fname) return E_INVALIDARG;
		HRslt hr;
		hr = ::D3DXCreateCubeTextureFromFile(device().impl(), fname, &ptr_);
		int retry_count = 0;
		while(hr == E_OUTOFMEMORY && ++retry_count < 100) { // …めんどいんで100回試行することにする…
			::Sleep(1);
			hr = ::D3DXCreateCubeTextureFromFile(device().impl(), fname, &ptr_);
		}
		if(!hr) {
			PRNN(_T("Win32リソース検索 ") << TURI(fname).basename());
			hr = ::D3DXCreateCubeTextureFromResource(device().impl(), NULL, TURI(fname).basename().c_str(), &ptr_);
			int retry_count = 0;
			while(hr == E_OUTOFMEMORY && ++retry_count < 100) { // …めんどいんで100回試行することにする…
				::Sleep(1);
				hr = ::D3DXCreateCubeTextureFromResource(device().impl(), NULL, TURI(fname).basename().c_str(), &ptr_);
			}
		}
		if(!hr) GCTP_ERRORINFO(hr<<" / fname:"<<fname);
		if(ptr_) {
			D3DSURFACE_DESC desc;
			hr = ptr_->GetLevelDesc(0, &desc);
			if(!hr) PRNN(hr);
			type_ = NORMAL;
			size_ = desc.Height;
			format_ = (int)desc.Format;
			miplevel_ = ptr_->GetLevelCount();
			//PRNN("CubeTexture "<<fname<<" "<<info<<" Desc {"<<desc.Width<<","<<desc.Height<<","<<desc.Format<<"}");
		}
		else type_ = NONE;
		return hr;
	}

	/// メモリから読みこみ
	HRslt CubeTexture::setUp(const void *memory, size_t size)
	{
		if(!memory || size <= 0) return E_INVALIDARG;
		HRslt hr;
		hr = ::D3DXCreateCubeTextureFromFileInMemory(device().impl(), memory, (UINT)size, &ptr_);
		if(!hr) GCTP_ERRORINFO(hr<<" / memory");
		if(ptr_) {
			D3DSURFACE_DESC desc;
			hr = ptr_->GetLevelDesc(0, &desc);
			if(!hr) PRNN(hr);
			type_ = NORMAL;
			size_ = desc.Height;
			format_ = (int)desc.Format;
			miplevel_ = ptr_->GetLevelCount();
			//PRNN("Texture memory "<<info<<" Desc {"<<desc.Width<<","<<desc.Height<<","<<desc.Format<<"}");
		}
		else type_ = NONE;
		return hr;
	}

	/// 領域だけ確保
	HRslt CubeTexture::setUp(Type type, int size, int format, int miplevel)
	{
		if(type == NONE || size == 0 || format == 0) return E_INVALIDARG;
		size_ = size;
		format_ = format;
		miplevel_ = miplevel;
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
			size_ = 2;
			while( size_ < size ) size <<= 1;
		}
		size_ = min(size_, (int)caps.MaxTextureHeight);
		//if(!device().impl().checkFormat(usage, D3DRTYPE_TEXTURE, format)) {
		// 非常に面倒だったので、適合する別のカラーフォーマットを探す、はオミットで…
		//}

		hr = ::D3DXCreateCubeTexture(device().impl(), size_, miplevel_, usage, (D3DFORMAT)format_, pool, &ptr_);
		if(!hr) GCTP_ERRORINFO(hr);
		if(ptr_) {
			type_ = type;
			D3DSURFACE_DESC desc;
			hr = ptr_->GetLevelDesc(0, &desc);
			if(hr) {
				size_ = desc.Height;
				format_ = desc.Format;
				miplevel_ = ptr_->GetLevelCount();
			}
			else GCTP_ERRORINFO(hr);

			//PRNN("CubeTexture {"<<org_width_<<","<<org_height_<<","<<(D3DFORMAT)org_format_<<"}->Desc{"<<desc.Width<<","<<desc.Height<<","<<desc.Format<<"}");
		}
		return hr;
	}

	int CubeTexture::format() const
	{
		if(ptr_) {
			D3DSURFACE_DESC desc;
			ptr_->GetLevelDesc(0, &desc);
			return desc.Format;
		}
		return -1;
	}

	Point2 CubeTexture::size() const
	{
		D3DSURFACE_DESC desc;
		if(ptr_) {
			ptr_->GetLevelDesc(0, &desc);
			return Point2C(desc.Width, desc.Height);
		}
		return Point2C(-1,-1);
	}

	HRslt CubeTexture::restore()
	{
		if(type_ == WORK || type_ == RENDERTARGET) return setUp(type_, size_, format_, miplevel_);
		return S_OK;
	}

	void CubeTexture::cleanUp()
	{
		if(type_ == WORK || type_ == RENDERTARGET) ptr_ = 0;
	}

	/// システムに設定（要するにSetTexture）
	HRslt CubeTexture::setCurrent(uint index) const
	{
		return device().impl()->SetTexture(index, this ? ptr_ : NULL);
	}

	bool CubeTexture::isCurrent(uint index) const
	{
		dx::IDirect3DBaseTexturePtr _w, _x;
		_x = ptr_;
		device().impl()->GetTexture(index, &_w);
		return _w == _x;
	}

	HRslt CubeTexture::unset(uint index)
	{
		return device().impl()->SetTexture(index, NULL);
	}

	CubeTexture::LockedRect CubeTexture::lock(Face face, uint miplevel, const Rect &rc, uint32_t flag)
	{
		D3DLOCKED_RECT lr;
		HRslt hr = ptr_->LockRect((D3DCUBEMAP_FACES)face, miplevel, &lr, rc, flag);
		if(hr) return LockedRect(lr.Pitch, lr.pBits);
		GCTP_TRACE(hr);
		return LockedRect(0, 0);
	}

	CubeTexture::LockedRect CubeTexture::lock(Face face, uint miplevel, uint32_t flag)
	{
		D3DLOCKED_RECT lr;
		HRslt hr = ptr_->LockRect((D3DCUBEMAP_FACES)face, miplevel, &lr, 0, flag);
		if(hr) return LockedRect(lr.Pitch, lr.pBits);
		GCTP_TRACE(hr);
		return LockedRect(0, 0);
	}

	HRslt CubeTexture::unlock(Face face, uint miplevel)
	{
		return ptr_->UnlockRect((D3DCUBEMAP_FACES)face, miplevel);
	}

}} // namespace gctp
