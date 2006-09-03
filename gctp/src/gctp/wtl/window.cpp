/**@file
 * GameCatapult �Q�[���p�V���v���E�B���h�E�N���X
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/26 2:23:18
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/wtl/window.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/dbgout.hpp>

namespace gctp { namespace wtl {

	bool GameWindow::service()
	{
		MSG msg;
		while(::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			int ret = ::GetMessage(&msg, NULL, 0, 0);
			if(ret == -1) {
				ATLTRACE2(atlTraceUI, 0, _T("::GetMessage returned -1 (error)\n"));
				continue;	// error, don't process
			}
			else if(!ret) {
				ATLTRACE2(atlTraceUI, 0, _T("canContinue = false - exiting\n"));
				return false;		// WM_QUIT, exit message loop
			}

			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		if(IsWindow()) i_.update();
		return true;
	}

	bool GameWindow::canContinue() {
		GameApp::canContinue();
		if(service() && g_.isOpen()) {
			g_.setCurrent();
			a_.setCurrent();
			i_.setCurrent();
			return true;
		}
		return false;
	}

	bool GameWindow::canDraw() {
		GameApp::canDraw();
		HRslt hr = g_.impl()->TestCooperativeLevel();
		if(IsIconic() || hr == D3DERR_DEVICELOST) return false;
		else if(hr == D3DERR_DEVICENOTRESET) {
			g_.cleanUp();
			if(reset()) {
				g_.restore();
				PRNN("restore! "<<((is_fs_)?"true":"false"));
			}
			else return false; 
		}
		g_.setCurrent();
		return true;
	}

	void GameWindow::present() {
		HRslt hr = g_.present();
		if(!hr) {
			hr = g_.impl()->TestCooperativeLevel();
			if(hr == D3DERR_DEVICENOTRESET) {
				g_.cleanUp();
				if(reset()) {
					g_.restore();
					PRNN("restore! "<<((is_fs_)?"true":"false"));
				}
				else return;
			}
		}
		GameApp::present();
	}

	bool GameWindow::create(const _TCHAR *title, bool is_fs, uint mode)
	{
		const D3DDISPLAYMODE &_mode = graphic::dx::adapters()[0].modes[mode];
		RECT rc = {0, 0, _mode.Width, _mode.Height};
		DWORD style;
		if(is_resizable_) style = WS_OVERLAPPEDWINDOW;
		else style = WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX;
		AdjustWindowRectEx(&rc, style, FALSE, WS_EX_OVERLAPPEDWINDOW);
		Create(NULL, rc, title, style, WS_EX_OVERLAPPEDWINDOW);
		if(IsWindow()) {
			current_ = m_hWnd;
			CenterWindow();
			is_fs_ = is_fs; mode_ = mode;
			GetWindowRect(&wndrect_); style_ = GetStyle(); exstyle_ = GetExStyle();
			if(is_fs_) {
				// ���ɂ������A�v���[���ȃE�B���h�E�ɂ���
				setStyle(0); setExStyle(0);
			}
			ShowWindow(SW_SHOW);
			UpdateWindow();
			if(setUp() && a_.open(m_hWnd) && i_.setUp(m_hWnd)) {
				incWindow();
				g_.setCurrent();
				a_.setCurrent();
				i_.setCurrent();
				setCurrent();
				return true;
			}
			else DestroyWindow();
		}
		return false;
	}

	bool GameWindow::create(HWND target, HWND msgwnd, HWND awnd, HWND iwnd, bool is_fs, uint mode)
	{
		can_change_mode_ = false;
		msgwnd_ = msgwnd;
		view_ = target;
		const D3DDISPLAYMODE &_mode = graphic::dx::adapters()[0].modes[mode];
		RECT rc = {0, 0, _mode.Width, _mode.Height};
		DWORD style;
		if(is_resizable_) style = WS_OVERLAPPEDWINDOW;
		else style = WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX;
		AdjustWindowRectEx(&rc, style, FALSE, WS_EX_OVERLAPPEDWINDOW);
		Create(NULL, rc, NULL, style, WS_EX_OVERLAPPEDWINDOW);
		if(IsWindow()) {
			is_fs_ = false; mode_ = mode;
			view_.GetWindowRect(&wndrect_); style_ = view_.GetStyle(); exstyle_ = view_.GetExStyle();
			if(is_fs_) {
				// ���ɂ������A�v���[���ȃE�B���h�E�ɂ���
				setStyle(0); setExStyle(0);
				ShowWindow(SW_SHOW);
				UpdateWindow();
				current_ = m_hWnd;
			}
			else current_ = view_;
			if(setUp() && a_.open(awnd) && i_.setUp(iwnd)) {
				g_.setCurrent();
				a_.setCurrent();
				i_.setCurrent();
				setCurrent();
				return true;
			}
			else DestroyWindow();
		}
		return false;
	}

	void GameWindow::setResizable(bool enable)
	{
		is_resizable_ = enable;
		if(IsWindow()) {
			CRect rc;
			current_.GetClientRect(&rc);
			if(is_fs_) {
				if(is_resizable_) style_ |= WS_THICKFRAME|WS_MAXIMIZEBOX;
				else style_ &= ~(WS_THICKFRAME|WS_MAXIMIZEBOX);
				AdjustWindowRectEx(&rc, style_, FALSE, WS_EX_OVERLAPPEDWINDOW);
				wndrect_.right = wndrect_.left + rc.Width();
				wndrect_.bottom = wndrect_.top + rc.Height();
			}
			else {
				if(is_resizable_) current_.ModifyStyle(0, WS_THICKFRAME|WS_MAXIMIZEBOX);
				else current_.ModifyStyle(WS_THICKFRAME|WS_MAXIMIZEBOX, 0);
				style_ = current_.GetStyle();
				AdjustWindowRectEx(&rc, style_, FALSE, WS_EX_OVERLAPPEDWINDOW);
				wndrect_.right = wndrect_.left + rc.Width();
				wndrect_.bottom = wndrect_.top + rc.Height();
				MoveWindow(wndrect_.left, wndrect_.top, wndrect_.Width(), wndrect_.Height());
			}
		}
	}

	bool GameWindow::setUp()
	{
		HRslt hr;
		if(is_fs_) {
			hr = g_.open(m_hWnd, 0, mode_);
			if(!hr) {
				PRNN("�t���X�N���[���Ő���ł��܂���ł����B:"<<hr);
				is_fs_ = false;
				hr = g_.open(m_hWnd, view_);
			}
		}
		else {
			hr = g_.open(m_hWnd, view_);
		}
		return hr;
	}

	void GameWindow::toggleFullscreen()
	{
		PRNN("###########toggleFullscreen");
		// �E�B���h�E���[�h�Ƃe�r�̐؂�ւ�
		PRNN("******changing mode");
		if(is_fs_) {
			g_.cleanUp();
			is_fs_ = false;
			if(reset()) {
				// �ȑO�̃E�B���h�E�X�^�C�����Č�
				in_sizing_ = true;
				if(view_) {
					ShowWindow(SW_HIDE);
					g_.restore();
				}
				else {
					CRect rc = wndrect_;
					setStyle(style_); setExStyle(exstyle_); setRect(wndrect_);
					// �t���X�N���[������E�B���h�E�ɁA�̏ꍇ�A�t���X�N���[���𑜓x��
					// �E�B���h�E�N���C�A���g���N�g���Ⴄ�ꍇ������񃊃Z�b�g���K�v���ۂ��B
					// �i�ǂ̊��ł������Ȃ̂��m��Ȃ����ǁj
					const D3DDISPLAYMODE &_mode = graphic::dx::adapters()[0].modes[mode_];
					CRect rc1(0, 0, _mode.Width, _mode.Height);
					CRect rc2;
					current_.GetWindowRect(&rc2);
					if(rc1.Size()!=rc2.Size()) {
						PRNN("*�𑜓x���Ⴄ�B������x���Z�b�g");
						if(reset()) g_.restore();
					}
					else g_.restore();
				}
				in_sizing_ = false;
			}
		}
		else {
			// ���݂̃E�B���h�E�X�^�C���ƃ��N�g��ۑ�
			current_.GetWindowRect(&wndrect_); style_ = current_.GetStyle(); exstyle_ = current_.GetExStyle();
			g_.cleanUp();
			is_fs_ = true;
			if(reset()) {
				if(is_fs_) {
					// ���ɂ������A�v���[���ȃE�B���h�E�ɂ���
					setStyle(0); setExStyle(0);
					ShowWindow(SW_SHOWMAXIMIZED); // XP�ł͕s�v�����ǁA2000�ł͕K�v�c
				}
				else {
					// �t���X�N���[���擾�Ɏ��s
					// �ȑO�̃E�B���h�E�X�^�C�����Č�
					setStyle(style_); setExStyle(exstyle_); setRect(wndrect_);
				}
				g_.restore();
			}
		}
		PRNN("*****change mode end");
	}

	bool GameWindow::recreate()
	{
		g_.cleanUp();
		if(reset()) {
			g_.restore();
			return true;
		}
		return false;
	}

	void GameWindow::resize()
	{
		if(!in_sizing_ && !is_fs_ && g_.isOpen()) {
			CRect rc;
			current_.GetWindowRect(&rc);
			if(wndrect_.Size()!=rc.Size()) {
				PRNN(":size=("<<wndrect_.Size().cx<<","<<wndrect_.Size().cy<<")->("<<rc.Size().cx<<","<<rc.Size().cy<<")");
				wndrect_ = rc;
				g_.cleanUp();
				if(reset()) g_.restore();
			}
		}
	}

	void GameWindow::onDestroy()
	{
		g_.close();
		a_.close();
		i_.tearDown();
		if(!view_ && decWindow()==0) ::PostQuitMessage(0);
	}

	void GameWindow::onMouseMove(UINT keys, CPoint pt)
	{
		/*
		MK_CONTROL   Ctrl �L�[��������Ă���ꍇ�ɃZ�b�g���܂��B
		MK_LBUTTON   �}�E�X�̍��{�^����������Ă���ꍇ�ɃZ�b�g���܂��B
		MK_MBUTTON   �}�E�X�̒����{�^����������Ă���ꍇ�ɃZ�b�g���܂��B
		MK_RBUTTON   �}�E�X�̉E�{�^����������Ă���ꍇ�ɃZ�b�g���܂��B
		MK_SHIFT     Shift �L�[��������Ă���ꍇ�ɃZ�b�g���܂��B
		*/
		if(msgwnd_ && view_) msgwnd_.MapWindowPoints(view_, &pt, 1);
		i_.mouse().setPoint(pt.x, pt.y);
		if(do_hold_cursor_ /*&& ::GetFocus() == m_hWnd*/) {
			CRect rc;
			current_.GetWindowRect(&rc);
			rc.DeflateRect(rc.Width()/4, rc.Height()/4);
			if(current_.ClientToScreen(&pt) && !rc.PtInRect(pt)) {
				CPoint center = rc.CenterPoint();
				::SetCursorPos(center.x, center.y);
			}
		}
	}

	LRESULT GameWindow::onMouseWheel(UINT nFlags, short zDelta, CPoint point)
	{
		uint8_t opt = 0;
		if(nFlags & MK_LBUTTON) opt |= GUIEvents::LB;
		if(nFlags & MK_RBUTTON) opt |= GUIEvents::RB;
		if(nFlags & MK_MBUTTON) opt |= GUIEvents::MB;
		if(nFlags & MK_CONTROL) opt |= GUIEvents::CTRL;
		if(nFlags & MK_SHIFT) opt |= GUIEvents::SHIFT;
		if(alt_on_) opt |= GUIEvents::ALT;
		GUIEvents::postWheelMsg(events(), Point2C(point.x, point.y), zDelta, GUIEvents::MOUSE, opt);
		return S_OK;
	}

	void GameWindow::onLButtonDown(UINT nFlags, CPoint point)
	{
		uint8_t opt = 0;
		if(nFlags & MK_RBUTTON) opt |= GUIEvents::RB;
		if(nFlags & MK_MBUTTON) opt |= GUIEvents::MB;
		if(nFlags & MK_CONTROL) opt |= GUIEvents::CTRL;
		if(nFlags & MK_SHIFT) opt |= GUIEvents::SHIFT;
		if(alt_on_) opt |= GUIEvents::ALT;
		GUIEvents::postDownMsg(events(), Point2C(point.x, point.y), GUIEvents::LB, opt);
	}

	void GameWindow::onLButtonUp(UINT nFlags, CPoint point)
	{
		uint8_t opt = 0;
		if(nFlags & MK_RBUTTON) opt |= GUIEvents::RB;
		if(nFlags & MK_MBUTTON) opt |= GUIEvents::MB;
		if(nFlags & MK_CONTROL) opt |= GUIEvents::CTRL;
		if(nFlags & MK_SHIFT) opt |= GUIEvents::SHIFT;
		if(alt_on_) opt |= GUIEvents::ALT;
		GUIEvents::postUpMsg(events(), Point2C(point.x, point.y), GUIEvents::LB, opt);
	}

	void GameWindow::onLButtonDblClk(UINT nFlags, CPoint point)
	{
		uint8_t opt = 0;
		if(nFlags & MK_RBUTTON) opt |= GUIEvents::RB;
		if(nFlags & MK_MBUTTON) opt |= GUIEvents::MB;
		if(nFlags & MK_CONTROL) opt |= GUIEvents::CTRL;
		if(nFlags & MK_SHIFT) opt |= GUIEvents::SHIFT;
		if(alt_on_) opt |= GUIEvents::ALT;
		GUIEvents::postDblClickMsg(events(), Point2C(point.x, point.y), GUIEvents::LB, opt);
	}

	void GameWindow::onRButtonDown(UINT nFlags, CPoint point)
	{
		uint8_t opt = 0;
		if(nFlags & MK_LBUTTON) opt |= GUIEvents::LB;
		if(nFlags & MK_MBUTTON) opt |= GUIEvents::MB;
		if(nFlags & MK_CONTROL) opt |= GUIEvents::CTRL;
		if(nFlags & MK_SHIFT) opt |= GUIEvents::SHIFT;
		if(alt_on_) opt |= GUIEvents::ALT;
		GUIEvents::postDownMsg(events(), Point2C(point.x, point.y), GUIEvents::RB, opt);
	}

	void GameWindow::onRButtonUp(UINT nFlags, CPoint point)
	{
		uint8_t opt = 0;
		if(nFlags & MK_LBUTTON) opt |= GUIEvents::LB;
		if(nFlags & MK_MBUTTON) opt |= GUIEvents::MB;
		if(nFlags & MK_CONTROL) opt |= GUIEvents::CTRL;
		if(nFlags & MK_SHIFT) opt |= GUIEvents::SHIFT;
		if(alt_on_) opt |= GUIEvents::ALT;
		GUIEvents::postUpMsg(events(), Point2C(point.x, point.y), GUIEvents::RB, opt);
	}

	void GameWindow::onRButtonDblClk(UINT nFlags, CPoint point)
	{
		uint8_t opt = 0;
		if(nFlags & MK_LBUTTON) opt |= GUIEvents::LB;
		if(nFlags & MK_MBUTTON) opt |= GUIEvents::MB;
		if(nFlags & MK_CONTROL) opt |= GUIEvents::CTRL;
		if(nFlags & MK_SHIFT) opt |= GUIEvents::SHIFT;
		if(alt_on_) opt |= GUIEvents::ALT;
		GUIEvents::postDblClickMsg(events(), Point2C(point.x, point.y), GUIEvents::RB, opt);
	}

	void GameWindow::onMButtonDown(UINT nFlags, CPoint point)
	{
		uint8_t opt = 0;
		if(nFlags & MK_LBUTTON) opt |= GUIEvents::LB;
		if(nFlags & MK_RBUTTON) opt |= GUIEvents::RB;
		if(nFlags & MK_CONTROL) opt |= GUIEvents::CTRL;
		if(nFlags & MK_SHIFT) opt |= GUIEvents::SHIFT;
		if(alt_on_) opt |= GUIEvents::ALT;
		GUIEvents::postDownMsg(events(), Point2C(point.x, point.y), GUIEvents::MB, opt);
	}

	void GameWindow::onMButtonUp(UINT nFlags, CPoint point)
	{
		uint8_t opt = 0;
		if(nFlags & MK_LBUTTON) opt |= GUIEvents::LB;
		if(nFlags & MK_RBUTTON) opt |= GUIEvents::RB;
		if(nFlags & MK_CONTROL) opt |= GUIEvents::CTRL;
		if(nFlags & MK_SHIFT) opt |= GUIEvents::SHIFT;
		if(alt_on_) opt |= GUIEvents::ALT;
		GUIEvents::postUpMsg(events(), Point2C(point.x, point.y), GUIEvents::MB, opt);
	}

	void GameWindow::onMButtonDblClk(UINT nFlags, CPoint point)
	{
		uint8_t opt = 0;
		if(nFlags & MK_LBUTTON) opt |= GUIEvents::LB;
		if(nFlags & MK_RBUTTON) opt |= GUIEvents::RB;
		if(nFlags & MK_CONTROL) opt |= GUIEvents::CTRL;
		if(nFlags & MK_SHIFT) opt |= GUIEvents::SHIFT;
		if(alt_on_) opt |= GUIEvents::ALT;
		GUIEvents::postDblClickMsg(events(), Point2C(point.x, point.y), GUIEvents::MB, opt);
	}

	void GameWindow::onSysKeyDown(TCHAR vkey, UINT repeats, UINT code)
	{
		alt_on_ = true;
		if(vkey == VK_RETURN && can_change_mode_) toggleFullscreen();
		else SetMsgHandled(FALSE);
	}

	void GameWindow::onSysKeyUp(TCHAR vkey, UINT repeats, UINT code)
	{
		alt_on_ = false;
		if(vkey != VK_MENU) SetMsgHandled(FALSE);
	}

	LRESULT GameWindow::onNcHitTest(CPoint /*pt*/)
	{
		if(is_fs_) return (LRESULT)HTCLIENT;
		else SetMsgHandled(FALSE);
		return 0;
	}

	void GameWindow::onSize(UINT type, CSize /*size*/)
	{
		if(type != SIZE_MINIMIZED) resize();
		SetMsgHandled(FALSE);
	}

	void GameWindow::onEnterSizeMove()
	{
		PRNN("### in sizing");
		in_sizing_ = true;
		SetMsgHandled(FALSE);
	}
	
	void GameWindow::onExitSizeMove()
	{
		PRNN("### out sizing");
		in_sizing_ = false;
		resize();
		SetMsgHandled(FALSE);
	}

	LRESULT GameWindow::onSetCursor(HWND hwnd, UINT nHitTest, UINT message)
	{
		if(nHitTest == HTCLIENT) {
			if(is_cursor_visible_) {
				if(cursor_) ::SetCursor(cursor_);
				else SetMsgHandled(FALSE);
			}
			else ::SetCursor(NULL);
		}
		else SetMsgHandled(FALSE);
		return FALSE;
	}

}} // namespace gctp::wtl
