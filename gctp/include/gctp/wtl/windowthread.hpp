#ifndef _GCTP_WTL_WINDOWTHREAD_HPP_
#define _GCTP_WTL_WINDOWTHREAD_HPP_
/** @file
 * GameCatapult ゲームスレッドクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/16 19:13:03
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/wtl/window.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/xtime.hpp>

namespace gctp { namespace wtl {

	/** ゲームスレッドクラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 14:41:20
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class GameWindowThreaded : public GameWindow {
		typedef std::auto_ptr<boost::thread> ThreadPtr;
		ThreadPtr thread_;
		volatile bool quit_request_;
		volatile bool suspend_request_;
		volatile bool restored_;
		volatile bool posted_;
		boost::condition suspended_;
		boost::condition resume_;
		bool isReqQuit() { Lock al(monitor_); return quit_request_; }
		bool isRestored() { Lock al(monitor_); return restored_; }
		void restored() { Lock al(monitor_); restored_ = true; posted_ = false; }
		void restore()
		{
			if(!posted_) {
				Lock al(monitor_);
				restored_ = false;
				posted_ = true;
				PostMessage(WM_RESTOREREQ);
			}
		}
		void doSuspend();
		int argc_;
		char **argv_;
		int (*main_)(int argc, char *argv[]);

	protected:
		boost::mutex monitor_;

	public:
		typedef boost::mutex::scoped_lock Lock;
		GameWindowThreaded(int (*main)(int, char *[]), int argc, char *argv[]) : thread_(0)
			,quit_request_(false), suspend_request_(false), restored_(true), posted_(false)
			,main_(main), argc_(argc), argv_(argv)
		{}

		void setCurrent()
		{
			g_.setCurrent();
			a_.setCurrent();
			i_.setCurrent();
			GameApp::setCurrent();
		}

		// 本体実行
		int operator()()
		{
			setCurrent();
			return main_(argc_, argv_);
		}

		virtual bool canContinue();
		virtual bool canDraw();
		virtual void present();
		virtual bool recreate();

		bool suspend(int sec_to_surrender)
		{
			Lock al(monitor_);
			suspend_request_ = true;
			boost::xtime xt;
			boost::xtime_get(&xt, boost::TIME_UTC);
			xt.sec += sec_to_surrender;
			return suspended_.timed_wait(al, xt);
		}
		bool suspend()
		{
			Lock al(monitor_);
			suspend_request_ = true;
			suspended_.wait(al);
			return true;
		}
		void resume() { resume_.notify_all(); }
#ifdef UNICODE
		bool create(const wchar_t *title, bool is_fs, uint mode);
#else
		bool create(const char *title, bool is_fs, uint mode);
#endif
		bool create(HWND target, HWND msgwnd, HWND awnd, HWND iwnd, bool is_fs, uint mode);

		void destroy() { Lock al(monitor_); quit_request_ = true; }

		enum {WM_RESTOREREQ = WM_USER + 0};
		DECLARE_WND_CLASS(_T("GameCatapultThreadedWindowClass"))

	private:
		virtual void toggleFullscreen();
		virtual void resize();

		// WM_DESTROYの処理
		void onDestroy();
		
		// マウス移動に応答（ロックする必要があるので、GameWindowのそれをオーバーライド）
		void onMouseMove(UINT keys, CPoint pt);

		// WM_RESTOREREQの処理
		LRESULT onRestore(UINT /*msg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);

	private:
		BEGIN_MSG_MAP(GameWindowThreaded)
			MSG_WM_DESTROY(onDestroy)
			MSG_WM_MOUSEMOVE(onMouseMove)
			MESSAGE_HANDLER_EX(WM_RESTOREREQ, onRestore)
		    CHAIN_MSG_MAP(GameWindow)
		END_MSG_MAP()
	};

}} //namespace gctp::wtl

#endif //_GCTP_WTL_WINDOWTHREAD_HPP_
