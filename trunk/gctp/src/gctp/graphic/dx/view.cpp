/**@file
 * GameCatapult DirectX 追加スワップチェーンクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/25 21:48:07
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/graphic/dx/view.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace graphic { namespace dx {

	HRslt View::setUp(IDirect3DSwapChain9Ptr ptr)
	{
		ptr_ = ptr;
		ptr_->GetDevice(&device_);
		ptr_->GetPresentParameters(&pp_);
		idx_ = -1;
		return S_OK;
	}

	HRslt View::setUp(IDirect3DDevice9Ptr dev, uint i)
	{
		device_ = dev;
		idx_ = i;
		return device_->GetSwapChain(idx_, &ptr_);
	}

	void View::cleanUp()
	{
		ptr_ = 0;
	}

	HRslt View::restore()
	{
		if(idx_ == -1) return device_->CreateAdditionalSwapChain(&pp_, &ptr_);
		else device_->GetSwapChain(idx_, &ptr_);
		return S_OK;
	}

	HRslt View::setCurrent() const
	{
		if( !this || !ptr_ || !device_ ) return E_FAIL;
		IDirect3DSurface9Ptr surf;
		HRslt hr = ptr_->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &surf);
		if(!hr) return hr;
		return device_->SetRenderTarget(0, surf);
	}

	HRslt View::present() const
	{
		if( !this || !ptr_ ) return E_FAIL;
		return ptr_->Present(NULL, NULL, NULL, NULL, 0);
	}

}}} // namespace gctp
