/**@file
 * グラフィック機能のテスト
 * 
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 16:56:21
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "stdafx.h"

#if 1
#include <gctp/dbgout.hpp>
#include <gctp/graphic.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/inputbuffer.hpp>
#include <gctp/graphic/spritebuffer.hpp>
#include <gctp/graphic/texture.hpp>
#include <gctp/font.hpp>
#include <gctp/graphic/text.hpp>
#include <gctp/graphic/fonttexture.hpp>
#include <gctp/inputmethod.hpp>
#include "SmartWin.h"

using namespace gctp;
using namespace gctp::graphic;
using namespace std;
using namespace SmartWin;

class TestWindow : public WidgetFactory<WidgetWindow, TestWindow>
{
	TYPEDEF_DXCOMPTR(ID3DXMesh);
	typedef TestWindow Self;

public:
	TestWindow() {}

	void setUp(bool is_fs = false)
	{
		HRslt hr;
		if(is_fs) {
			size_t mode_no = graphic::dx::adapters()[0].modes.size()-1;
			createWindow();
			ime_.setUp(handle());
			for(size_t i = 0; i < graphic::dx::adapters()[0].modes.size(); i++) {
				const D3DDISPLAYMODE &mode = graphic::dx::adapters()[0].modes[i];
				if(mode.Width >= 640 && mode.Width >= 480 && mode.Format < D3DFMT_A16B16G16R16) {
					mode_no = i;
					break;
				}
			}
			const D3DDISPLAYMODE &mode = graphic::dx::adapters()[0].modes[mode_no];
			setBounds(0, 0, mode.Width, mode.Height);
			setText( _T("Hello SmartWin") );    // Title
			if(getParent()) hr = g_.open(handle(), getParent()->handle(), 0, (uint)mode_no);
			else hr = g_.open(handle(), 0, (uint)mode_no);
			::SetWindowLong(handle(), GWL_STYLE, 0);
			::SetWindowLong(handle(), GWL_EXSTYLE, 0);
			maximize();
		}
		else {
			createWindow();
			ime_.setUp(handle());
			setText( _T("Hello SmartWin") );    // Title
			if(getParent()) hr = g_.open(handle(), getParent()->handle());
			else hr = g_.open(handle());
		}
		if(!hr) {
			GCTP_TRACE(hr);
			close();
			return;
		}
		g_.setCurrent();

		tex_ = graphic::createOnDB<Texture>("../media/gctp.jpg");
		spr_ = graphic::createOnDB<SpriteBuffer>("spritebuffer");
		font_ = graphic::createOnDB<FontTexture>("fonttexture");
		theta_ = 0; scale_ = 0; count_ = 0;

		updateDisplay();

		// handlers
		onSized( &Self::doOnResized );
		onKeyPressed( &Self::doOnKeyPressed );
		onMouseMove( &Self::doOnMouseMove );
		onRaw( &Self::doOnChar, Message(WM_CHAR) );
		onRaw( &Self::doOnIMEChar, Message(WM_IME_CHAR) );
		onRaw( &Self::doOnIMENotify, Message(WM_IME_NOTIFY) );

		Command com( _T("Testing") );
		createTimer(&Self::onTimer, 16, com);
	}

	void doOnResized(const WidgetSizedEventResult & sz)
	{
		GCTP_TRACE("onResized "<<sz.newSize.x<<","<<sz.newSize.y);
	}

	void onTimer(const CommandPtr &p)
	{
		updateDisplay();
		Command com( _T("Testing") );
		createTimer(&Self::onTimer, 16, com);
	}

	sw::Point mouse_pos_;
	void doOnMouseMove(const MouseEventResult &mouse)
	{
		mouse_pos_ = mouse.pos;
	}

	bool doOnKeyPressed(int key)
	{
		return ibuf_.inputKey(key);
	}

	HRESULT doOnChar(LPARAM lParam, WPARAM wParam)
	{
		ibuf_.input(wParam);
		return 0;
	}

	HRESULT doOnIMEChar(LPARAM lParam, WPARAM wParam)
	{
		ibuf_.input(wParam);
		return 0;
	}

	HRESULT doOnIMENotify(LPARAM lParam, WPARAM wParam)
	{
		ime_.onNotify(wParam);
		return 0;
	}
	
	void updateDisplay()
	{
		if(g_.isOpen()) {
			g_.setCurrent();

			Matrix mat_world;
			mat_world.rotY(count_/360.0f);
			setWorld(mat_world);
			count_++;

			clear();
			begin();

			SpriteDesc desc;
			desc.setUp(*tex_);
			desc.addOffset(Point2fC(0.5f, 0.5f));
			spr_->begin(getViewPort().size(), *tex_).draw(desc).end();

			text_.out() << desc.pos[0] << "," << desc.pos[1] << "," << desc.pos[2] << "," << desc.pos[3] << endl;
			ID3DXMeshPtr mesh;
			HRslt hr = D3DXCreateTeapot(device().impl().ptr(), &mesh, NULL);
			if(!hr) PRNN(hr);
			setView(Matrix().setLookAt(VectorC(0,3,5), VectorC(0,0,0), VectorC(0,1,0)));
			setProjection(Matrix().setFOV(g_pi/4, getViewPort().aspectRatio(), 1.0f, 100.0f));

			if(mesh) mesh->DrawSubset(0);
			else PRNN("ティーポッドが無い。。。");

			spr_->begin(*font_).draw(SpriteDesc().setUp(*font_).addOffset(Point2fC(256, 8)).setColor(Color32(255,0,0))).end();
			
			spr_->begin(*tex_);
			spr_->draw(SpriteDesc().setUp(*tex_).mult(Point2fC(tex_->originalSize())/2, Matrix2C(true).scale(scale_, scale_)*Matrix2C(true).rot(theta_)));
			spr_->draw(SpriteDesc().setUp(*tex_).addOffset(Point2fC(scale_*100.0f, scale_*100.0f)));
			spr_->end();
			theta_ += 1/180.0f*g_pi;
			scale_ = fabsf(sinf(theta_))+0.5f;

			text_.setClumpPos(10, -20).out() << "Alt + F4 to exit";
			text_.setPos(10, 20).setColor(Color32(255, 0, 127)).out() << "現在の角度:";
			text_.setPos(90, 20).out() << theta_;
			text_.setPos(160, 20).out() << scale_;
			text_.setPos(10, 40).out() << "mouse (" << mouse_pos_.x << ", " << mouse_pos_.y << ")";
			text_.setPos(10, 60).out() << ibuf_.get();
			Point2f cursor_pos = text_.getPos(*font_, (int)ibuf_.cursor()-(int)ibuf_.get().size());
			if(ime_.isOpen()) {
				ime_.setPos(Point2C(cursor_pos));
				Handle<gctp::Font> default_font = gctp::db()["defaultfont"];
				if(default_font) {
					LOGFONT logfont;
					default_font->getLogFont(logfont);
					ime_.setFont(logfont);
				}
			}
#if 0
			if(ime_.isOpen() && !ime_.str().empty()) {
				ime_.setPos(Point2C(cursor_pos));
				uint i;
				for(i = 0; i < ime_.attrs().size(); i++) {
					if(i == 0) text_.setBackColor(Color32(64, 64, 127));
					if(ime_.attrs()[i] == ATTR_TARGET_CONVERTED || ime_.attrs()[i] == ATTR_TARGET_NOTCONVERTED) {
						text_.setColor(Color32(255, 255, 127), i);
					}
					else text_.setColor(Color32(255, 127, 255), i);
				}
				text_.setBackColor(Color32(0, 0, 0, 0), i);
				text_.setPos(cursor_pos.x, cursor_pos.y).out() << ime_.str();
				text_.setColor(Color32(255, 127, 255));
				cursor_pos = text_.getPos(*font_, (int)ime_.cursor()-(int)ime_.str().size());
				text_.setPos(cursor_pos.x, cursor_pos.y+2).out() << (isleadbyte(ime_.str().c_str()[ime_.cursor()]) ? "＿" : "_");
			}
			else text_.setPos(cursor_pos.x, cursor_pos.y+2).out() << (isleadbyte(ibuf_.cursorChar()) ? "＿" : "_");
#else
			text_.setPos(cursor_pos.x, cursor_pos.y+2).out() << (isleadbyte(ibuf_.cursorChar()) ? "＿" : "_");
#endif
			if(ime_.isOpen()) text_.setPos(10, 6).setColor(Color32(255, 127, 255)).out() << "変換";
			text_.draw(*spr_, *font_);

			end();
			present();
		}
	}

private:
	Device g_;

	Pointer<SpriteBuffer> spr_;
	Pointer<Texture> tex_;
	Pointer<FontTexture> font_;

	float theta_;
	float scale_;
	int count_;

	Text text_;
	InputBuffer ibuf_;
	InputMethod ime_;
};

int SmartWinMain( Application & app )
{
	locale::global(locale("japanese"));
	initialize();

	WidgetMessageBoxFree msgbox;
	WidgetMessageBoxFree::RetVal retval = msgbox.show(
		_T("フルスクリーンで起動しますか？"),
		_T("モード選択"),
		WidgetMessageBoxFree::BOX_YESNO, WidgetMessageBoxFree::BOX_ICONINFORMATION);

	TestWindow *test_window = new TestWindow;
	test_window->setUp(retval == WidgetMessageBoxFree::RETBOX_YES);
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

#include <gctp/dbgout.hpp>
#include <time.h>
#include <gctp/graphic.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <sstream>
#include <string>
#include <gctp/inputbuffer.hpp>
#include <gctp/graphic/spritebuffer.hpp>
#include <gctp/graphic/texture.hpp>
#include <boost/lexical_cast.hpp>
#include <gctp/font.hpp>
#include <gctp/graphic/text.hpp>
#include <gctp/graphic/fonttexture.hpp>
#include <locale.h>
#include <gctp/inputmethod.hpp>

using namespace gctp;
using namespace gctp::graphic;
using namespace std;

class Window : public CWindowImpl<Window> {
	TYPEDEF_DXCOMPTR(ID3DXMesh);
	static int wndcnt;
	Device g_;
	UINT timer_;
	int count_;
	InputBuffer ibuf_;
	Pointer<SpriteBuffer> spr_;
	Pointer<Texture> tex_;
	Pointer<FontTexture> font_;
	//Pointer<Font> lucida_;
	//Pointer<Font> msgothic_;
	float theta_;
	float scale_;
	Text text_;
	InputMethod ime_;

public:
	bool create()
	{
		RECT rc = {0, 0, 480, 320};
		AdjustWindowRectEx(&rc, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_OVERLAPPEDWINDOW);
		if(Create(NULL, rc, "graphictest", WS_OVERLAPPEDWINDOW)) {
			timer_ = SetTimer(1, 33);
			CenterWindow();
			ShowWindow(SW_SHOW);
			UpdateWindow();

			ime_.setUp(m_hWnd);

			HRslt hr = g_.open(*this, GetParent());
			if(!hr) {
				GCTP_TRACE(hr);
				return hr;
			}
			g_.setCurrent();

			//InputMethod::setAlwaysOff(*this);
			is_handling_ = true;
			tex_ = graphic::createOnDB<Texture>("../media/gctp.jpg");
			spr_ = graphic::createOnDB<SpriteBuffer>("spritebuffer");
			font_ = graphic::createOnDB<FontTexture>("fonttexture");
			//lucida_ = gctp::createOnDB<gctp::Font>("Lucida Console,10,BOLD");
			//msgothic_ = gctp::createOnDB<gctp::Font>("MS ゴシック,11,NORMAL");
			//PRNN("font : "<<msgothic_->cellsize());
			theta_ = 0; scale_ = 0;

			count_ = 0;
			wndcnt++;
			return true;
		}
		return false;
	}

private:
	BEGIN_MSG_MAP_EX(Window)
		MSG_WM_TIMER(onTimer)
		MSG_WM_CHAR(onChar)
		MSG_WM_KEYDOWN(onKeyDown)
		MSG_WM_DESTROY(onDestroy)
		MESSAGE_HANDLER_EX(WM_IME_CHAR, onIMEChar)
		MESSAGE_HANDLER_EX(WM_IME_COMPOSITION, onIMEComposition)
		MESSAGE_HANDLER_EX(WM_IME_NOTIFY, onIMENotify)
	END_MSG_MAP()

	void onTimer(UINT id, TIMERPROC /*proc*/)
	{
		if(g_.isOpen()) {
			g_.setCurrent();

			Matrix mat_world;
			mat_world.rotY(count_/360.0f);
			setWorld(mat_world);
			count_++;

			clear();
			begin();

			SpriteDesc desc;
			desc.setUp(*tex_);
			desc.addOffset(Point2f(0.5f, 0.5f));
			spr_->begin(getViewPort().size(), *tex_).draw(desc).end();

			text_.out() << desc.pos[0] << "," << desc.pos[1] << "," << desc.pos[2] << "," << desc.pos[3] << endl;
			ID3DXMeshPtr mesh;
			HRslt hr = D3DXCreateTeapot(device().impl().ptr(), &mesh, NULL);
			if(!hr) PRNN(hr);
			setView(Matrix().setLookAt(Vector(0,3,5), Vector(0,0,0), Vector(0,1,0)));
			setProjection(Matrix().setFOV(g_pi/4, getViewPort().aspectRatio(), 1.0f, 100.0f));
			//device().impl()->SetRenderState( D3DRS_CLIPPING, FALSE );
			//device().impl()->SetRenderState( D3DRS_LIGHTING, FALSE );
			//device().impl()->SetRenderState( D3DRS_AMBIENT, 0xFFFFFFFF );

			if(mesh) mesh->DrawSubset(0);
			else PRNN("ティーポッドが無い。。。");

			spr_->begin(*font_).draw(SpriteDesc().setUp(*font_).addOffset(Point2f(200, 8)).setColor(Color32(255,0,0))).end();
			
			spr_->begin(*tex_);
			spr_->draw(SpriteDesc().setUp(*tex_).mult(tex_->originalSize()/2, Matrix2(true).scale(scale_, scale_)*Matrix2(true).rot(theta_)));
			spr_->draw(SpriteDesc().setUp(*tex_).addOffset(Point2f(scale_*100.0f, scale_*100.0f)));
			spr_->end();
			theta_ += 1/180.0f*g_pi;
			scale_ = fabsf(sinf(theta_))+0.5f;

			//text_.setFont(msgothic_);
			text_.setPos(10, 20).setColor(Color32(255, 0, 127)).out() << "現在の角度:" << theta_ << " & " << scale_;
			text_.setPos(10, 40).out() << ibuf_.get();
			Point2f cursor_pos = text_.getPos(*font_, (int)ibuf_.cursor()-(int)ibuf_.get().size());
			if(ime_.isOpen()) ime_.setPos(Point2(cursor_pos));
			if(ime_.isOpen() && !ime_.str().empty()) {
				ime_.setPos(Point2(cursor_pos));
				uint i;
				for(i = 0; i < ime_.attrs().size(); i++) {
					if(i == 0) text_.setBackColor(Color32(64, 64, 127));
					if(ime_.attrs()[i] == ATTR_TARGET_CONVERTED || ime_.attrs()[i] == ATTR_TARGET_NOTCONVERTED) {
						text_.setColor(Color32(255, 255, 127), i);
					}
					else text_.setColor(Color32(255, 127, 255), i);
				}
				text_.setBackColor(Color32(0, 0, 0, 0), i);
				text_.setPos(cursor_pos.x, cursor_pos.y).out() << ime_.str();
				text_.setColor(Color32(255, 127, 255));
				cursor_pos = text_.getPos(*font_, (int)ime_.cursor()-(int)ime_.str().size());
				text_.setPos(cursor_pos.x, cursor_pos.y+2).out() << (isleadbyte(ime_.str().c_str()[ime_.cursor()]) ? "＿" : "_");
			}
			else text_.setPos(cursor_pos.x, cursor_pos.y+2).out() << (isleadbyte(ibuf_.cursorChar()) ? "＿" : "_");
			if(ime_.isOpen()) text_.setPos(10, 6).setColor(Color32(255, 127, 255)).out() << "変換";
			text_.draw(*spr_, *font_);

			end();
			present();
		}
	}

	void onChar(TCHAR nChar, UINT nRepCnt, UINT nFlags)
	{
		ibuf_.input(nChar);
	}

	void onKeyDown(TCHAR nChar, UINT nRepCnt, UINT nFlags)
	{
		ibuf_.inputKey(nChar);
	}

	// WM_DESTROYの処理
	void onDestroy()
	{
		g_.close();
		wndcnt--;
		if(!wndcnt) ::PostQuitMessage(0);
	}

	LRESULT onIMEChar(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		ibuf_.input(wParam);
		return 0;
	}

	bool is_handling_;
	LRESULT onIMEComposition(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if(!is_handling_) SetMsgHandled(FALSE);
		else if(!ime_.onComposition(lParam)) SetMsgHandled(FALSE);
		return 0;
	}

	LRESULT onIMENotify(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if(IMN_SETCANDIDATEPOS==wParam) SetMsgHandled(FALSE);
		else if(IMN_SETCOMPOSITIONWINDOW==wParam) SetMsgHandled(FALSE);
		else if(!ime_.onNotify(wParam)) SetMsgHandled(FALSE);
		return 0;
	}
};

int Window::wndcnt = 0;

extern "C" int APIENTRY WinMain(HINSTANCE self, HINSTANCE /*prev*/, LPSTR cmdline, int /*cmdshow*/)
{
	setlocale(LC_ALL, "");
	int ret = 1;
	_Module.Init(NULL, self);
	// メッセージループの設定
	CMessageLoop loop;
	_Module.AddMessageLoop(&loop);
	initialize();
	// メインウィンドウの作成と表示
	Window wnd, altwnd;
	if(wnd.create() /*&& altwnd.create()*/) {
	//Window wnd;
	//if(wnd.create()) {
		// メッセージループ
		ret = loop.Run();
	}
	else AtlMessageBox(NULL, "DirectX製作失敗");
	// 終了処理
	_Module.RemoveMessageLoop();
	_Module.Term();
	return ret;
}

CAppModule _Module;

#endif

#pragma comment(lib, "DxErr9.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dxof.lib")
#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dxguid.lib")
#ifdef _DEBUG
# pragma comment(lib, "d3dx9d.lib")
#else
# pragma comment(lib, "d3dx9.lib")
#endif
