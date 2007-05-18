#ifndef _GCTP_GRAPHIC_RENDERTARGET_HPP_
#define _GCTP_GRAPHIC_RENDERTARGET_HPP_
/** @file
 * GameCatapult レンダーターゲットクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/28 12:43:53
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/graphic.hpp>
#include <gctp/graphic/rsrc.hpp>
#include <gctp/class.hpp>

namespace gctp { namespace graphic {

	class Texture;

	/// テクスチャリソースクラス
	class RenderTarget : public Rsrc {
	public:
		enum Type {
			NORMAL, // 後でテクスチャとして使いたいという、普通の使い道
			OFFSCREEN // ファイルに落としたい時用
		};

		struct LockedRect {
			int pitch;
			void *buf;
			LockedRect(int _pitch, void *_buf) : pitch(_pitch), buf(_buf) {}
			bool operator !() { return buf ? false : true; }
			operator bool() { return buf ? true : false; }
		};

		RenderTarget();

		HRslt setUp(const _TCHAR *fname);
		HRslt setUp(Type type, int width, int height, int format, int miplevel=1);

		HRslt restore();
		void cleanUp();

		operator dx::IDirect3DSurface *() const { return ptr_; }
		dx::IDirect3DSurface *get() const { return ptr_; }

		/// カレントに設定
		HRslt begin(uint index = 0);
		/// レンダーターゲットを無効に
		HRslt end();
		/// テクスチャに転送
		HRslt resolve();
		/// テクスチャ取得
		Texture &texture()
		{
			return *tex_;
		}

		Point2 size() const;
		
		enum LockFlag {
			DISCARD = D3DLOCK_DISCARD,
			NO_DIRTY_UPDATE = D3DLOCK_NO_DIRTY_UPDATE,
			NO_SYSLOCK = D3DLOCK_NOSYSLOCK,
			READONLY = D3DLOCK_READONLY
		};
		LockedRect lock(const Rect &rc, uint32_t flag = 0L);
		LockedRect lock(uint32_t flag = 0L);
		HRslt unlock();

	GCTP_DECLARE_CLASS

	protected:
		dx::IDirect3DSurfacePtr ptr_;
		dx::IDirect3DSurfacePtr backup_;
		gctp::Pointer<Texture> tex_;

	private:
		int org_width_;
		int org_height_;
		int org_format_;
		int org_miplevel_;
		int index_for_system_;
		Type type_;
	};

}} //namespace gctp

#endif //_GCTP_GRAPHIC_RENDERTARGET_HPP_