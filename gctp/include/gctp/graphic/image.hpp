#ifndef _GCTP_GRAPHIC_IMAGE_HPP_
#define _GCTP_GRAPHIC_IMAGE_HPP_
/** @file
 * GameCatapult オフスクリーンサーフェスクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/28 12:43:53
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/graphic.hpp>
#include <gctp/graphic/rsrc.hpp>
#include <gctp/class.hpp>

namespace gctp { namespace graphic {

	/// オフスクリーンサーフェスリソースクラス
	class Image : public Rsrc {
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
		};

		Image() : type_(NONE)
		{
		}

		HRslt setUp(const _TCHAR *fname);
		HRslt setUp(const void *memory, std::size_t size);
		HRslt setUp(Type type, int width, int height, int format);

		HRslt restore();
		void cleanUp();

		operator dx::IDirect3DSurface *() const { return ptr_; }
		dx::IDirect3DSurface *get() const { return ptr_; }

		int format() const;
		Point2 size() const;
		bool isDynamic() const { return type_ == WORK; }

		enum LockFlag {
			DISCARD = D3DLOCK_DISCARD,
			NO_DIRTY_UPDATE = D3DLOCK_NO_DIRTY_UPDATE,
			NO_SYSLOCK = D3DLOCK_NOSYSLOCK,
			READONLY = D3DLOCK_READONLY
		};
		LockedRect lock(const Rect &rc, uint32_t flag = 0L);
		LockedRect lock(uint32_t flag = 0L);
		HRslt unlock();

		HRslt save(const _TCHAR *fname);

		/** スコープから抜けるとアンロックする
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/15 5:07:24
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		class ScopedLock {
		public:
			ScopedLock(Image &img, const Rect &rc, uint32_t flag = 0L)
				: img_(img), lr(img.lock(rc, flag)) {}
			ScopedLock(Image &img, uint32_t flag = 0L)
				: img_(img), lr(img.lock(flag)) {}
			~ScopedLock() { img_.unlock(); }
			LockedRect lr;
		private:
			Image &img_;
			uint miplevel_;
		};

	GCTP_DECLARE_CLASS

	protected:
		Type type_;
		int org_width_;
		int org_height_;
		int org_format_;
		dx::IDirect3DSurfacePtr ptr_;
	};

}} //namespace gctp

#endif //_GCTP_GRAPHIC_IMAGE_HPP_