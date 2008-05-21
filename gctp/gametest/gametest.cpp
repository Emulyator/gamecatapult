/**@file
 * 統合テスト
 * 
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/26 23:18:14
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "stdafx.h"

#include "resource.h"
#include <gctp/dbgout.hpp>
#include <gctp/cmdline.hpp>
#include <gctp/timer.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/graphic/texture.hpp>
#include <gctp/audio.hpp>
#include <gctp/input.hpp>
#include <gctp/app.hpp>

#include <mbstring.h>

extern "C" int main(int argc, char *argv[]);

class GameWindow
	: public gctp::GameApp
	, public SmartWin::WidgetFactory<SmartWin::WidgetWindow, GameWindow>
{
	typedef GameWindow Self;

	sw::BitmapPtr bitmap_;
	WidgetStaticPtr banner_;

public:
	GameWindow()
		: is_fs_(false), mode_(0), is_closing_(false), device_ok_(true)
		, cursor_(::LoadCursor(NULL, IDC_ARROW)), is_cursor_visible_(true), do_hold_cursor_(false), do_close_(false), can_change_mode_(true)
		, is_suspending_(false), req_suspend_(false)
	{}

	void splash(const _TCHAR *title, DWORD bitmap_rc)
	{
		sw::BitmapPtr bitmap_ = sw::BitmapPtr(new sw::Bitmap(bitmap_rc));
		sw::Point screen = getDesktopSize();
		sw::Point sz = bitmap_->getBitmapSize();
		{
			Seed cs;
			cs.style = WS_POPUP|WS_BORDER;
			cs.exStyle = 0;
			cs.location = sw::Rectangle((screen.x-sz.x)/2, (screen.y-sz.y)/2, sz.x, sz.y);
			createWindow(cs);
			setText(title);
		}
		{
			WidgetStatic::Seed cs;
			cs.style = SS_BITMAP | WS_VISIBLE;
			cs.location = sw::Rectangle(0, 0, sz.x, sz.y);
			banner_ = createStatic(cs);
			banner_->setBitmap(bitmap_);
		}
		animateBlend( true, 500 );
		animateBlend( true, 500 ); // 何で二度やらないと上手くいかない場合があるんだ？
	}

	void setUp(bool is_fs, gctp::uint mode_no)
	{
		banner_->setVisible(false);
		gctp::HRslt hr;
		const D3DDISPLAYMODE &mode = gctp::graphic::dx::adapters()[0].modes[mode_no];
		{
			style_backup_ = WS_OVERLAPPED|WS_VISIBLE|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX;
			exstyle_backup_ = 0;
			sw::Point screen = getDesktopSize();
			RECT rc = {0, 0, mode.Width, mode.Height};
			::AdjustWindowRectEx(&rc, style_backup_, FALSE, exstyle_backup_);
			DWORD width = rc.right-rc.left;
			DWORD height = rc.bottom-rc.top;
			location_backup_ = sw::Rectangle((screen.x-width)/2, (screen.y-height)/2, width, height);
		}
		if(is_fs) {
			::SetWindowLong(handle(), GWL_STYLE, 0);
			::SetWindowLong(handle(), GWL_EXSTYLE, 0);
			setBounds(0, 0, mode.Width, mode.Height);
			if(getParent()) hr = g_.open(handle(), getParent()->handle(), 0, mode_no);
			else hr = g_.open(handle(), 0, mode_no);
			maximize();
		}
		else {
			::SetWindowLong(handle(), GWL_STYLE, style_backup_);
			::SetWindowLong(handle(), GWL_EXSTYLE, exstyle_backup_);
			setBounds(location_backup_);
			if(getParent()) hr = g_.open(handle(), getParent()->handle());
			else hr = g_.open(handle());
		}
		is_fs_ = is_fs;
		mode_ = mode_no;
		if(!hr) {
			GCTP_TRACE(hr);
			close();
			return;
		}
		hr = a_.open(handle());
		if(!hr) {
			GCTP_TRACE(hr);
			close();
			return;
		}
		hr = i_.setUp(handle());
		if(!hr) {
			GCTP_TRACE(hr);
			close();
			return;
		}
		
		g_.setCurrent();
		a_.setCurrent();
		i_.setCurrent();
		setCurrent();

		// handlers
		onClosing( &Self::doOnClosing );
		onSized( &Self::doOnResized );
		onLeftMouseUp( &Self::doOnLeftMouseUp );
		onLeftMouseDown( &Self::doOnLeftMouseDown );
		onMiddleMouseUp( &Self::doOnMiddleMouseUp );
		onMiddleMouseDown( &Self::doOnMiddleMouseDown );
		onRightMouseUp( &Self::doOnRightMouseUp );
		onRightMouseDown( &Self::doOnRightMouseDown );
		onLeftMouseDblClick( &Self::doOnLeftMouseDblClick );
		onRightMouseDblClick( &Self::doOnRightMouseDblClick );
		onMouseMove( &Self::doOnMouseMove );
		onKeyPressed( &Self::doOnKeyPressed );
		onRaw( &Self::doOnSetCursor, sw::Message(WM_SETCURSOR) );
		onRaw( &Self::doOnSysChar, sw::Message(WM_SYSCHAR) );

		//sw::Application::instance().setHeartBeatFunction(this);
		game_thread_ = fork( SmartUtil::tstring(_T("GameThread")), &Self::runInThread );
	}

	bool reset()
	{
		gctp::HRslt hr;
		if(is_fs_) {
			hr = g_.open(handle(), 0, mode_);
			if(!hr) {
				PRNN("フルスクリーンで製作できませんでした。:"<<hr);
				is_fs_ = false;
				hr = g_.open(handle());
			}
		}
		else {
			hr = g_.open(handle());
		}
		return hr;
	}

	virtual HWND getHandle() const
	{
		return handle();
	}

	virtual bool canContinue()
	{
		::Sleep(0); // yield
		if(req_suspend_) {
			req_suspend_ = false;
			is_suspending_ = true;
			while(is_suspending_) ::Sleep(10);
		}
		if(!is_closing_ && g_.isOpen() && GameApp::canContinue()) {
			g_.setCurrent();
			a_.setCurrent();
			i_.setCurrent();
			if(::IsWindow(handle())) i_.update();
			return true;
		}
		return false;
	}

	virtual bool canDraw()
	{
		if(GameApp::canDraw() && !::IsIconic(handle()) && device_ok_) {
			g_.setCurrent();
			gctp::HRslt hr = g_.impl()->TestCooperativeLevel();
			if(hr) return true;
			else device_ok_ = false;
		}
		if(!device_ok_) ::SendMessage(handle(), WM_SETCURSOR, 0, 0); // checkDeviceStatusをさせるために
		D3DRASTER_STATUS status;
		D3DDISPLAYMODE mode;
		if(gctp::HRslt(g_.impl()->GetRasterStatus(0, &status))&&gctp::HRslt(g_.impl()->GetDisplayMode(0, &mode))) {
			if(!status.InVBlank && status.ScanLine < mode.Height) {
				if(mode.RefreshRate==0) mode.RefreshRate = 60; // 面倒なので60Hzと仮定
				DWORD sleeptime = (DWORD)(((mode.Height-status.ScanLine)*1000)/(mode.Height * (mode.RefreshRate/2)));
				//PRNN("sleeptime "<<sleeptime<<","<<mode.Height<<","<<status.ScanLine<<","<<mode.RefreshRate);
				if(sleeptime) ::Sleep(sleeptime);
			}
		}
		return false;
	}

	virtual void present()
	{
		if(device_ok_) {
			gctp::HRslt hr = g_.present();
			if(!hr) {
				hr = g_.impl()->TestCooperativeLevel();
				if(!hr) device_ok_ = false;
			}
			GameApp::present();
		}
	}

	virtual void showCursor(bool yes) { is_cursor_visible_ = yes; }
	virtual void holdCursor(bool yes) { do_hold_cursor_ = yes; }

protected:
	bool doOnClosing()
	{
		is_closing_ = true;
		game_thread_.resume();
		sw::Utilities::Thread::waitForObject(game_thread_);

		g_.close();
		a_.close();
		i_.tearDown();
		return true;
	}

	void doOnResized(const sw::WidgetSizedEventResult & sz)
	{
		GCTP_TRACE("onResized "<<sz.newSize.x<<","<<sz.newSize.y);
	}

	HRESULT doOnSetCursor(LPARAM lParam, WPARAM wParam)
	{
		checkDeviceStatus();
		if(LOWORD(lParam) == HTCLIENT) {
			if(is_cursor_visible_) {
				if(cursor_) ::SetCursor(cursor_);
			}
			else {
				::SetCursor(NULL);
			}
		}
		return S_OK;
	}

	bool doOnKeyPressed(int key)
	{
		return false;
	}

	HRESULT doOnSysChar(LPARAM lparam, WPARAM vkey)
	{
		if(vkey == VK_RETURN && can_change_mode_) toggleFullscreen();
		return S_OK;
	}

	void doOnLeftMouseDown(const sw::MouseEventResult &mouse)
	{
		gctp::uint8_t opt = 0;
		switch(mouse.ButtonPressed) {
			case sw::MouseEventResult::RIGHT: opt |= gctp::GUIEvents::RB; break;
			case sw::MouseEventResult::MIDDLE: opt |= gctp::GUIEvents::MB; break;
			case sw::MouseEventResult::LEFT: opt |= gctp::GUIEvents::LB; break;
		}
		if(mouse.isControlPressed) opt |= gctp::GUIEvents::CTRL;
		if(mouse.isShiftPressed) opt |= gctp::GUIEvents::SHIFT;
		if(mouse.isAltPressed) opt |= gctp::GUIEvents::ALT;
		gctp::GUIEvents::postDownMsg(events(), getTimeStamp(), gctp::Point2C(mouse.pos.x, mouse.pos.y), gctp::GUIEvents::LB, opt);
	}

	void doOnLeftMouseUp(const sw::MouseEventResult &mouse)
	{
		gctp::uint8_t opt = 0;
		switch(mouse.ButtonPressed) {
			case sw::MouseEventResult::RIGHT: opt |= gctp::GUIEvents::RB; break;
			case sw::MouseEventResult::MIDDLE: opt |= gctp::GUIEvents::MB; break;
			case sw::MouseEventResult::LEFT: opt |= gctp::GUIEvents::LB; break;
		}
		if(mouse.isControlPressed) opt |= gctp::GUIEvents::CTRL;
		if(mouse.isShiftPressed) opt |= gctp::GUIEvents::SHIFT;
		if(mouse.isAltPressed) opt |= gctp::GUIEvents::ALT;
		gctp::GUIEvents::postUpMsg(events(), getTimeStamp(), gctp::Point2C(mouse.pos.x, mouse.pos.y), gctp::GUIEvents::LB, opt);
	}

	void doOnMiddleMouseDown(const sw::MouseEventResult &mouse)
	{
		gctp::uint8_t opt = 0;
		switch(mouse.ButtonPressed) {
			case sw::MouseEventResult::RIGHT: opt |= gctp::GUIEvents::RB; break;
			case sw::MouseEventResult::MIDDLE: opt |= gctp::GUIEvents::MB; break;
			case sw::MouseEventResult::LEFT: opt |= gctp::GUIEvents::LB; break;
		}
		if(mouse.isControlPressed) opt |= gctp::GUIEvents::CTRL;
		if(mouse.isShiftPressed) opt |= gctp::GUIEvents::SHIFT;
		if(mouse.isAltPressed) opt |= gctp::GUIEvents::ALT;
		gctp::GUIEvents::postDownMsg(events(), getTimeStamp(), gctp::Point2C(mouse.pos.x, mouse.pos.y), gctp::GUIEvents::MB, opt);
	}

	void doOnMiddleMouseUp(const sw::MouseEventResult &mouse)
	{
		gctp::uint8_t opt = 0;
		switch(mouse.ButtonPressed) {
			case sw::MouseEventResult::RIGHT: opt |= gctp::GUIEvents::RB; break;
			case sw::MouseEventResult::MIDDLE: opt |= gctp::GUIEvents::MB; break;
			case sw::MouseEventResult::LEFT: opt |= gctp::GUIEvents::LB; break;
		}
		if(mouse.isControlPressed) opt |= gctp::GUIEvents::CTRL;
		if(mouse.isShiftPressed) opt |= gctp::GUIEvents::SHIFT;
		if(mouse.isAltPressed) opt |= gctp::GUIEvents::ALT;
		gctp::GUIEvents::postUpMsg(events(), getTimeStamp(), gctp::Point2C(mouse.pos.x, mouse.pos.y), gctp::GUIEvents::MB, opt);
	}

	void doOnRightMouseDown(const sw::MouseEventResult &mouse)
	{
		gctp::uint8_t opt = 0;
		switch(mouse.ButtonPressed) {
			case sw::MouseEventResult::RIGHT: opt |= gctp::GUIEvents::RB; break;
			case sw::MouseEventResult::MIDDLE: opt |= gctp::GUIEvents::MB; break;
			case sw::MouseEventResult::LEFT: opt |= gctp::GUIEvents::LB; break;
		}
		if(mouse.isControlPressed) opt |= gctp::GUIEvents::CTRL;
		if(mouse.isShiftPressed) opt |= gctp::GUIEvents::SHIFT;
		if(mouse.isAltPressed) opt |= gctp::GUIEvents::ALT;
		gctp::GUIEvents::postDownMsg(events(), getTimeStamp(), gctp::Point2C(mouse.pos.x, mouse.pos.y), gctp::GUIEvents::RB, opt);
	}

	void doOnRightMouseUp(const sw::MouseEventResult &mouse)
	{
		gctp::uint8_t opt = 0;
		switch(mouse.ButtonPressed) {
			case sw::MouseEventResult::RIGHT: opt |= gctp::GUIEvents::RB; break;
			case sw::MouseEventResult::MIDDLE: opt |= gctp::GUIEvents::MB; break;
			case sw::MouseEventResult::LEFT: opt |= gctp::GUIEvents::LB; break;
		}
		if(mouse.isControlPressed) opt |= gctp::GUIEvents::CTRL;
		if(mouse.isShiftPressed) opt |= gctp::GUIEvents::SHIFT;
		if(mouse.isAltPressed) opt |= gctp::GUIEvents::ALT;
		gctp::GUIEvents::postUpMsg(events(), getTimeStamp(), gctp::Point2C(mouse.pos.x, mouse.pos.y), gctp::GUIEvents::RB, opt);
	}

	void doOnLeftMouseDblClick(const sw::MouseEventResult &mouse)
	{
		gctp::uint8_t opt = 0;
		switch(mouse.ButtonPressed) {
			case sw::MouseEventResult::RIGHT: opt |= gctp::GUIEvents::RB; break;
			case sw::MouseEventResult::MIDDLE: opt |= gctp::GUIEvents::MB; break;
			case sw::MouseEventResult::LEFT: opt |= gctp::GUIEvents::LB; break;
		}
		if(mouse.isControlPressed) opt |= gctp::GUIEvents::CTRL;
		if(mouse.isShiftPressed) opt |= gctp::GUIEvents::SHIFT;
		if(mouse.isAltPressed) opt |= gctp::GUIEvents::ALT;
		gctp::GUIEvents::postDblClickMsg(events(), getTimeStamp(), gctp::Point2C(mouse.pos.x, mouse.pos.y), gctp::GUIEvents::LB, opt);
	}

	void doOnRightMouseDblClick(const sw::MouseEventResult &mouse)
	{
		gctp::uint8_t opt = 0;
		switch(mouse.ButtonPressed) {
			case sw::MouseEventResult::RIGHT: opt |= gctp::GUIEvents::RB; break;
			case sw::MouseEventResult::MIDDLE: opt |= gctp::GUIEvents::MB; break;
			case sw::MouseEventResult::LEFT: opt |= gctp::GUIEvents::LB; break;
		}
		if(mouse.isControlPressed) opt |= gctp::GUIEvents::CTRL;
		if(mouse.isShiftPressed) opt |= gctp::GUIEvents::SHIFT;
		if(mouse.isAltPressed) opt |= gctp::GUIEvents::ALT;
		gctp::GUIEvents::postDblClickMsg(events(), getTimeStamp(), gctp::Point2C(mouse.pos.x, mouse.pos.y), gctp::GUIEvents::RB, opt);
	}

	void doOnMouseMove(const sw::MouseEventResult &mouse)
	{
		i_.mouse().setPoint(mouse.pos.x, mouse.pos.y);
		if(do_hold_cursor_ /*&& ::GetFocus() == m_hWnd*/) {
			if(mouse.pos.x < getSize().x/3
			|| mouse.pos.x > getSize().x*2/3
			|| mouse.pos.y < getSize().y/3
			|| mouse.pos.y > getSize().y*2/3)
			{
				sw::Point center = getPosition();
				center.x += getSize().x/2;
				center.y += getSize().y/2;
				::SetCursorPos(center.x, center.y);
			}
		}
	}

	// ウィンドウモードとＦＳの切り替え
	void toggleFullscreen()
	{
		if(!is_closing_) {
			PRNN("###########toggleFullscreen");
			req_suspend_ = true;
			is_suspending_ = false;
			for(int i = 0; i < 50; i++) {
				if(is_suspending_) break;
				::Sleep(100);
			}
			if(!is_suspending_) {
				req_suspend_ = false;
				is_suspending_ = false;
				PRNN("Game Thread do not response.");
				return;
			}
			//game_thread_.suspend();
			g_.cleanUp();
			if(is_fs_) {
				restore();
			}
			else {
				location_backup_ = getBounds();
				style_backup_ = ::GetWindowLong(handle(), GWL_STYLE);
				exstyle_backup_ = ::GetWindowLong(handle(), GWL_EXSTYLE);
				const D3DDISPLAYMODE &mode = gctp::graphic::dx::adapters()[0].modes[mode_];
				setBounds(0, 0, mode.Width, mode.Height, false);
				::SetWindowLong(handle(), GWL_STYLE, 0);
				::SetWindowLong(handle(), GWL_EXSTYLE, 0);
			}
			is_fs_ = !is_fs_;
			if(reset()) {
				if(is_fs_) {
					maximize();
				}
				else {
					::SetWindowLong(handle(), GWL_STYLE, style_backup_);
					::SetWindowLong(handle(), GWL_EXSTYLE, exstyle_backup_);
					setBounds(location_backup_);
				}
				g_.restore();
				PRNN("toggle done! "<<((is_fs_)?"true":"false"));
				is_suspending_ = false;
				//game_thread_.resume();
			}
		}
	}
	
	void checkDeviceStatus()
	{
		if(!is_closing_) {
			if(do_close_) {
				do_close_ = false;
				close();
				return;
			}
			if(!device_ok_) {
				gctp::HRslt hr = g_.impl()->TestCooperativeLevel();
				if(hr == D3DERR_DEVICENOTRESET) {
					g_.cleanUp();
					if(reset()) {
						g_.restore();
						PRNN("restore! "<<((is_fs_)?"true":"false"));
						device_ok_ = true;
					}
				}
			}
		}
	}

	virtual void tick()
	{
		checkDeviceStatus();
	}

	unsigned long runInThread( SmartUtil::tstring & message )
	{
		setCurrent();
		g_.setCurrent();
		a_.setCurrent();
		i_.setCurrent();
		gctp::CmdLine arg(sw::Application::instance().getCommandLine().getParamsRaw());
		unsigned long ret = (unsigned long)main(arg.argc(), arg.argv());
		if(!is_closing_) do_close_ = true;
		return ret;
	}

private:
	gctp::graphic::Device g_;
	gctp::audio::Device a_;
	gctp::Input i_;
	sw::Utilities::Thread game_thread_;

	HCURSOR cursor_;
	bool is_fs_;
	gctp::uint mode_;
	bool is_cursor_visible_;
	bool do_hold_cursor_;
	bool is_closing_;
	bool do_close_;
	bool device_ok_;
	bool can_change_mode_;
	bool is_suspending_;
	bool req_suspend_;
	SmartWin::Rectangle location_backup_;
	DWORD style_backup_;
	DWORD exstyle_backup_;
};

class StartupDialog
	: public SmartWin::WidgetFactory<SmartWin::WidgetModalDialog, StartupDialog, SmartWin::MessageMapPolicyModalDialogWidget>
{
	typedef StartupDialog Self;

public:
	std::vector<DWORD> modes;
	bool is_fs;
	int mode;

	StartupDialog(bool is_fs, Widget *parent = 0)
		: Widget( parent ), is_fs(is_fs), mode(0)
	{
		onInitDialog(&StartupDialog::setUp);
	}

	bool setUp()
	{
		screens_ = subclassComboBox( IDC_SCREENLIST );

		//fullscreen_ = subclassRadioButton( IDC_RADIO_FSMODE );
		::CheckRadioButton(handle(), IDC_RADIO_FSMODE, IDC_RADIO_WNDMODE, (is_fs)?IDC_RADIO_FSMODE:IDC_RADIO_WNDMODE);
		
		for(DWORD i = 0; i < modes.size(); i++) {
			const D3DDISPLAYMODE &mode = gctp::graphic::dx::adapters()[D3DADAPTER_DEFAULT].modes[modes[i]];
			addToComboBox(modes[i], mode.Width, mode.Height, mode.Format, mode.RefreshRate);
		}
		if(screens_->getCount()>0) screens_->setSelectedIndex(0);
		else screens_->setEnabled(false);

		WidgetButtonPtr btn;
		btn = subclassButton( IDOK );
		if(screens_->getCount()==0) btn->setEnabled(false);
		else {
			btn->onClicked( &Self::doOnOk );
			btn->setFocus();
		}
		btn = subclassButton( IDCANCEL );
		btn->onClicked( &Self::doOnCancel );
		if(screens_->getCount()==0) btn->setFocus();

		setVisible(true);
		return true;
	}

private:
	void addToComboBox(DWORD data, DWORD width, DWORD height, DWORD format, DWORD refresh_rate)
	{
		_TCHAR *str;
		switch(format) {
		case D3DFMT_R8G8B8: case D3DFMT_A8R8G8B8: case D3DFMT_X8R8G8B8:
			str = _T("フルカラー");
			break;
		case D3DFMT_R5G6B5: case D3DFMT_X1R5G5B5: case D3DFMT_A1R5G5B5: case D3DFMT_A4R4G4B4:
		case D3DFMT_R3G3B2: case D3DFMT_A8R3G3B2: case D3DFMT_X4R4G4B4:
			str = _T("ハイカラー");
			break;
		default:
			return;
		};
		_TCHAR screenstr[256];
		if(refresh_rate==D3DPRESENT_RATE_DEFAULT) _stprintf(screenstr, _T("%d×%d:%s:規定値"), width, height, str);
		else _stprintf(screenstr, _T("%d×%d:%s:%dHz"), width, height, str, refresh_rate);
		int idx = screens_->addValue(screenstr);
		screen_map_[idx] = data;
	}

	void doOnOk( WidgetButtonPtr btn )
	{
		//is_fs = fullscreen_->getChecked();
		is_fs = (IsDlgButtonChecked(handle(), IDC_RADIO_FSMODE)==BST_CHECKED);
		mode = screen_map_[screens_->getSelectedIndex()];
		endDialog( IDOK );
	}

	void doOnCancel( WidgetButtonPtr btn )
	{
		endDialog( IDCANCEL );
	}

	std::map<int, DWORD> screen_map_;
	WidgetComboBoxPtr screens_;
	//WidgetRadioButtonPtr fullscreen_; // 直接WinAPI呼んだほうが便利
};

using namespace std;
using namespace gctp;
using namespace SmartWin;

namespace {

	void initialize(HINSTANCE hinst)
	{
		Timer::initialize();
//		graphic::allowHardwareVertexProcessing(true);
		graphic::allowStrictMultiThreadSafe(true);
//		graphic::setIntervalTime(1);
		graphic::initialize();
		Input::initialize(hinst);

		PRNN(_T("列挙されたアダプタ"));
		for(graphic::dx::AdapterList::const_iterator i = graphic::dx::adapters().begin(); i != graphic::dx::adapters().end(); ++i) {
			PRNN(*i);
		}
		PRNN(_T("列挙された入力デバイス"));
		for(Input::DeviceList::const_iterator i = Input::devicies().begin(); i != Input::devicies().end(); ++i) {
			PRNN(*i);
		}
	}

	bool startupdialog(Widget *parent, bool &is_fs, uint &mode)
	{
		StartupDialog dlg(false, parent);

		int mode_num = (int)graphic::dx::adapters()[D3DADAPTER_DEFAULT].modes.size();
		for(int i = 0; i < mode_num; i++) {
			const D3DDISPLAYMODE &mode = graphic::dx::adapters()[D3DADAPTER_DEFAULT].modes[i];
//			if(mode.Width==640 && mode.Height==480) {
			if(mode.Width>=800 && mode.Height>=600 && mode.RefreshRate==D3DPRESENT_RATE_DEFAULT) {
				switch(mode.Format) {
				case D3DFMT_R8G8B8: case D3DFMT_A8R8G8B8: case D3DFMT_X8R8G8B8:
					dlg.modes.push_back(i);
					break;
				case D3DFMT_R5G6B5: case D3DFMT_X1R5G5B5: case D3DFMT_A1R5G5B5: case D3DFMT_A4R4G4B4:
				case D3DFMT_R3G3B2: case D3DFMT_A8R3G3B2: case D3DFMT_X4R4G4B4:
					dlg.modes.push_back(i);
					break;
				}
			}
		}

		int ret = dlg.createDialog( IDD_STARTUPDLG_DIALOG );
		is_fs = dlg.is_fs;
		mode = dlg.mode;
		return (ret == IDOK)? true : false;
	}

}

int SmartWinMain(Application & app)
{
	locale::global(locale(locale::classic(), locale(""), LC_CTYPE));
	initialize(app.getAppHandle());
	GameWindow *window = new GameWindow;
	window->splash(_T("GameCatapult DEMO"), IDB_BITMAP1);
	bool is_fs = false; uint mode = 0;
	if(startupdialog(window, is_fs, mode)) {
		window->setUp(is_fs, mode);
		return app.run();
	}
	return 1; // CANCEL
}

#ifdef _UNICODE
# ifdef _DEBUG
#  pragma comment(lib, "SmartWinDU.lib")
# else
#  pragma comment(lib, "SmartWinU.lib")
# endif
#else
# ifdef _DEBUG
#  pragma comment(lib, "SmartWinD.lib")
# else
#  pragma comment(lib, "SmartWin.lib")
# endif
#endif

#pragma comment(lib, "DxErr9.lib")
#pragma comment(lib, "amstrmid.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dxof.lib")
#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dinput.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dmoguids.lib")
#pragma comment(lib, "dplayx.lib")
#pragma comment(lib, "dsetup.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxtrans.lib")
#pragma comment(lib, "ksproxy.lib")
#pragma comment(lib, "ksuser.lib")
#pragma comment(lib, "msdmo.lib")
#pragma comment(lib, "quartz.lib")
#pragma comment(lib, "strmiids.lib")
#ifdef _DEBUG
# pragma comment(lib, "luad.lib")
# pragma comment(lib, "d3dx9d.lib")
#else
# pragma comment(lib, "lua.lib")
# pragma comment(lib, "d3dx9.lib")
#endif
