/** @file
 * GameCatapult 頂点バッファクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 3:18:53
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/graphic.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/graphic/vertexbuffer.hpp>
#include <gctp/dbgout.hpp>
#include <assert.h>

using namespace std;

namespace gctp { namespace graphic {

	/** 頂点バッファ制作
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 4:33:42
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt VertexBuffer::_setUp(Type type, dx::FVF fvf, uint size, uint minsize)
	{
		HRslt hr;
		ulong usage = (type == DYNAMIC) ? D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC : 0;
		D3DPOOL memclass = (type == DYNAMIC) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
		while(size > minsize &&
			!(hr = device().impl()->CreateVertexBuffer(size, usage, fvf.val, memclass, &ptr_, NULL)) &&
			hr != D3DERR_INVALIDCALL
			 ) {
			size /= 2;
		}
		type_ = type; size_ = size; fvf_ = fvf;
		used_ = 0;
		if(!hr) PRNN(hr);
		return hr;
	}

	/** ＦＶＦ頂点バッファ制作
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 4:33:42
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt VertexBuffer::setUp(Type type, dx::FVF fvf, uint num)
	{
		return _setUp(type, fvf, num*fvf.stride(), fvf.stride()*2);
	}

	/** 非ＦＶＦ頂点バッファ制作
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 4:33:42
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt VertexBuffer::setUp(Type type, uint size)
	{
		return _setUp(type, 0, size, 32);
	}

	HRslt VertexBuffer::restore()
	{
		if(type_==DYNAMIC) {
			if(fvf_.val == 0) return _setUp(type_, 0, size_, 32);
			else return _setUp(type_, fvf_, size_, fvf_.stride()*2);
		}
		return S_OK;
	}

	void VertexBuffer::cleanUp()
	{
		if(type_==DYNAMIC) ptr_ = 0;
	}

	void *VertexBuffer::lock()
	{
		return lock(used_*fvf_.stride(), 0);
	}

	void *VertexBuffer::lock(uint offset, uint length)
	{
		if(!ptr_) return NULL;
		HRslt hr;
		void *ret;
		if(type_==DYNAMIC) {
			if(offset==0) hr = ptr_->Lock(offset, length, &ret, D3DLOCK_DISCARD);
			else hr = ptr_->Lock(offset, length, &ret, D3DLOCK_NOOVERWRITE);
		}
		else hr = ptr_->Lock(offset, length, &ret, 0);
		if(!hr) return NULL;
		return ret;
	}

	const void *VertexBuffer::lock(uint offset, uint length) const
	{
		if(!ptr_) return NULL;
		HRslt hr;
		void *ret;
		hr = ptr_->Lock(offset, length, &ret, D3DLOCK_READONLY);
		if(!hr) return NULL;
		return ret;
	}

	void VertexBuffer::unlock() const
	{
		ptr_->Unlock();
	}

	/** 描画（プリミティブ描画命令発行）
	 *
	 * 頂点フォーマットとストリームソースは設定するが、頂点シェーダーの設定は変更しないことに注意
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 5:19:45
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt VertexBuffer::draw(uint index, D3DPRIMITIVETYPE prim_type, uint num, uint offset)
	{
		assert(fvf_.val!=0);
		if(!ptr_) return E_POINTER;
		HRslt hr;
		hr = device().impl()->SetFVF( fvf_.val );
		if(hr) {
			hr = setCurrent(index);
			if(hr) {
				hr = device().impl()->DrawPrimitive( prim_type, offset, num );
				if(!hr) PRNN(hr);
			}
		}
		return hr;
	}

	/** 描画（プリミティブ描画命令発行）
	 *
	 * 非ＦＶＦモード用
	 * 頂点フォーマットとストリームソースは設定するが、頂点シェーダーの設定は変更しないことに注意
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 5:19:45
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt VertexBuffer::draw(dx::FVF fvf, uint index, D3DPRIMITIVETYPE prim_type, uint num, uint offset)
	{
		assert(fvf.val==0);
		if(!ptr_) return E_POINTER;
		HRslt hr;
		hr = device().impl()->SetFVF( fvf.val );
		if(hr) {
			hr = setCurrent(index);
			if(hr) {
				hr = device().impl()->DrawPrimitive( prim_type, offset, num );
				if(!hr) PRNN(hr);
			}
		}
		return hr;
	}

	HRslt VertexBuffer::draw(dx::FVF fvf, uint index, D3DPRIMITIVETYPE prim_type, uint num)
	{
		HRslt ret = draw(fvf, index, prim_type, num, used_);
		switch(prim_type) {
		case D3DPT_POINTLIST:
			used_ += num;
			break;
		case D3DPT_LINELIST:
			used_ += num*2;
			break;
		case D3DPT_LINESTRIP:
			used_ += num+1;
			break;
		case D3DPT_TRIANGLELIST:
			used_ += num*3;
			break;
		case D3DPT_TRIANGLESTRIP:
		case D3DPT_TRIANGLEFAN:
			used_ += num+2;
			break;
		}
		return ret;
	}

	HRslt VertexBuffer::draw(uint index, D3DPRIMITIVETYPE prim_type, uint num)
	{
		HRslt ret = draw(index, prim_type, num, used_);
		switch(prim_type) {
		case D3DPT_POINTLIST:
			used_ += num;
			break;
		case D3DPT_LINELIST:
			used_ += num*2;
			break;
		case D3DPT_LINESTRIP:
			used_ += num+1;
			break;
		case D3DPT_TRIANGLELIST:
			used_ += num*3;
			break;
		case D3DPT_TRIANGLESTRIP:
		case D3DPT_TRIANGLEFAN:
			used_ += num+2;
			break;
		}
		return ret;
	}

	HRslt VertexBuffer::setCurrent(uint index) const
	{
		if(!ptr_) return E_POINTER;
		HRslt hr;
		hr = device().impl()->SetStreamSource(index, ptr_, 0, fvf_.stride());
		if(!hr) PRNN(hr);
		return hr;
	}

	uint VertexBuffer::size() const
	{
		/*
		 * size_の値を使わないのは、size_はユーザーが要求したサイズであって、実際に確保されたサイズではないから
		 * デバイス復帰時の再製作のために保持している。
		 */
		D3DVERTEXBUFFER_DESC desc;
		ptr_->GetDesc(&desc);
		return desc.Size;
	}

	uint VertexBuffer::freesize() const
	{
		uint _size = size(), _used = used_*fvf_.stride();
		if(_size>_used) return _size-_used;
		return 0;
	}

	uint VertexBuffer::freenum() const
	{
		if(fvf_.val != 0) return freesize()/fvf_.stride();
		return -1;
	}

}} // namespace gctp
