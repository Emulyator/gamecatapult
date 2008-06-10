#ifndef _GCTP_GRAPHIC_INDEXBUFFER_HPP_
#define _GCTP_GRAPHIC_INDEXBUFFER_HPP_
/** @file
 * GameCatapult �C���f�b�N�X�o�b�t�@�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 3:16:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/graphic/rsrc.hpp>
#include <gctp/graphic/dx/fvf.hpp>

namespace gctp { namespace graphic {

	class VertexBuffer;
	/** �C���f�b�N�X�o�b�t�@���\�[�X�N���X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 3:23:40
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class IndexBuffer : public Rsrc {
	public:
		enum Type {
			DYNAMIC = 0,
			STATIC  = 1,
		};

		enum Bits {
			SHORT,
			LONG,
		};

		/// ����
		HRslt setUp(Type type/**< DYNAMIC VRAM�Ɋm��  STATIC �Ǘ����\�[�X*/, Bits bits/**< SHORT 16�r�b�g  LONG 32�r�b�g*/, uint num/**< �C���f�b�N�X��*/);
		/// dx::IDirect3DIndexBuffer����Z�b�g�A�b�v
		HRslt setUp(dx::IDirect3DIndexBufferPtr src)
		{
			ptr_ = src;
			if(ptr_) {
				D3DINDEXBUFFER_DESC desc;
				ptr_->GetDesc(&desc);
				type_ = (desc.Pool==D3DPOOL_DEFAULT) ? DYNAMIC : STATIC;
				bits_ = desc.Format == D3DFMT_INDEX16 ? SHORT : LONG;
				num_ = desc.Size / (bits_== SHORT ? 2 : 4);
			}
			else {
				bits_ = SHORT;
				num_ = 0;
			}
			return S_OK;
		}

		virtual HRslt restore();
		virtual void cleanUp();
		
		void *lock(uint offset, uint length);
		const void *lock(uint offset, uint length) const;
		void unlock() const;

		ushort *lock16(uint offset, uint length)
		{
			return reinterpret_cast<ushort *>(lock(offset, length));
		}
		ulong *lock32(uint offset, uint length)
		{
			return reinterpret_cast<ulong *>(lock(offset, length));
		}
		
		/// �C���f�b�N�X�X�g���[���ɐݒ�
		HRslt setCurrent() const;
		uint size() const;

		uint indexNum() const { return num_; }

		uint primitiveNum(D3DPRIMITIVETYPE prim_type) const
		{
			if(prim_type == D3DPT_POINTLIST) return num_;
			else if(prim_type == D3DPT_LINELIST) return num_/2;
			else if(prim_type == D3DPT_LINESTRIP) return num_-1;
			else if(prim_type == D3DPT_TRIANGLELIST) return num_/3;
			else if(prim_type == D3DPT_TRIANGLESTRIP) return num_-2;
			else if(prim_type == D3DPT_TRIANGLEFAN) return num_-2;
			return 0;
		}

		operator dx::IDirect3DIndexBuffer *() { return ptr_; }
		operator const dx::IDirect3DIndexBuffer *() const { return ptr_; }
		dx::IDirect3DIndexBuffer *get() { return ptr_; }
		const dx::IDirect3DIndexBuffer *get() const { return ptr_; }
		
		/// �C���f�b�N�X�v���~�e�B�u�`��
		HRslt draw(const VertexBuffer &vb, dx::FVF fvf, uint index, D3DPRIMITIVETYPE prim_type, uint num, uint offset) const;
		/// �C���f�b�N�X�v���~�e�B�u�`��
		HRslt draw(const VertexBuffer &vb, uint index, D3DPRIMITIVETYPE prim_type, uint num, uint offset) const;
		
		/** �X�R�[�v���甲����ƃA�����b�N����
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/15 5:07:24
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		class ScopedLock {
		public:
			explicit ScopedLock(IndexBuffer &ib) : ib_(ib), buf(ib.lock(0, 0)) { base_ = buf; }
			ScopedLock(IndexBuffer &ib, uint offset, uint length) : ib_(ib), buf(ib.lock(offset, length)) { base_ = buf; }
			~ScopedLock() { ib_.unlock(); }
			union {
				void *buf;
				ushort *buf16;
				ulong  *buf32;
			};
			template<typename _T>
			_T &get(int i)
			{
				return *reinterpret_cast<_T *>(((uchar *)base_) + sizeof(_T)*i);
			}
			template<typename _T>
			const _T &get(int i) const
			{
				return *reinterpret_cast<const _T *>(((const uchar *)base_) + sizeof(_T)*i);
			}
		private:
			IndexBuffer &ib_;
			void *base_;
		};

	private:
		dx::IDirect3DIndexBufferPtr ptr_;
		Type	type_;
		Bits	bits_;
		uint	num_;
	};

}} //namespace gctp

#endif //_GCTP_GRAPHIC_INDEXBUFFER_HPP_