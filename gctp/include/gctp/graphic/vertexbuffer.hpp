#ifndef _GCTP_GRAPHIC_VERTEXBUFFER_HPP_
#define _GCTP_GRAPHIC_VERTEXBUFFER_HPP_
/** @file
 * GameCatapult 頂点バッファクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 3:16:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/graphic/rsrc.hpp>
#include <gctp/graphic/dx/fvf.hpp>

namespace gctp { namespace graphic {

	/** 頂点バッファリソースクラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 3:23:40
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class VertexBuffer : public Rsrc {
	public:
		enum Type {
			DYNAMIC = 0,
			STATIC  = 1,
		};

		/// 製作
		HRslt setUp(Type type/**< DYNAMIC VRAMに確保  STATIC 管理リソース*/, dx::FVF fvf/**< 頂点フォーマットを指定*/, uint num/**< 頂点数*/);

		/// 非ＦＶＦバッファとして製作
		HRslt setUp(Type type/**< DYNAMIC VRAMに確保  STATIC 管理リソース*/, uint size/**< バイトサイズ*/);

		/// dx::IDirect3DVertexBufferからセットアップ
		HRslt setUp(dx::IDirect3DVertexBufferPtr src)
		{
			ptr_ = src;
			if(ptr_) {
				D3DVERTEXBUFFER_DESC desc;
				ptr_->GetDesc(&desc);
				type_ = (desc.Pool==D3DPOOL_DEFAULT) ? DYNAMIC : STATIC;
				fvf_ = desc.FVF;
				size_ = desc.Size;
			}
			else size_ = 0;
			used_ = 0;
			return S_OK;
		}

		virtual HRslt restore();
		virtual void cleanUp();
		
		void *lock();
		void *lock(uint offset, uint length);
		const void *lock(uint offset, uint length) const;
		void unlock() const;
		void rewind() { used_ = 0; }
		
		HRslt draw(dx::FVF fvf, uint index, D3DPRIMITIVETYPE prim_type, uint num, uint offset) const;
		HRslt draw(uint index, D3DPRIMITIVETYPE prim_type, uint num, uint offset) const;
		HRslt draw(dx::FVF fvf, uint index, D3DPRIMITIVETYPE prim_type, uint num);
		HRslt draw(uint index, D3DPRIMITIVETYPE prim_type, uint num);
		
		dx::FVF fvf() const { return fvf_; }

		/// 頂点ストリームに設定
		HRslt setCurrent(uint index) const;
		/// バッファのバイトサイズ
		uint size() const;
		/// 頂点数
		uint numVerticies() const { return size()/fvf_.stride(); }
		/// バッファの未使用領域のバイトサイズ
		uint freesize() const;
		/// バッファの未使用領域の頂点数
		uint freenum() const;

		operator dx::IDirect3DVertexBuffer *() { return ptr_; }
		operator const dx::IDirect3DVertexBuffer *() const { return ptr_; }
		dx::IDirect3DVertexBuffer *get() { return ptr_; }
		const dx::IDirect3DVertexBuffer *get() const { return ptr_; }
		
		/** スコープから抜けるとアンロックする
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/15 5:07:24
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		class ScopedLock {
		public:
			explicit ScopedLock(VertexBuffer &vb) : vb_(vb), buf(vb.lock()) { base_ = buf; }
			ScopedLock(VertexBuffer &vb, uint offset, uint length) : vb_(vb), buf(vb.lock(offset, length)) { base_ = buf; }
			~ScopedLock() { vb_.unlock(); }
			void step()
			{
				buf = ((uchar *)buf) + D3DXGetFVFVertexSize(vb_.fvf().val);
			}
			void *buf;
			template<typename _T>
			_T &get(int i)
			{
				return *reinterpret_cast<_T *>(((uchar *)base_) + D3DXGetFVFVertexSize(vb_.fvf().val)*i);
			}
			template<typename _T>
			const _T &get(int i) const
			{
				return *reinterpret_cast<const _T *>(((const uchar *)base_) + D3DXGetFVFVertexSize(vb_.fvf().val)*i);
			}
		private:
			VertexBuffer &vb_;
			void *base_;
		};

	private:
		HRslt setUpEx(Type type, dx::FVF fvf, uint size, uint minsize);

		dx::IDirect3DVertexBufferPtr ptr_;
		Type	type_;
		dx::FVF	fvf_;
		uint	size_;
		uint	used_;
	};

}} //namespace gctp

#endif //_GCTP_GRAPHIC_VERTEXBUFFER_HPP_