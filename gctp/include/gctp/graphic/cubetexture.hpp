#ifndef _GCTP_GRAPHIC_CUBETEXTURE_HPP_
#define _GCTP_GRAPHIC_CUBETEXTURE_HPP_
/** @file
 * GameCatapult キューブテクスチャクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/28 12:43:53
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/graphic.hpp>
#include <gctp/graphic/rsrc.hpp>
#include <gctp/class.hpp>

namespace gctp { namespace graphic {

	/// キューブテクスチャリソースクラス
	class CubeTexture : public Rsrc {
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
			OFFSCREEN
		};

		enum Face
		{
			FACE_POSITIVE_X,
			FACE_NEGATIVE_X,
			FACE_POSITIVE_Y,
			FACE_NEGATIVE_Y,
			FACE_POSITIVE_Z,
			FACE_NEGATIVE_Z
		};

		CubeTexture();

		HRslt setUp(const _TCHAR *fname);
		HRslt setUp(const void *memory, std::size_t size);
		HRslt setUp(Type type, int size, int format, int miplevel);

		HRslt restore();
		void cleanUp();

		operator dx::IDirect3DCubeTexture *() const { return ptr_; }
		operator dx::IDirect3DBaseTexture *() const { return ptr_; }
		dx::IDirect3DCubeTexture *get() const { return ptr_; }

		/// カレントに設定
		HRslt setCurrent(uint index) const;
		/// カレントか？
		bool isCurrent(uint index) const;
		/// テクスチャを無効に
		static HRslt unset(uint index);

		int format() const;
		Point2 size() const;
		bool isDynamic() const { return type_ == WORK; }

		enum LockFlag {
			DISCARD = D3DLOCK_DISCARD,
			NO_DIRTY_UPDATE = D3DLOCK_NO_DIRTY_UPDATE,
			NO_SYSLOCK = D3DLOCK_NOSYSLOCK,
			READONLY = D3DLOCK_READONLY
		};
		LockedRect lock(Face face, uint miplevel, const Rect &rc, uint32_t flag = 0L);
		LockedRect lock(Face face, uint miplevel, uint32_t flag = 0L);
		HRslt unlock(Face face, uint miplevel);

		/** スコープから抜けるとアンロックする
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/15 5:07:24
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		class ScopedLock {
		public:
			ScopedLock(CubeTexture &tex, Face face, uint miplevel, const Rect &rc, uint32_t flag = 0L)
				: tex_(tex), face_(face), miplevel_(miplevel), lr(tex.lock(face, miplevel, rc, flag)) {}
			ScopedLock(CubeTexture &tex, Face face, uint miplevel, uint32_t flag = 0L)
				: tex_(tex), face_(face), miplevel_(miplevel), lr(tex.lock(face, miplevel, flag)) {}
			~ScopedLock() { tex_.unlock(face_, miplevel_); }
			LockedRect lr;
		private:
			CubeTexture &tex_;
			Face face_;
			uint miplevel_;
		};

	GCTP_DECLARE_CLASS

	protected:
		Type type_;
		int size_, format_, miplevel_;
		dx::IDirect3DCubeTexturePtr ptr_;
	};

}} //namespace gctp

#endif //_GCTP_GRAPHIC_CUBETEXTURE_HPP_