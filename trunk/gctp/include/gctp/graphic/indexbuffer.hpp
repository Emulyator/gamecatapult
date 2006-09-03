#ifndef _GCTP_GRAPHIC_INDEXBUFFER_HPP_
#define _GCTP_GRAPHIC_INDEXBUFFER_HPP_
/** @file
 * GameCatapult インデックスバッファクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 3:16:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/graphic/rsrc.hpp>
#include <gctp/graphic/dx/fvf.hpp>

namespace gctp { namespace graphic {

	class VertexBuffer;
	/** インデックスバッファリソースクラス
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

		/// 製作
		HRslt setUp(Type type/**< DYNAMIC VRAMに確保  STATIC 管理リソース*/, Bits bits/**< SHORT 16ビット  LONG 32ビット*/, uint num/**< インデックス長*/);

		virtual HRslt restore();
		virtual void cleanUp();
		
		void *lock(uint offset, uint length);
		ushort *lock16(uint offset, uint length)
		{
			return reinterpret_cast<ushort *>(lock(offset, length));
		}
		ulong *lock32(uint offset, uint length)
		{
			return reinterpret_cast<ulong *>(lock(offset, length));
		}
		void unlock() const;
		
		HRslt setCurrent() const;
		uint size() const;

		operator IDirect3DIndexBuffer9 *() { return ptr_; }
		IDirect3DIndexBuffer9 *getIB() { return ptr_; }
		
		/// インデックスプリミティブ描画
		HRslt draw(const VertexBuffer &vb, dx::FVF fvf, uint index, D3DPRIMITIVETYPE prim_type, uint num, uint offset) const;
		/// インデックスプリミティブ描画
		HRslt draw(const VertexBuffer &vb, uint index, D3DPRIMITIVETYPE prim_type, uint num, uint offset) const;
		
		/** スコープから抜けるとアンロックする
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/15 5:07:24
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		class ScopedLock {
		public:
			ScopedLock(IndexBuffer &ib, uint offset, uint length) : ib_(ib), buf(ib.lock(offset, length)) {}
			~ScopedLock() { ib_.unlock(); }
			union {
				void *buf;
				ushort *buf16;
				ulong  *buf32;
			};
		private:
			IndexBuffer &ib_;
		};

	private:
		dx::IDirect3DIndexBuffer9Ptr ptr_;
		Type	type_;
		Bits	bits_;
		uint	num_;
	};

}} //namespace gctp

#endif //_GCTP_GRAPHIC_INDEXBUFFER_HPP_