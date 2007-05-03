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
#include <sstream>  
#include "SmartWin.h"

using namespace gctp;
using namespace std;

class TestDialog
	: public SmartWin::WidgetFactory<SmartWin::WidgetDialog, TestDialog, SmartWin::MessageMapPolicyDialogWidget>
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
		std::basic_stringstream<_TCHAR> str;
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
	locale::global(locale(locale::classic(), locale(""), LC_CTYPE));
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

#pragma comment(lib, "DxErr9.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput.lib")
#pragma comment(lib, "dinput8.lib")
