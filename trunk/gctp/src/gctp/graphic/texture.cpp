/** @file
 * GameCatapult �e�N�X�`���N���X
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
#include <gctp/uri.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace graphic {

	namespace {

		Ptr realizeTexture(const char *name)
		{
			return createOnDB<Texture>(name);
		}

#define __REGISTER(ext) Extension ext##_realizer(#ext, realizeTexture)
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

	GCTP_IMPLEMENT_CLASS_NS(gctp, Texture, Object);

	/// �摜�t�@�C������ǂ݂���
	HRslt Texture::setUp(const char *fname)
	{
		HRslt hr;
		D3DXIMAGE_INFO info;
		hr = D3DXGetImageInfoFromFile(fname, &info);
		if(!hr) GCTP_TRACE(hr);
		hr = D3DXCreateTextureFromFile(device().impl(), fname, &ptr_);
		// ���ꂪ�A�}���`�X���b�h�i�񓯊��ǂݍ��݂ŁA�ʃX���b�h�ŕ`�悪�i�s���A���̊֐��̎g�p�͂��̃X���b�h�̂݁j
		// ���ƂȂ���E_OUTOFMEMORY��Ԃ����Ƃ��p������B
		// ��̂ǂ��Ń��b�N����΂����񂾁H
		if(hr == E_OUTOFMEMORY) { // �c�߂�ǂ���łR�񎎍s���邱�Ƃɂ���c
			::Sleep(1);
			hr = D3DXCreateTextureFromFile(device().impl(), fname, &ptr_);
		}
		if(hr == E_OUTOFMEMORY) { // �c�߂�ǂ���łR�񎎍s���邱�Ƃɂ���c
			::Sleep(1);
			hr = D3DXCreateTextureFromFile(device().impl(), fname, &ptr_);
		}
		if(hr == E_OUTOFMEMORY) { // �c�߂�ǂ���łR�񎎍s���邱�Ƃɂ���c
			::Sleep(1);
			hr = D3DXCreateTextureFromFile(device().impl(), fname, &ptr_);
		}
		// �e�N�X�`���ǂݍ��݊֐����炢���삵���ق����������c�i�ꉞ�A���̑Ώ��㌻�ۂ͊m�F�ł��Ȃ����j
		if(!hr && hr != E_OUTOFMEMORY) { // �����A���\�[�X������
			PRNN("Win32���\�[�X���� " << URI(fname).basename());
			hr = D3DXGetImageInfoFromResource(NULL, URI(fname).basename().c_str(), &info);
			if(!hr) GCTP_TRACE(hr);
			hr = D3DXCreateTextureFromResource(device().impl(), NULL, URI(fname).basename().c_str(), &ptr_);
			// ���ꂪ�A�}���`�X���b�h�i�񓯊��ǂݍ��݂ŁA�ʃX���b�h�ŕ`�悪�i�s���A���̊֐��̎g�p�͂��̃X���b�h�̂݁j
			// ���ƂȂ���E_OUTOFMEMORY��Ԃ����Ƃ��p������B
			// ��̂ǂ��Ń��b�N����΂����񂾁H
			if(hr == E_OUTOFMEMORY) { // �c�߂�ǂ���łR�񎎍s���邱�Ƃɂ���c
				::Sleep(1);
				hr = D3DXCreateTextureFromResource(device().impl(), NULL, URI(fname).basename().c_str(), &ptr_);
			}
			if(hr == E_OUTOFMEMORY) { // �c�߂�ǂ���łR�񎎍s���邱�Ƃɂ���c
				::Sleep(1);
				hr = D3DXCreateTextureFromResource(device().impl(), NULL, URI(fname).basename().c_str(), &ptr_);
			}
			if(hr == E_OUTOFMEMORY) { // �c�߂�ǂ���łR�񎎍s���邱�Ƃɂ���c
				::Sleep(1);
				hr = D3DXCreateTextureFromResource(device().impl(), NULL, URI(fname).basename().c_str(), &ptr_);
			}
		}
		if(!hr) GCTP_TRACE(hr<<" / fname:"<<fname);
		else {
			D3DSURFACE_DESC desc;
			hr = ptr_->GetLevelDesc(0, &desc);
			if(!hr) PRNN(hr);
			name_ = fname;
			org_width_ = info.Width;
			org_height_ = info.Height;
			PRNN("Image "<<name_<<" "<<info<<" Desc {"<<desc.Width<<","<<desc.Height<<"}");
		}
		return hr;
	}

	/// �̈悾���m��
	HRslt Texture::setUp(int _width, int _height, int _format)
	{
		HRslt hr = D3DXCreateTexture(device().impl(), _width, _height, 1, 0, (D3DFORMAT)_format, D3DPOOL_MANAGED, &ptr_);
		name_.clear();
		org_width_ = _width;
		org_height_ = _height;
		if(!hr) PRNN(hr);
		else {
			D3DSURFACE_DESC desc;
			hr = ptr_->GetLevelDesc(0, &desc);
			if(!hr) PRNN(hr);
			PRNN("Desc{"<<desc.Width<<","<<desc.Height<<"}");
		}
		return hr;
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

#if 0
	HRslt Texture::restore()
	{
		//if(!name_.empty()) setUp(name_.c_str());
		return S_OK;
	}

	void Texture::cleanUp()
	{
		//if(!name_.empty()) ptr_ = 0;
	}
#endif
	/// �V�X�e���ɐݒ�i�v�����SetTexture�j
	HRslt Texture::setCurrent(uint index) const
	{
		return device().impl()->SetTexture(index, this ? ptr_ : NULL);
	}

	bool Texture::isCurrent(uint index) const
	{
		dx::IDirect3DBaseTexture9Ptr _w, _x;
		_x = ptr_;
		device().impl()->GetTexture(index, &_w);
		return _w == _x;
	}

	HRslt Texture::unSet(uint index)
	{
		return device().impl()->SetTexture(index, NULL);
	}

	Texture::LockedRect Texture::lock(uint miplevel, const Rect &rc, uint32_t flag)
	{
		D3DLOCKED_RECT lr;
		if(HRslt(ptr_->LockRect(miplevel, &lr, rc, flag))) {
			return LockedRect(lr.Pitch, lr.pBits);
		}
		return LockedRect(0, 0);
	}

	Texture::LockedRect Texture::lock(uint miplevel, uint32_t flag)
	{
		D3DLOCKED_RECT lr;
		if(HRslt(ptr_->LockRect(miplevel, &lr, 0, flag))) {
			return LockedRect(lr.Pitch, lr.pBits);
		}
		return LockedRect(0, 0);
	}

	HRslt Texture::unlock(uint miplevel)
	{
		return ptr_->UnlockRect(miplevel);
	}

}} // namespace gctp
