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
#include <gctp/context.hpp>
#include <gctp/scene/camera.hpp>
#include <gctp/scene/quakecamera.hpp>
#include <gctp/scene/world.hpp>
#include <gctp/scene/entity.hpp>
#include <gctp/scene/motion.hpp>
#include <gctp/scene/motionmixer.hpp>

#include <mbstring.h>

#include <assert.h>

extern "C" int main(int argc, char *argv[]);

extern gctp::Pointer<gctp::core::Context> main_context;

class ToolWindow
	: public gctp::Object
	, public gctp::GameApp
	, public SmartWin::WidgetFactory<SmartWin::WidgetWindow, ToolWindow>
{
	typedef ToolWindow Self;

public:
	ToolWindow()
		: is_fs_(false), mode_(0), is_closing_(false), device_ok_(true)
		, cursor_(::LoadCursor(NULL, IDC_ARROW)), is_cursor_visible_(true), do_hold_cursor_(false), do_close_(false), can_change_mode_(true)
		, is_suspending_(false), req_suspend_(false)
	{
		deleteGuard();
		filereq_slot.bind(this);
	}

	WidgetMenuPtr mainmenu;

	void setUp()
	{
		Seed cs;
		cs.style = WS_THICKFRAME|WS_VISIBLE|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX;
		//cs.menuName = _T("IDR_MAINMENU"); // 面度印でソースに書く。よりによってメニューにだけサブクラス化を怠ってやがった
		createWindow(cs);
		setText(_T("SceneGraphFileViewer"));

		// Creating main menu
		mainmenu = createMenu();
		WidgetMenuPtr file = mainmenu->appendPopup( _T( "ファイル(&F)" ) );
		file->appendItem( 1, _T( "開く…(&O)" ), & Self::menuEventHandler );
		//compile tests(?これなんだ？)
		file->isSystemMenu();
		file->getCount();
		mainmenu->attach( this );

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
		//onRaw( &Self::doOnSysChar, sw::Message(WM_SYSCHAR) );

		//sw::Application::instance().setHeartBeatFunction(this);

		gctp::HRslt hr;
		if(getParent()) hr = g_.open(handle(), getParent()->handle());
		else hr = g_.open(handle());
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

		game_thread_ = fork( SmartUtil::tstring(_T("GameThread")), &Self::runInThread );
	}

	SmartUtil::tstring filepath;
	void menuEventHandler( WidgetMenuPtr menu, unsigned item )
	{
		WidgetLoadFile dlg = createLoadFile();
		dlg.addFilter( _T( "X Files" ), _T( "*.x" ) );
		dlg.addFilter( _T( "Collada Files" ), _T( "*.collada" ) );
		dlg.addFilter( _T( "FBX Files" ), _T( "*.fbx" ) );
		dlg.addFilter( _T( "All Files" ), _T( "*.*" ) );
		dlg.activeFilter( 0 );
		filepath = dlg.showDialog();
		update_signal.connectOnce(filereq_slot);
	}

	/// 更新
	bool onFileRequest(float delta)
	{
		main_context = 0;
		main_context = gctp::core::Context::current().newChild();
		gctp::core::Context &context = *main_context;
		gctp::Pointer<gctp::scene::World> world = context[_T("world")].lock();
		gctp::Pointer<gctp::scene::Entity> entity;
		entity = newEntity(context, *world, "gctp.scene.Entity", _T("chara"), filepath.c_str()).lock();
		if(entity) {
			entity->target()->update();
			entity->target()->print(gctp::dbgout);
			PRNN(entity->target()->bs().c.x << ", " << entity->target()->bs().c.y << ", " << entity->target()->bs().c.z << ", " << entity->target()->bs().r);
		}
		if(entity && entity->hasMotionMixer()) {
			for(std::size_t i = 0; i < entity->mixer().tracks().size(); i++) {
				entity->mixer().tracks()[i].setWeight(i == 0 ? 1.0f : 0.0f);
				entity->mixer().tracks()[i].setSpeed(i == 0 ? 1.0f : 0.0f);
				entity->mixer().tracks()[i].setLoop(true);
			}
		}
		return false;
	}
	/// 更新スロット
	gctp::MemberSlot1<Self, float /*delta*/, &Self::onFileRequest> filereq_slot;

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
		return hr ? true : false;
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
		if(!device_ok_) ::PostMessage(handle(), WM_SETCURSOR, 0, 0); // checkDeviceStatusをさせるために
		D3DRASTER_STATUS status;
		D3DDISPLAYMODE mode;
		if(gctp::HRslt(g_.impl()->GetRasterStatus(0, &status))&&gctp::HRslt(g_.impl()->GetDisplayMode(0, &mode))) {
			if(!status.InVBlank && status.ScanLine < mode.Height) {
				if(mode.RefreshRate==0) mode.RefreshRate = 60; // 面倒なので60Hzと仮定
				DWORD sleeptime = (DWORD)(((mode.Height-status.ScanLine)*1000)/(mode.Height * (mode.RefreshRate/2)));
				//PRNN("sleeptime "<<sleeptime<<","<<mode.Height<<","<<status.ScanLine<<","<<mode.RefreshRate);
				if(sleeptime) ::Sleep(sleeptime);
				// これムービー用の処理？もう忘れた
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

	virtual void showCursor(bool yes)
	{
		is_cursor_visible_ = yes;
		::PostMessage(handle(), WM_SETCURSOR, 0, HTCLIENT); // 即反映させるため
	}
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
		if(!is_closing_) {
			PRNN("###########resize");
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
			if(reset()) {
				g_.restore();
				PRNN("resize done! "<<((is_fs_)?"true":"false"));
				is_suspending_ = false;
				//game_thread_.resume();
			}
		}
	}

	HRESULT doOnSetCursor(LPARAM lParam, WPARAM wParam)
	{
		checkDeviceStatus();
		if(LOWORD(lParam) == HTCLIENT) {
			if(is_cursor_visible_) {
				if(cursor_) ::SetCursor(cursor_);
				else return returnFromUnhandledWindowProc(handle(), WM_SETCURSOR, wParam, lParam);
			}
			else {
				::SetCursor(NULL);
			}
			return S_OK;
		}
		return returnFromUnhandledWindowProc(handle(), WM_SETCURSOR, wParam, lParam);
	}

	bool doOnKeyPressed(int key)
	{
		return false;
	}

	HRESULT doOnSysChar(LPARAM lparam, WPARAM vkey)
	{
		//if(vkey == VK_RETURN && can_change_mode_) toggleFullscreen();
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
		//input().mouse().setPoint(mouse.pos.x, mouse.pos.y);
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
		//gctp::CmdLine arg(sw::Application::instance().getCommandLine().getParamsRaw());
		gctp::CmdLine arg;
		for(size_t i = 0; i < sw::Application::instance().getCommandLine().getParams().size(); i++) {
			arg.push(sw::Application::instance().getCommandLine().getParams()[i].c_str());
		}
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

}

int SmartWinMain(Application & app)
{
	locale::global(locale(locale::classic(), locale(""), LC_CTYPE));
	initialize(app.getAppHandle());
	ToolWindow *window = new ToolWindow;
	window->setUp();
	return app.run();
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
# pragma comment(lib, "libbulletcollision_d.lib")
# pragma comment(lib, "libbulletmath_d.lib")
#else
# pragma comment(lib, "lua.lib")
# pragma comment(lib, "d3dx9.lib")
# pragma comment(lib, "libbulletcollision.lib")
# pragma comment(lib, "libbulletmath.lib")
#endif
