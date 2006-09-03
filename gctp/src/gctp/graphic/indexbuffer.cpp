/** @file
 * GameCatapult �C���f�b�N�X�o�b�t�@�N���X
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 3:18:53
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/graphic.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/graphic/indexbuffer.hpp>
#include <gctp/graphic/vertexbuffer.hpp>
#include <gctp/dbgout.hpp>
#include <assert.h>

using namespace std;

namespace gctp { namespace graphic {

	/** �C���f�b�N�X�o�b�t�@����
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 4:33:42
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt IndexBuffer::setUp(Type type, Bits bits, uint num)
	{
		HRslt hr;
		uint minsize = 3;
		uint size = (bits == SHORT) ? num*2 : num*4;
		ulong usage = (type == DYNAMIC) ? D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC : 0;
		D3DFORMAT format = (bits == SHORT) ? D3DFMT_INDEX16 : D3DFMT_INDEX32;
		D3DPOOL memclass = (type == DYNAMIC) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
		while(size > minsize &&
			!(hr = device().impl()->CreateIndexBuffer(size, usage, format, memclass, &ptr_, NULL)) &&
			hr != D3DERR_INVALIDCALL
			 ) {
			size /= 2;
		}
		type_ = type; bits_ = bits; num_ = num;
		if(!hr) PRNN(hr);
		return hr;
	}

	HRslt IndexBuffer::restore()
	{
		if(type_==DYNAMIC) {
			return setUp(type_, bits_, num_);
		}
		return S_OK;
	}

	void IndexBuffer::cleanUp()
	{
		if(type_==DYNAMIC) ptr_ = 0;
	}

	void *IndexBuffer::lock(uint offset, uint length)
	{
		if(!ptr_) return NULL;
		HRslt hr;
		void *ret;
		if(type_==DYNAMIC) hr = ptr_->Lock(offset, length, &ret, D3DLOCK_DISCARD);
		else hr = ptr_->Lock(offset, length, &ret, 0);
		if(!hr) return NULL;
		return ret;
	}

	void IndexBuffer::unlock() const
	{
		ptr_->Unlock();
	}

	HRslt IndexBuffer::setCurrent() const
	{
		if(!ptr_) return E_POINTER;
		HRslt hr;
		hr = device().impl()->SetIndices(ptr_);
		if(!hr) PRNN(hr);
		return hr;
	}

	uint IndexBuffer::size() const
	{
		D3DINDEXBUFFER_DESC desc;
		ptr_->GetDesc(&desc);
		return desc.Size;
	}

	/** �`��i�v���~�e�B�u�`�施�ߔ��s�j
	 *
	 * ���_�t�H�[�}�b�g�ƃX�g���[���\�[�X�͐ݒ肷�邪�A���_�V�F�[�_�[�̐ݒ�͕ύX���Ȃ����Ƃɒ���
	 *
	 * @param vb �`��Ɏg�p���钸�_�o�b�t�@
	 * @param vbi ���_�o�b�t�@��ݒ肷�钸�_�X�g���[���ԍ�
	 * @param num �`�悷��v���~�e�B�u��
	 * @param offset �C���f�b�N�X�o�b�t�@�̃I�t�Z�b�g
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 5:19:45
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt IndexBuffer::draw(const VertexBuffer &vb, uint index, D3DPRIMITIVETYPE prim_type, uint num, uint offset) const
	{
		if(!ptr_) return E_POINTER;
		HRslt hr;
		hr = device().impl()->SetFVF( vb.fvf().val );
		if(hr) {
			hr = vb.setCurrent(index);
			if(hr) {
				hr = setCurrent();
				if(hr) {
					hr = device().impl()->DrawIndexedPrimitive( prim_type, 0, 0, num_, offset, num );
					if(!hr) PRNN(hr);
				}
			}
		}
		return hr;
	}

	/** �`��i�v���~�e�B�u�`�施�ߔ��s�j
	 *
	 * ��e�u�e���[�h�p
	 * ���_�t�H�[�}�b�g�ƃX�g���[���\�[�X�͐ݒ肷�邪�A���_�V�F�[�_�[�̐ݒ�͕ύX���Ȃ����Ƃɒ���
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 5:19:45
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt IndexBuffer::draw(const VertexBuffer &vb, dx::FVF fvf, uint index, D3DPRIMITIVETYPE prim_type, uint num, uint offset) const
	{
		assert(fvf.val==0);
		if(!ptr_) return E_POINTER;
		HRslt hr;
		hr = device().impl()->SetFVF( fvf.val );
		if(hr) {
			hr = vb.setCurrent(index);
			if(hr) {
				hr = setCurrent();
				if(hr) {
					hr = device().impl()->DrawIndexedPrimitive( prim_type, 0, 0, num_, offset, num );
					if(!hr) PRNN(hr);
				}
			}
		}
		return hr;
	}

}} // namespace gctp
