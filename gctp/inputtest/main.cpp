/**@file
 * パッドなど入力機能のテスト
 * @author osamu takasugi
 * @date 2003/07/24 14:00:31
 * $Id: main.cpp,v 1.7 2003/08/26 01:26:14 SAM Exp $
 */
#include "stdafx.h"
#include "resource.h"
#include <gctp/dbgout.hpp>
#include <gctp/input.hpp>
#include <strstream>  
#include "SmartWin.h"

using namespace gctp;
using namespace std;

#if 1
class TestDialog
	: public SmartWin::WidgetFactory<SmartWin::WidgetDialog, TestDialog, SmartWin::DialogWidget>
{
	typedef TestDialog Self;

public:
	TestDialog()
	{
		createDialog( IDD_DIALOG1 );

		onClosing( &Self::doOnClosing );

		// "Morphing" the window into sight...
		//animateBlend( true, 1000 );
		setVisible(true);

		text_ = subclassStatic( IDC_STATIC1 );
		WidgetButtonPtr btn1 = subclassButton( IDOK );
		WidgetButtonPtr btn2 = subclassButton( IDCANCEL );
		btn1->onClicked( &Self::doOnClicked );
		btn1->setFocus();
		btn2->onClicked( &Self::doOnClicked );
		onMouseMove( &Self::doOnMouseMove );

		HRslt hr = i_.setUp(handle());
		if(!hr) GCTP_TRACE(hr);
		i_.setCurrent();

		sw::Command com( _T("Update") );
		createTimer(&Self::onTimer, 16, com);
		PRNN("Create Done");
	}

private:
	void doOnMouseMove(const sw::MouseEventResult &mouse)
	{
		/*
		MK_CONTROL   Ctrl キーが押されている場合にセットします。
		MK_LBUTTON   マウスの左ボタンが押されている場合にセットします。
		MK_MBUTTON   マウスの中央ボタンが押されている場合にセットします。
		MK_RBUTTON   マウスの右ボタンが押されている場合にセットします。
		MK_SHIFT     Shift キーが押されている場合にセットします。
		*/
		i_.mouse().setPoint(mouse.pos.x, mouse.pos.y);
	}

	void doOnClicked( WidgetButtonPtr btn )
	{
		close();
	}

	bool doOnClosing()
	{
		PRNN("Closed");
		i_.tearDown();
		return true;
	}

	void onTimer(const sw::CommandPtr &p)
	{
		HRslt hr = i_.update();
		std::strstream str;
		str << "input status " << input().mouse().x << " "<< input().mouse().y << endl;
		str << input().mouse().dx << " "<< input().mouse().dy << " " << input().mouse().dz << endl;
		str << " " << input().mouse().press[0] << " "<< input().mouse().press[1] << " " << input().mouse().press[2] << endl;
		str << " A=" << input().kbd().press(DIK_A) << " S=" << input().kbd().press(DIK_S) << endl;
		str << hr << ends;
		text_->setText(str.str());
		text_->updateWidget();

		sw::Command com( _T("Update") );
		createTimer(&Self::onTimer, 16, com);
	}

	Input i_;
	WidgetStaticPtr text_;
};

int SmartWinMain( SmartWin::Application & app )
{
	Input::initialize(app.getAppHandle());
	PRNN("列挙されたデバイス");
	for(Input::DeviceList::const_iterator i = Input::devicies().begin(); i != Input::devicies().end(); i++) {
		PRNN(*i);
	}
	new TestDialog();
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

#else
class Dialog : public CDialogImpl<Dialog> {
	Input i_;
	UINT timer_;
	CStatic text_;
public:
	enum {IDD = IDD_DIALOG1};
	Dialog() : timer_(0) {}

private:
	BEGIN_MSG_MAP_EX(Dialog)
		MSG_WM_INITDIALOG(onInitDialog)
		COMMAND_ID_HANDLER(IDOK, onCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, onCloseCmd)
		MSG_WM_MOUSEMOVE(onMouseMove)
		MSG_WM_TIMER(onTimer)
		MSG_WM_DESTROY(onDestroy)
	END_MSG_MAP()

	LRESULT onInitDialog(HWND /*forcus*/, LPARAM /*initparam*/)
	{
		timer_ = SetTimer(1, 33);
		CenterWindow();
		ShowWindow(SW_SHOW);
		UpdateWindow();
		RECT rc;
		GetClientRect(&rc);
		text_ = GetDlgItem(IDC_STATIC1);
		ATLASSERT(text_.IsWindow());
		HRslt hr = i_.setUp(m_hWnd);
		if(!hr) GCTP_TRACE(hr);
		i_.setCurrent();
		return S_OK;
	}

	LRESULT onCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(wID);
		return 0;
	}
	
	void onMouseMove(UINT keys, CPoint pt)
	{
		/*
		MK_CONTROL   Ctrl キーが押されている場合にセットします。
		MK_LBUTTON   マウスの左ボタンが押されている場合にセットします。
		MK_MBUTTON   マウスの中央ボタンが押されている場合にセットします。
		MK_RBUTTON   マウスの右ボタンが押されている場合にセットします。
		MK_SHIFT     Shift キーが押されている場合にセットします。
		*/
		i_.mouse().setPoint(pt.x, pt.y);
	}

	void onTimer(UINT id, TIMERPROC /*proc*/)
	{
		HRslt hr = i_.update();
		std::strstream str;
		str << "input status " << input().mouse().x << " "<< input().mouse().y << endl;
		str << input().mouse().dx << " "<< input().mouse().dy << " " << input().mouse().dz << endl;
		str << " " << input().mouse().press[0] << " "<< input().mouse().press[1] << " " << input().mouse().press[2] << endl;
		str << " A=" << input().kbd().press(DIK_A) << " S=" << input().kbd().press(DIK_S) << endl;
		str << hr << ends;
		text_.SetWindowText(str.str());
		text_.UpdateWindow();
	}

	void onDestroy()
	{
		i_.tearDown();
		KillTimer(timer_);
	}
};

extern "C" int APIENTRY WinMain(HINSTANCE self, HINSTANCE /*prev*/, LPSTR cmdline, int /*cmdshow*/)
{
	int ret = 1;
	_Module.Init(NULL, self);
	// メッセージループの設定
	CMessageLoop loop;
	_Module.AddMessageLoop(&loop);
	Input::initialize(self);
	PRNN("列挙されたデバイス");
	for(Input::DeviceList::const_iterator i = Input::devicies().begin(); i != Input::devicies().end(); i++) {
		PRNN(*i);
	}
	// メインウィンドウの作成と表示
	Dialog dlg;
	ret = dlg.DoModal(NULL);
	// 終了処理
	_Module.RemoveMessageLoop();
	_Module.Term();
	return ret;
}

CAppModule _Module;
#endif

#pragma comment(lib, "DxErr9.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput.lib")
#pragma comment(lib, "dinput8.lib")
