/** @file
 * GameCatapult スレッドクラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: windowthread.cpp,v 1.2 2005/02/21 02:21:12 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/wtl/windowthread.hpp>
#include <gctp/dbgout.hpp>
#include <boost/ref.hpp>
#include <gctp/graphic/dx/device.hpp>

namespace gctp { namespace wtl {

	void GameWindowThreaded::doSuspend()
	{
		Lock al(monitor_);
		bool do_suspend = false;
		if(suspend_request_) {
			suspend_request_ = false;
			suspended_.notify_all();
			do_suspend = true;
		}
		if(do_suspend) resume_.wait(al);
	}

	bool GameWindowThreaded::canContinue()
	{
		GameApp::canContinue();
		doSuspend();
		boost::thread::yield();
		{
			Lock al(monitor_);
			i_.update();
			// こうしていることに、深い意味あるのかなぁ？
		}
		if(!isReqQuit()) {
			Lock al(monitor_);
			if(g_.isOpen() && IsWindow()) {
				g_.setCurrent();
				a_.setCurrent();
				i_.setCurrent();
				return true;
			}
		}
		return false;
	}

	bool GameWindowThreaded::canDraw()
	{
		GameApp::canDraw();
		if(!posted_ || isRestored()) {
			HRslt hr = g_.impl()->TestCooperativeLevel();
			if(hr) {
				g_.setCurrent();
				return true;
			}
			else if(hr == D3DERR_DEVICELOST || hr == D3DERR_DEVICENOTRESET) {
				restore();
				boost::thread::yield();
			}
			else return false;
		}
		doSuspend();
		return false;
	}

	void GameWindowThreaded::present()
	{
		HRslt hr = g_.present();
		if(hr == D3DERR_DEVICELOST) canDraw();
		else boost::thread::yield();
		GameApp::present();
	}

#ifdef UNICODE
	bool GameWindowThreaded::create(const wchar_t *title, bool is_fs, uint mode)
#else
	bool GameWindowThreaded::create(const char *title, bool is_fs, uint mode)
#endif
	{
		if(GameWindow::create(title, is_fs, mode)) {
			thread_ = ThreadPtr(new boost::thread(boost::ref(*this)));
			return true;
		}
		return false;
	}

	bool GameWindowThreaded::create(HWND target, HWND msgwnd, HWND awnd, HWND iwnd, bool is_fs, uint mode)
	{
		if(GameWindow::create(target, msgwnd, awnd, iwnd, is_fs, mode)) {
			thread_ = ThreadPtr(new boost::thread(boost::ref(*this)));
			return true;
		}
		return false;
	}

	bool GameWindowThreaded::recreate()
	{
		if(suspend(10)) {
			g_.cleanUp();
			if(reset()) {
				g_.restore();
				resume();
				return true;
			}
			resume();
		}
		return false;
	}

	// ウィンドウモードとＦＳの切り替え
	void GameWindowThreaded::toggleFullscreen()
	{
		if(suspend(5)) {
			GameWindow::toggleFullscreen();
			resume();
		}
		else PRNN("game thread cant response");
	}

	void GameWindowThreaded::resize()
	{
		if(g_.isOpen() && !in_sizing_ && !is_fs_) {
			CRect rc;
			current_.GetWindowRect(&rc);
			if(wndrect_.Size()!=rc.Size()) {
				PRNN(":size=("<<wndrect_.Size().cx<<","<<wndrect_.Size().cy<<")->("<<rc.Size().cx<<","<<rc.Size().cy<<")");
				wndrect_ = rc;
				if(suspend(10)) {
					g_.cleanUp();
					if(reset()) g_.restore();
					resume();
				}
			}
		}
	}

	void GameWindowThreaded::onDestroy()
	{
		destroy();
		if(thread_.get()) thread_->join();
		GameWindow::onDestroy();
	}

	void GameWindowThreaded::onMouseMove(UINT keys, CPoint pt)
	{
		Lock al(monitor_);
		GameWindow::onMouseMove(keys, pt);
	}
	
	LRESULT GameWindowThreaded::onRestore(UINT /*msg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
	{
		if(!IsIconic()) {
			HRslt hr = g_.impl()->TestCooperativeLevel();
			if(hr == D3DERR_DEVICENOTRESET) {
				Lock al(monitor_);
				PRNN("try restore");
				g_.cleanUp();
				if(reset()) {
					g_.restore();
					PRNN("restore! "<<((is_fs_)?"true":"false"));
					restored();
				}
			}
		}
		posted_ = false;
		return S_OK;
	}
}} // namespace gctp::wtl
