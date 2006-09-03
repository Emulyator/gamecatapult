#ifndef _GCTP_WTL_WINDOW_HPP_
#define _GCTP_WTL_WINDOW_HPP_
/**@file
 * GameCatapult �Q�[���p�V���v���E�B���h�E�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/26 2:22:57
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <atlbase.h>
#include <atlapp.h>
#if (_ATL_VER < 0x0700)
extern CAppModule _Module;
#define _AtlBaseModule _Module
#endif

#define GCTP_AVOID_MINMAX_ERROR // �Ȃ�ŁA�Ƃ��ǂ�max�ŕςȂ��Ƃ�����̂��ȁc
#ifdef GCTP_AVOID_MINMAX_ERROR
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define GCTP_DEFINED_MAX
#endif
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#define GCTP_DEFINED_MIN
#endif
#endif

#include <atlwin.h>
#include <atlcrack.h>
#include <atlmisc.h>
#include <atlframe.h>
#include <atlctrls.h>

#ifdef GCTP_AVOID_MINMAX_ERROR
#ifdef GCTP_DEFINED_MAX
#undef max
#undef GCTP_DEFINED_MAX
#endif
#ifdef GCTP_DEFINED_MIN
#undef min
#undef GCTP_DEFINED_MIN
#endif
#endif

#include <gctp/app.hpp>
#include <gctp/graphic.hpp>
#include <gctp/audio.hpp>
#include <gctp/input.hpp>

namespace gctp { namespace wtl {

	/** �Q�[���p�P���E�B���h�E�N���X
	 *
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2003/01/05 1:17:09
	 * $Id: window.hpp,v 1.2 2005/02/21 02:20:28 takasugi Exp $
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	class GameWindow : public GameApp, public CWindowImpl<GameWindow> {
	protected:
		graphic::Device g_; audio::Device a_; Input i_;
		CStatic view_;
		CWindow msgwnd_;
		CWindow current_;
		
	public:
		GameWindow() : can_change_mode_(true), is_resizable_(false), in_sizing_(false)
			, cursor_(NULL), is_cursor_visible_(true), do_hold_cursor_(false), alt_on_(false) {}

		bool create(const _TCHAR *title, bool is_fs, uint mode);
		bool create(HWND target, HWND msgwnd, HWND awnd, HWND iwnd, bool is_fs, uint mode);

		void setModeChangeEnable(bool enable) { can_change_mode_ = enable; }
		bool canChangeMode() { return can_change_mode_; }
		void setResizable(bool enable);
		void setCursor(HCURSOR cursor) { cursor_ = cursor; }
		void showCursor(bool yes) { is_cursor_visible_ = yes; }
		void holdCursor(bool yes) { do_hold_cursor_ = yes; }
		bool isFS() const { return is_fs_; }

		DECLARE_WND_CLASS(_T("GameCatapultWindowClass"))

		bool service();
		virtual bool canContinue();
		virtual bool canDraw();
		virtual void present();
		virtual bool recreate();

	protected:
		bool setUp();
		bool reset() { return setUp(); }
		virtual void toggleFullscreen();
		virtual void resize();

		/** WM_DESTROY�̏���
		 * �E�B���h�E�J�E���g�����炷
		 * ���ʃ[���ɂȂ�����A�v���I��
		 */
		void onDestroy();
		
		/** �E�B���h�E�̃N���C�A���g�̈�ł̐�΍��W���Z�b�g����
		 */
		void onMouseMove(UINT keys, CPoint pt);

		LRESULT onMouseWheel(UINT nFlags, short zDelta, CPoint point);
		void onLButtonDown(UINT nFlags, CPoint point);
		void onLButtonUp(UINT nFlags, CPoint point);
		void onLButtonDblClk(UINT nFlags, CPoint point);
		void onRButtonDown(UINT nFlags, CPoint point);
		void onRButtonUp(UINT nFlags, CPoint point);
		void onRButtonDblClk(UINT nFlags, CPoint point);
		void onMButtonDown(UINT nFlags, CPoint point);
		void onMButtonUp(UINT nFlags, CPoint point);
		void onMButtonDblClk(UINT nFlags, CPoint point);

		// alt+return�Ń��[�h�ؑ�
		void onSysKeyDown(TCHAR vkey, UINT repeats, UINT code);

		// alt�Ń��j���[���J���̂��֎~
		void onSysKeyUp(TCHAR vkey, UINT repeats, UINT code);

		// WM_NCHITTEST�̏���
		LRESULT onNcHitTest(CPoint /*pt*/);
		
		// �E�B���h�E�ő剻�E���A�����m
		void onSize(UINT /*type*/, CSize /*size*/);

		// �E�B���h�E�T�C�Y�ύX�����m
		void onEnterSizeMove();
		
		// �E�B���h�E�T�C�Y�̕ύX���������ꍇ�A�X�N���[�������Z�b�g
		void onExitSizeMove();
		
		// �J�[�\���ύX���b�Z�[�W�̉���
		LRESULT onSetCursor(HWND hwnd, UINT nHitTest, UINT message);

		HRESULT setRect(const CRect &rc) {
			return current_.SetWindowPos(HWND_NOTOPMOST, rc.left, rc.top, (rc.right-rc.left), (rc.bottom-rc.top), SWP_SHOWWINDOW);
		}
		HRESULT setStyle(long style) {
			return current_.SetWindowLong(GWL_STYLE, style);
		}
		HRESULT setExStyle(long style) {
			return current_.SetWindowLong(GWL_EXSTYLE, style);
		}

		bool can_change_mode_;	// ���[�h�`�F���W���������H
		CRect wndrect_; long style_; long exstyle_;	// �E�B���h�E���[�h���̐ݒ�
		bool is_fs_;
		bool is_resizable_;
		bool in_sizing_;
		// �t���X�N���[�����̐ݒ�
		uint mode_;							///< ��ʃ��[�h�ԍ�

		HCURSOR cursor_;
		bool is_cursor_visible_;
		bool do_hold_cursor_;

	private:
		BEGIN_MSG_MAP_EX(GameWindow)
			MSG_WM_DESTROY(onDestroy)
			MSG_WM_SETCURSOR(onSetCursor)
			MSG_WM_MOUSEMOVE(onMouseMove)
			MSG_WM_SYSKEYDOWN(onSysKeyDown)
			MSG_WM_SYSKEYUP(onSysKeyUp)
			MSG_WM_NCHITTEST(onNcHitTest)
			MSG_WM_SIZE(onSize)
			MSG_WM_ENTERSIZEMOVE(onEnterSizeMove)
			MSG_WM_EXITSIZEMOVE(onExitSizeMove)
//			MSG_WM_MOUSEWHEEL(onMouseWheel)
			MSG_WM_LBUTTONDOWN(onLButtonDown)
			MSG_WM_LBUTTONUP(onLButtonUp)
			MSG_WM_LBUTTONDBLCLK(onLButtonDblClk)
			MSG_WM_RBUTTONDOWN(onRButtonDown)
			MSG_WM_RBUTTONUP(onRButtonUp)
			MSG_WM_RBUTTONDBLCLK(onRButtonDblClk)
			MSG_WM_MBUTTONDOWN(onMButtonDown)
			MSG_WM_MBUTTONUP(onMButtonUp)
			MSG_WM_MBUTTONDBLCLK(onMButtonDblClk)
		END_MSG_MAP()

		bool alt_on_;
	};

}} //namespace gctp::wtl

#endif //_GCTP_WTL_WINDOW_HPP_