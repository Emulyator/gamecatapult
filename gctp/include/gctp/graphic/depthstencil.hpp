#ifndef _GCTP_GRAPHIC_DEPTHSTENCIL_HPP_
#define _GCTP_GRAPHIC_DEPTHSTENCIL_HPP_
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

	/// 深度・ステンシルバッファリソースクラス
	class DepthStencil : public Rsrc {
	public:
		struct LockedRect {
			int pitch;
			void *buf;
			LockedRect(int _pitch, void *_buf) : pitch(_pitch), buf(_buf) {}
			bool operator !() { return buf ? false : true; }
			operator bool() { return buf ? true : false; }
		};

		DepthStencil();

		HRslt setUp(const _TCHAR *fname);
		HRslt setUp(int width, int height, int format = D3DFMT_D24S8, bool discardable = false);

		HRslt restore();
		void cleanUp();

		operator dx::IDirect3DSurface *() const { return ptr_; }
		dx::IDirect3DSurface *get() const { return ptr_; }

		/// カレントに設定
		HRslt begin(bool stencil =  true);
		/// 無効に
		HRslt end();

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

	private:
		int org_width_;
		int org_height_;
		int org_format_;
		bool discardable_;
		int index_for_system_;
	};

}} //namespace gctp

#endif //_GCTP_GRAPHIC_DEPTHSTENCIL_HPP_