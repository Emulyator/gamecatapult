/** @file
 * GameCatapult オフスクリーンサーフェス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 3:31:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/graphic.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/graphic/rsrc.hpp>
#include <gctp/graphic/image.hpp>
#include <gctp/extension.hpp>
#include <gctp/buffer.hpp>
#include <gctp/turi.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace graphic {

	namespace {

		template<class E, class T>
		std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, D3DXIMAGE_INFO const & info)
		{
			os<<"{"<<info.Width<<","<<info.Height<<","<<info.Depth<<","<<info.MipLevels<<","<<info.Format
			  <<","<<info.ResourceType<<","<<info.ImageFileFormat<<"}";
			return os;
		}

	}

	GCTP_IMPLEMENT_CLASS_NS2(gctp, graphic, Image, Object);

	/// 画像ファイルから読みこみ
	HRslt Image::setUp(const _TCHAR *fname)
	{
		if(!fname) return E_INVALIDARG;
		HRslt hr;
		D3DXIMAGE_INFO info;
		hr = ::D3DXGetImageInfoFromFile(fname, &info);
		if(hr) {
			hr = setUp(NORMAL, info.Width, info.Height, info.Format);
			if(hr) {
				hr = ::D3DXLoadSurfaceFromFile(ptr_, 0, 0, fname, 0, D3DX_DEFAULT, 0, 0);
			}
		}
		else {
			PRNN(_T("Win32リソース検索 ") << TURI(fname).basename());
			hr = ::D3DXGetImageInfoFromResource(NULL, TURI(fname).basename().c_str(), &info);
			if(!hr) return hr;
			hr = setUp(NORMAL, info.Width, info.Height, info.Format);
			if(hr) {
				hr = ::D3DXLoadSurfaceFromResource(ptr_, 0, 0, NULL, TURI(fname).basename().c_str(), 0, D3DX_DEFAULT, 0, 0);
			}
		}
		if(!hr) GCTP_TRACE(hr<<" / fname:"<<fname);
		if(ptr_) {
			D3DSURFACE_DESC desc;
			hr = ptr_->GetDesc(&desc);
			if(!hr) PRNN(hr);
			type_ = NORMAL;
			org_width_ = info.Width;
			org_height_ = info.Height;
			org_format_ = (int)info.Format;
			//PRNN("Image "<<fname<<" "<<info<<" Desc {"<<desc.Width<<","<<desc.Height<<","<<desc.Format<<"}");
		}
		else type_ = NONE;
		return hr;
	}

	/// メモリから読みこみ
	HRslt Image::setUp(const void *memory, size_t size)
	{
		if(!memory || size <= 0) return E_INVALIDARG;
		HRslt hr;
		D3DXIMAGE_INFO info;
		hr = ::D3DXGetImageInfoFromFileInMemory(memory, (UINT)size, &info);
		if(hr) {
			hr = setUp(NORMAL, info.Width, info.Height, info.Format);
			if(hr) {
				hr = ::D3DXLoadSurfaceFromFileInMemory(ptr_, 0, 0, memory, (UINT)size, 0, D3DX_DEFAULT, 0, 0);
			}
		}
		if(!hr) GCTP_TRACE(hr<<" / memory");
		if(ptr_) {
			D3DSURFACE_DESC desc;
			hr = ptr_->GetDesc(&desc);
			if(!hr) PRNN(hr);
			type_ = NORMAL;
			org_width_ = info.Width;
			org_height_ = info.Height;
			org_format_ = (int)info.Format;
			//PRNN("Image memory "<<info<<" Desc {"<<desc.Width<<","<<desc.Height<<","<<desc.Format<<"}");
		}
		else type_ = NONE;
		return hr;
	}

	/// 領域だけ確保
	HRslt Image::setUp(Type type, int width, int height, int format)
	{
		if(type == NONE || width == 0 || height == 0 || format == 0) return E_INVALIDARG;
		ptr_ = 0;

		D3DPOOL pool;
		switch(type) {
		case NORMAL:
			pool = D3DPOOL_SCRATCH;
			break;
		case WORK:
			pool = D3DPOOL_DEFAULT;
			break;
		}
		HRslt hr = device().impl()->CreateOffscreenPlainSurface(width, height, (D3DFORMAT)format, pool, &ptr_, NULL);
		if(!hr) GCTP_TRACE(hr);
		if(ptr_) {
			type_ = type;
			org_width_ = width;
			org_height_ = height;
			org_format_ = format;
			D3DSURFACE_DESC desc;
			hr = ptr_->GetDesc(&desc);
			if(!hr) GCTP_TRACE(hr);
			//PRNN("Image {"<<desc.Width<<","<<desc.Height<<","<<desc.Format<<"}");
		}
		return hr;
	}

	/// 画像ファイルとして保存
	HRslt Image::save(const _TCHAR *fname)
	{
		gctp::TURI save_file(fname);
		std::basic_string<_TCHAR> ext = save_file.extension();
		D3DXIMAGE_FILEFORMAT format = D3DXIFF_BMP;
		if(ext == _T("jpg")) format = D3DXIFF_JPG;
		else if(ext == _T("tga")) format = D3DXIFF_TGA;
		else if(ext == _T("png")) format = D3DXIFF_PNG;
		else if(ext == _T("dds")) format = D3DXIFF_DDS;
		else if(ext == _T("ppm")) format = D3DXIFF_PPM;
		else if(ext == _T("dib")) format = D3DXIFF_DIB;
		else if(ext == _T("hdr")) format = D3DXIFF_HDR;
		else if(ext == _T("pfm")) format = D3DXIFF_PFM;
		return D3DXSaveSurfaceToFile(save_file.raw().c_str(), format, ptr_, NULL, NULL);
	}

	int Image::format() const
	{
		if(ptr_) {
			D3DSURFACE_DESC desc;
			ptr_->GetDesc(&desc);
			return desc.Format;
		}
		return -1;
	}

	Point2 Image::size() const
	{
		if(ptr_) {
			D3DSURFACE_DESC desc;
			ptr_->GetDesc(&desc);
			return Point2C(desc.Width, desc.Height);
		}
		return Point2C(-1,-1);
	}

	HRslt Image::restore()
	{
		if(type_ == WORK) return setUp(type_, org_width_, org_height_, org_format_);
		return S_OK;
	}

	void Image::cleanUp()
	{
		if(type_ == WORK) ptr_ = 0;
	}

	Image::LockedRect Image::lock(const Rect &rc, uint32_t flag)
	{
		D3DLOCKED_RECT lr;
		HRslt hr = ptr_->LockRect(&lr, rc, flag);
		if(hr) return LockedRect(lr.Pitch, lr.pBits);
		GCTP_TRACE(hr);
		return LockedRect(0, 0);
	}

	Image::LockedRect Image::lock(uint32_t flag)
	{
		D3DLOCKED_RECT lr;
		HRslt hr = ptr_->LockRect(&lr, 0, flag);
		if(hr) return LockedRect(lr.Pitch, lr.pBits);
		GCTP_TRACE(hr);
		return LockedRect(0, 0);
	}

	HRslt Image::unlock()
	{
		return ptr_->UnlockRect();
	}

}} // namespace gctp
