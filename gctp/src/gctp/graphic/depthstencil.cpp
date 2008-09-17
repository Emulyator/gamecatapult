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
#include <gctp/graphic/depthstencil.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace graphic {

	GCTP_IMPLEMENT_CLASS_NS2(gctp, graphic, DepthStencil, Object);

	DepthStencil::DepthStencil() : org_format_(-1)
	{
	}

	/// 領域確保
	HRslt DepthStencil::setUp(int width, int height, int format, bool discardable)
	{
		org_width_ = width;
		org_height_ = height;
		org_format_ = format;
		discardable_ = discardable;
	
		HRslt hr;
		hr = device().impl()->CreateDepthStencilSurface(width, height, (D3DFORMAT)format, D3DMULTISAMPLE_NONE, 0, discardable?TRUE:FALSE, &ptr_, NULL);
		if(!hr) GCTP_TRACE(hr);
		return hr;
	}

	Point2 DepthStencil::size() const
	{
		D3DSURFACE_DESC desc;
		if(ptr_) {
			ptr_->GetDesc(&desc);
			return Point2C(desc.Width, desc.Height);
		}
		return Point2C(-1,-1);
	}

	HRslt DepthStencil::restore()
	{
		if(org_format_ > 0) return device().impl()->CreateDepthStencilSurface(org_width_, org_height_, (D3DFORMAT)org_format_, D3DMULTISAMPLE_NONE, 0, discardable_?TRUE:FALSE, &ptr_, NULL);
		else return S_OK;
	}

	void DepthStencil::cleanUp()
	{
		backup_ = 0;
		ptr_ = 0;
	}

	/// システムに設定（要するにSetDepthStencilSurface）
	HRslt DepthStencil::begin(bool stencil)
	{
		backup_ = 0;
		device().impl()->GetDepthStencilSurface(&backup_);
		HRslt hr = device().impl()->SetDepthStencilSurface(ptr_);
		if(stencil) device().impl()->SetRenderState(D3DRS_STENCILENABLE, TRUE);
		return hr;
	}

	HRslt DepthStencil::end()
	{
		HRslt ret = device().impl()->SetDepthStencilSurface(backup_);
		device().impl()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
		backup_ = 0;
		return ret;
	}

	DepthStencil::LockedRect DepthStencil::lock(const Rect &rc, uint32_t flag)
	{
		D3DLOCKED_RECT lr;
		if(HRslt(ptr_->LockRect(&lr, rc, flag))) {
			return LockedRect(lr.Pitch, lr.pBits);
		}
		return LockedRect(0, 0);
	}

	DepthStencil::LockedRect DepthStencil::lock(uint32_t flag)
	{
		D3DLOCKED_RECT lr;
		if(HRslt(ptr_->LockRect(&lr, 0, flag))) {
			return LockedRect(lr.Pitch, lr.pBits);
		}
		return LockedRect(0, 0);
	}

	HRslt DepthStencil::unlock()
	{
		return ptr_->UnlockRect();
	}

}} // namespace gctp
