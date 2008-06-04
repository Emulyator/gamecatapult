/**@file
 * グラフィック機能のテスト
 * 
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 16:56:21
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "stdafx.h"

#include <gctp/dbgout.hpp>
#include <gctp/graphic.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/inputbuffer.hpp>
#include <gctp/graphic/spritebuffer.hpp>
#include <gctp/graphic/texture.hpp>
#include <gctp/font.hpp>
#include <gctp/graphic/text.hpp>
#include <gctp/graphic/fonttexture.hpp>
#include <gctp/movie/player.hpp>
#include <gctp/inputmethod.hpp>
#include "SmartWin.h"

using namespace gctp;
using namespace gctp::graphic;
using namespace std;
using namespace SmartWin;

//#define MOVIETEST
//#define MOVIE_FOR_WINDOW

class TestWindow : public WidgetFactory<WidgetWindow, TestWindow>
{
#ifndef GCTP_LITE
	TYPEDEF_DXCOMPTR(ID3DXMesh);
#endif
	typedef TestWindow Self;

public:
	TestWindow() {}

	void setUp(bool is_fs = false)
	{
		HRslt hr;
		if(is_fs) {
			size_t mode_no = graphic::dx::adapters()[0].modes.size()-1;
			for(size_t i = 0; i < graphic::dx::adapters()[0].modes.size(); i++) {
				const D3DDISPLAYMODE &mode = graphic::dx::adapters()[0].modes[i];
				if(mode.Width >= 800 && mode.Width >= 600 && mode.Format < D3DFMT_A16B16G16R16) {
					mode_no = i;
					break;
				}
			}
			const D3DDISPLAYMODE &mode = graphic::dx::adapters()[0].modes[mode_no];
			Seed seed = getDefaultSeed();
			seed.location.pos.x = 0;
			seed.location.pos.y = 0;
			seed.location.size.x = mode.Width;
			seed.location.size.y = mode.Height;
			seed.style = 0;
			seed.exStyle = 0;
			seed.caption = _T("Hello GameCatapult");    // Title
			createWindow(seed);
			ime_.setUp(handle());
			if(getParent()) hr = g_.open(handle(), getParent()->handle(), 0, (uint)mode_no);
			else hr = g_.open(handle(), 0, (uint)mode_no);
			maximize();
		}
		else {
			Seed seed = getDefaultSeed();
			seed.caption = _T("Hello GameCatapult");    // Title
			createWindow(seed);
			SmartWin::Rectangle location = getBounds();
			location.size.x = 800;
			location.size.y = 600;
			::RECT rc = location;
			::AdjustWindowRectEx(&rc, seed.style, 0, seed.exStyle);
			setBounds(SmartWin::Rectangle::FromRECT(rc));
			ime_.setUp(handle());
			if(getParent()) hr = g_.open(handle(), getParent()->handle());
			else hr = g_.open(handle());
		}
		if(!hr) {
			GCTP_TRACE(hr);
			close();
			return;
		}
		g_.setCurrent();

#ifndef GCTP_LITE
		//hr = D3DXCreateTeapot(device().impl().ptr(), &mesh_, NULL);
		//if(!hr) PRNN(hr);
#endif

#ifdef MOVIETEST
# ifdef MOVIE_FOR_WINDOW
		hr = movie_.open(handle(), _T("movie.avi"));
		if(!hr) {
			hr = movie_.open(handle(), _T("movie.mpg"));
			if(!hr) {
				GCTP_TRACE(hr);
				close();
				return;
			}
		}
# else
		hr = movie_.openForTexture(_T("movie.avi"));
		if(!hr) {
			hr = movie_.openForTexture(_T("movie.mpg"));
			if(!hr) {
				GCTP_TRACE(hr);
				close();
				return;
			}
		}
		tex_ = movie_.getTexture().lock();
# endif
		movie_.play(0);
#else
		tex_ = new Texture;
		tex_->setUp(_T("../../../media/gctp.jpg"));
#endif
		font_ = new gctp::Font;
		font_->setUp(_T(",10,BOLD|FIXEDPITCH"));
		spr_ = new SpriteBuffer;
		spr_->setUp();
		fonttex_ = new FontTexture;
		fonttex_->setUp(512, 512);
		theta_ = 0; scale_ = 0; count_ = 0;

		updateDisplay();

		// handlers
		onSized( &Self::doOnResized );
		onMouseMove( &Self::doOnMouseMove );
		onChar( &Self::doOnChar );
		onKeyPressed( &Self::doOnKeyPressed );
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
#ifdef MOVIETEST
		movie_.checkStatus();
#endif
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

	bool doOnChar(int input_char)
	{
		ibuf_.input(input_char);
		return true;
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
#if !defined(MOVIETEST) || !defined(MOVIE_FOR_WINDOW)
		if(g_.isOpen()) {
			g_.setCurrent();

			Matrix mat_world;
			mat_world.rotY(count_/360.0f);
			setWorld(mat_world);
			count_++;

			clear();
			begin();
			text_.reset();
			text_.setFont(font_);

			SpriteDesc desc;
#ifdef MOVIETEST
			desc = movie_.getDesc();
#else
			if(tex_) desc.setUp(*tex_);
#endif
			if(tex_) spr_->begin(getViewPort().size(), *tex_).draw(desc).end();

			text_.out() << desc.pos[0] << "," << desc.pos[1] << "," << desc.pos[2] << "," << desc.pos[3] << endl;
#ifndef GCTP_LITE
			if(mesh_) {
				setView(Matrix().setLookAt(VectorC(0,3,5), VectorC(0,0,0), VectorC(0,1,0)));
				setProjection(Matrix().setFOV(g_pi/4, getViewPort().aspectRatio(), 1.0f, 100.0f));
				mesh_->DrawSubset(0);
			}
#endif
			spr_->begin(*fonttex_).draw(SpriteDesc().setUp(*fonttex_).addOffset(Point2fC(256, 8)).setColor(Color32(255,0,0))).end();
			
			if(tex_) {
				spr_->begin(*tex_);
				spr_->draw(SpriteDesc(desc).mult(Point2fC(tex_->originalSize())/2, Matrix2C(true).scale(scale_*0.3f, scale_*0.3f)*Matrix2C(true).rot(theta_)));
				spr_->draw(SpriteDesc(desc).addOffset(Point2fC(scale_*100.0f, scale_*100.0f)));
				spr_->end();
			}
			theta_ += 1/180.0f*g_pi;
			scale_ = fabsf(sinf(theta_))+0.5f;

			text_.setPos(10, g_.getScreenSize().y-20).out() << "Alt + F4 to exit";
			text_.setPos(10, 20).setColor(Color32(255, 0, 127)).out() << _T("現在の角度:");
			text_.setPos(90, 20).out() << theta_;
			text_.setPos(160, 20).out() << scale_;
			text_.setPos(10, 40).out() << "mouse (" << mouse_pos_.x << ", " << mouse_pos_.y << ")";
			text_.setPos(10, 60).out() << ibuf_.get();
			Point2f cursor_pos = text_.getPos(*fonttex_, (int)ibuf_.cursor()-(int)ibuf_.get().size());
			if(ime_.isOpen()) {
				ime_.setPos(Point2C(cursor_pos));
				LOGFONT logfont;
				font_->getLogFont(logfont);
				ime_.setFont(logfont);
			}
#if 0
			if(ime_.isOpen() && !ime_.str().empty()) {
				ime_.setPos(Point2C(cursor_pos));
				uint i;
				for(i = 0; i < ime_.attrs().size(); i++) {
					if(ime_.attrs()[i] == ATTR_TARGET_CONVERTED || ime_.attrs()[i] == ATTR_TARGET_NOTCONVERTED) {
						text_.setColor(Color32(255, 255, 127), i);
					}
					else text_.setColor(Color32(255, 127, 255), i);
				}
				text_.setPos(cursor_pos.x, cursor_pos.y).out() << ime_.str();
				text_.setColor(Color32(255, 127, 255));
				cursor_pos = text_.getPos(*font_, (int)ime_.cursor()-(int)ime_.str().size());
				text_.setPos(cursor_pos.x, cursor_pos.y+2).out() << (isleadbyte(ime_.str().c_str()[ime_.cursor()]) ? "＿" : "_");
			}
			else text_.setPos(cursor_pos.x, cursor_pos.y+2).out() << (isleadbyte(ibuf_.cursorChar()) ? "＿" : "_");
#else
# ifdef UNICODE
			text_.setPos(cursor_pos.x, cursor_pos.y+2).out() << "_";
# else
			text_.setPos(cursor_pos.x, cursor_pos.y+2).out() << (isleadbyte(ibuf_.cursorChar()) ? _T("＿") : _T("_"));
# endif
#endif
			if(ime_.isOpen()) text_.setPos(10, 6).setColor(Color32(255, 127, 255)).out() << _T("変換");
			text_.draw(*spr_, *fonttex_);

			HRslt hr = end();
			if(!hr) GCTP_TRACE(hr);
			hr = present();
			if(!hr) GCTP_TRACE(hr);
		}
#endif
	}

private:
	Device g_;

#ifndef GCTP_LITE
	ID3DXMeshPtr mesh_;
#endif
#ifdef MOVIETEST
	movie::Player movie_;
#endif
	Pointer<gctp::Font> font_;
	Pointer<SpriteBuffer> spr_;
	Pointer<Texture> tex_;
	Pointer<FontTexture> fonttex_;

	float theta_;
	float scale_;
	int count_;

	Text text_;
	InputBuffer ibuf_;
	InputMethod ime_;
};

int SmartWinMain( Application & app )
{
	locale::global(locale(locale::classic(), locale(""), LC_CTYPE));
	gctp::logfile.imbue(locale(locale::classic(), locale(""), LC_CTYPE));
	// Initialize COM
    CoInitialize(NULL);

#ifdef MOVIETEST
	allowStrictMultiThreadSafe(true); // ムービーは別スレッドでテクスチャに触るので
	setIntervalTime(1);
	//setSwapMode(true);
	// もうこれデフォルトでよくね？
#endif
	initialize();

	WidgetMessageBoxFree msgbox;
	WidgetMessageBoxFree::RetVal retval = msgbox.show(
		_T("フルスクリーンで起動しますか？"),
		_T("モード選択"),
		WidgetMessageBoxFree::BOX_YESNO, WidgetMessageBoxFree::BOX_ICONINFORMATION);

	TestWindow *test_window = new TestWindow;
	test_window->setUp(retval == WidgetMessageBoxFree::RETBOX_YES);
	int ret = app.run();
    CoUninitialize();
	return ret;
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

#ifdef GCTP_LITE
# pragma comment(lib, "DxErr8.lib")
# pragma comment(lib, "d3d8.lib")
# pragma comment(lib, "ddraw.lib")
# pragma comment(lib, "dxguid.lib")
#else
# pragma comment(lib, "DxErr9.lib")
# pragma comment(lib, "d3d9.lib")
# pragma comment(lib, "d3dxof.lib")
# pragma comment(lib, "ddraw.lib")
# pragma comment(lib, "dxguid.lib")
# pragma comment(lib, "winmm.lib")
# ifdef _DEBUG
#  pragma comment(lib, "d3dx9d.lib")
#  ifdef MOVIETEST
#   pragma comment(lib, "strmbasd.lib")
#   pragma comment(lib, "asynbasd.lib")
#  endif
# else
#  pragma comment(lib, "d3dx9.lib")
#  ifdef MOVIETEST
#   pragma comment(lib, "strmbase.lib")
#   pragma comment(lib, "asynbase.lib")
#  endif
# endif
#endif
