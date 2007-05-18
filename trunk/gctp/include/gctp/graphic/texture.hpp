#ifndef _GCTP_GRAPHIC_TEXTURE_HPP_
#define _GCTP_GRAPHIC_TEXTURE_HPP_
/** @file
 * GameCatapult テクスチャクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/28 12:43:53
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/graphic.hpp>
#include <gctp/graphic/rsrc.hpp>
#include <gctp/class.hpp>

namespace gctp { namespace graphic {

	/// テクスチャリソースクラス
	class Texture : public Rsrc {
	public:
		struct LockedRect {
			int pitch;
			void *buf;
			LockedRect(int _pitch, void *_buf) : pitch(_pitch), buf(_buf) {}
			bool operator !() { return buf ? false : true; }
			operator bool() { return buf ? true : false; }
		};

		enum Type {
			NONE, // 未初期化をあらわす。setUpに指定してはいけない
			NORMAL,
			WORK,
			RENDERTARGET,
			OFFSCREEN,
		};

		Texture();

		HRslt setUp(const _TCHAR *fname);
		HRslt setUp(const void *memory, std::size_t size);
		HRslt setUp(Type type, int width, int height, int format, int miplevel = 1);

		HRslt restore();
		void cleanUp();

		operator dx::IDirect3DTexture *() const { return ptr_; }
		operator dx::IDirect3DBaseTexture *() const { return ptr_; }
		dx::IDirect3DTexture *get() const { return ptr_; }

		/// カレントに設定
		HRslt setCurrent(uint index) const;
		/// カレントか？
		bool isCurrent(uint index) const;
		/// テクスチャを無効に
		static HRslt unSet(uint index);

		Point2 size() const;
		Point2 originalSize() const { return Point2C(org_width_, org_height_); }
		bool isDynamic() { return type_ == WORK; }
		
		enum LockFlag {
			DISCARD = D3DLOCK_DISCARD,
			NO_DIRTY_UPDATE = D3DLOCK_NO_DIRTY_UPDATE,
			NO_SYSLOCK = D3DLOCK_NOSYSLOCK,
			READONLY = D3DLOCK_READONLY
		};
		LockedRect lock(uint miplevel, const Rect &rc, uint32_t flag = 0L);
		LockedRect lock(uint miplevel, uint32_t flag = 0L);
		HRslt unlock(uint miplevel);

		/** スコープから抜けるとアンロックする
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/15 5:07:24
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		class ScopedLock {
		public:
			ScopedLock(Texture &tex, uint miplevel, const Rect &rc, uint32_t flag = 0L)
				: tex_(tex), miplevel_(miplevel), lr(tex.lock(miplevel, rc, flag)) {}
			ScopedLock(Texture &tex, uint miplevel, uint32_t flag = 0L)
				: tex_(tex), miplevel_(miplevel), lr(tex.lock(miplevel, flag)) {}
			~ScopedLock() { tex_.unlock(miplevel_); }
			LockedRect lr;
		private:
			Texture &tex_;
			uint miplevel_;
		};

	GCTP_DECLARE_CLASS

	protected:
		int org_width_;
		int org_height_;
		int org_format_;
		int org_miplevel_;
		Type type_;
		dx::IDirect3DTexturePtr ptr_;
	};

}} //namespace gctp

#endif //_GCTP_GRAPHIC_TEXTURE_HPP_