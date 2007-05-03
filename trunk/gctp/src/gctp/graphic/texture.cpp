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
#include <gctp/turi.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace graphic {

	namespace {

		Ptr realizeTexture(const _TCHAR *name)
		{
			return createOnDB<Texture>(name);
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

	GCTP_IMPLEMENT_CLASS_NS(gctp, Texture, Object);

	Texture::Texture() : dynamic_(false)
	{
	}

	/// �摜�t�@�C������ǂ݂���
	HRslt Texture::setUp(const _TCHAR *fname)
	{
		HRslt hr;
		D3DXIMAGE_INFO info;
		hr = D3DXGetImageInfoFromFile(fname, &info);
		if(!hr) GCTP_TRACE(hr);
		hr = D3DXCreateTextureFromFile(device().impl(), fname, &ptr_);
		dynamic_ = false;
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
			PRNN(_T("Win32���\�[�X���� ") << TURI(fname).basename());
			hr = D3DXGetImageInfoFromResource(NULL, TURI(fname).basename().c_str(), &info);
			if(!hr) GCTP_TRACE(hr);
			hr = D3DXCreateTextureFromResource(device().impl(), NULL, TURI(fname).basename().c_str(), &ptr_);
			// ���ꂪ�A�}���`�X���b�h�i�񓯊��ǂݍ��݂ŁA�ʃX���b�h�ŕ`�悪�i�s���A���̊֐��̎g�p�͂��̃X���b�h�̂݁j
			// ���ƂȂ���E_OUTOFMEMORY��Ԃ����Ƃ��p������B
			// ��̂ǂ��Ń��b�N����΂����񂾁H
			if(hr == E_OUTOFMEMORY) { // �c�߂�ǂ���łR�񎎍s���邱�Ƃɂ���c
				::Sleep(1);
				hr = D3DXCreateTextureFromResource(device().impl(), NULL, TURI(fname).basename().c_str(), &ptr_);
			}
			if(hr == E_OUTOFMEMORY) { // �c�߂�ǂ���łR�񎎍s���邱�Ƃɂ���c
				::Sleep(1);
				hr = D3DXCreateTextureFromResource(device().impl(), NULL, TURI(fname).basename().c_str(), &ptr_);
			}
			if(hr == E_OUTOFMEMORY) { // �c�߂�ǂ���łR�񎎍s���邱�Ƃɂ���c
				::Sleep(1);
				hr = D3DXCreateTextureFromResource(device().impl(), NULL, TURI(fname).basename().c_str(), &ptr_);
			}
		}
		if(!hr) GCTP_TRACE(hr<<" / fname:"<<fname);
		else {
			D3DSURFACE_DESC desc;
			hr = ptr_->GetLevelDesc(0, &desc);
			if(!hr) PRNN(hr);
			org_width_ = info.Width;
			org_height_ = info.Height;
			org_format_ = (int)info.Format;
			PRNN("Image "<<fname<<" "<<info<<" Desc {"<<desc.Width<<","<<desc.Height<<"}");
		}
		return hr;
	}

	/// ����������ǂ݂���
	HRslt Texture::setUp(const void *memory, size_t size)
	{
		HRslt hr;
		D3DXIMAGE_INFO info;
		hr = D3DXGetImageInfoFromFileInMemory(memory, (UINT)size, &info);
		if(!hr) GCTP_TRACE(hr);
		hr = D3DXCreateTextureFromFileInMemory(device().impl(), memory, (UINT)size, &ptr_);
		dynamic_ = false;
		if(hr) {
			D3DSURFACE_DESC desc;
			hr = ptr_->GetLevelDesc(0, &desc);
			if(!hr) PRNN(hr);
			org_width_ = info.Width;
			org_height_ = info.Height;
			org_format_ = (int)info.Format;
			PRNN("Image memory "<<info<<" Desc {"<<desc.Width<<","<<desc.Height<<"}");
		}
		return hr;
	}

	/// �̈悾���m��
	HRslt Texture::setUp(int width, int height, int format, int miplevel, bool dynamic)
	{
		org_width_ = width;
		org_height_ = height;
		org_format_ = format;

		HRslt hr = D3DXCreateTexture(device().impl(), width, height, miplevel, dynamic ? D3DUSAGE_DYNAMIC : 0, (D3DFORMAT)format, dynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &ptr_);
		// D3DXCreateTexture�͎g���ׂ�����Ȃ��̂���
		dynamic_ = false;
		if(!hr) PRNN(hr);
		else {
			D3DSURFACE_DESC desc;
			hr = ptr_->GetLevelDesc(0, &desc);
			if(!hr) PRNN(hr);
			// ���ɂ���ẮAD3DXCreateTexture�͗v���T�C�Y��菬�����l�𕽋C�ŕԂ��݂���
			// ����Ď��O��
			if((DWORD)width < desc.Width || (DWORD)height < desc.Height) {
				ptr_ = 0;

				int real_width, real_height;
				dx::D3DCAPS caps;
				ZeroMemory( &caps, sizeof(dx::D3DCAPS));
				hr = device().impl()->GetDeviceCaps( &caps );
				if(dynamic && !(caps.Caps2 & D3DCAPS2_DYNAMICTEXTURES)) return E_FAIL;
				if(caps.TextureCaps & D3DPTEXTURECAPS_POW2) {
					real_width = 2;
					real_height = 2;
					while( real_width < width ) real_width = real_width << 1;
					while( real_height < height ) real_height = real_height << 1;
				}
				else {
					real_width = width;
					real_height = height;
				}
				hr = device().impl()->CreateTexture(real_width, real_height, miplevel, dynamic ? D3DUSAGE_DYNAMIC : 0, (D3DFORMAT)format, dynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &ptr_, NULL);
				hr = ptr_->GetLevelDesc(0, &desc);
				if(!hr) PRNN(hr);
			}
			dynamic_ = dynamic;
			PRNN("Texture {"<<width<<","<<height<<"}->Desc{"<<desc.Width<<","<<desc.Height<<"}");
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

	HRslt Texture::restore()
	{
		if(dynamic_) return setUp(org_width_, org_height_, org_format_, dynamic_);
		return S_OK;
	}

	void Texture::cleanUp()
	{
		if(dynamic_) ptr_ = 0;
	}

	/// �V�X�e���ɐݒ�i�v�����SetTexture�j
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
