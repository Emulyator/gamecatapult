#ifndef _GCTP_WTL_SPLASHSCREEN_HPP_
#define _GCTP_WTL_SPLASHSCREEN_HPP_
/**@file
 * スプラッシュスクリーン
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/26 18:18:03
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <atlbase.h>
#include <atlapp.h>
#if (_ATL_VER < 0x0700)
extern CAppModule _Module;
#define _AtlBaseModule _Module
#endif
#include <atlwin.h>
#include <atlcrack.h>

namespace gctp { namespace wtl {

	/** スプラッシュスクリーン
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/26 18:18:13
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class SplashScreen : public CWindowImpl<SplashScreen> {
		CBitmap bmp_;
	public:
		SplashScreen(HBITMAP bitmap, HWND parent, LPCTSTR windowname = NULL)
		{
			setBitmap(bitmap);
			Create(parent, windowname);
		}
		~SplashScreen()
		{
			if(IsWindow()) DestroyWindow();
		}

		void setBitmap(HBITMAP bitmap)
		{
			if(!bmp_.IsNull()) bmp_.DeleteObject();
			bmp_ = bitmap;
		}

		BEGIN_MSG_MAP_EX(SplashScreen)
			MSG_WM_CREATE(onCreate)
			MSG_WM_PAINT(onPaint)
		END_MSG_MAP()

		HWND Create(HWND parent, LPCTSTR windowname = NULL)
		{
			return CWindowImpl<SplashScreen>::Create(parent, CWindow::rcDefault, windowname, WS_POPUP|WS_BORDER);
		}

		int onCreate(LPCREATESTRUCT /*createstruct*/)
		{
			SIZE size;
			if(!bmp_.IsNull()) {
				bmp_.GetSize(size);
				ResizeClient(size.cx, size.cy, TRUE);
			}
			CenterWindow();
			ShowWindow(SW_SHOW);
			UpdateWindow();
			return 0;
		}

		void onPaint(HDC dc)
		{
			if(!bmp_.IsNull())
			{
				CPaintDC nPDC(m_hWnd);
				CDC dcMem;
				dcMem.CreateCompatibleDC(dc);
				HBITMAP hBmpOld = dcMem.SelectBitmap(bmp_);
				SIZE size;
				bmp_.GetSize(size);
				nPDC.BitBlt(0, 0, size.cx, size.cy, dcMem, 0, 0, SRCCOPY);
				dcMem.SelectBitmap(hBmpOld);
			}
		}
	};

}} //namespace gctp::wtl

#endif //_GCTP_WTL_SPLASHSCREEN_HPP_