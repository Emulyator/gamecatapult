/**@file
 * グラフィック機能のテスト
 * 
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 16:56:21
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "stdafx.h"
#include <gctp/dbgout.hpp>
#include <time.h>
#include <gctp/graphic.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <sstream>
#include <gctp/graphic/asciifont.hpp>
#include <gctp/camera.hpp>
#include <gctp/context.hpp>
#include <gctp/model.hpp>
#include <gctp/motion.hpp>
#include <gctp/db.hpp>
#include <gctp/fpsinfo.hpp>
#include <gctp/xfile.hpp>

using namespace gctp;
using namespace std;

class Window : public CWindowImpl<Window> {
	TYPEDEF_DXCOMPTR(ID3DXMesh);
	static int wndcnt;
	graphic::Device g_;
	Pointer<graphic::AsciiText> text_;
	Camera cam_;
	Context context_;
	Pointer<graphic::Model> model;
	Pointer<MotionSet> motion;
	Pointer<Skeleton> skeleton;
	Pointer<Skeleton> work_skeleton;
	UINT timer_;
	int count_;
	float key_;
	float speed_;
	FPSInfo fps_;

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

			HRslt hr = g_.open(m_hWnd, GetParent());
			if(!hr) {
				GTRACE(hr);
				return hr;
			}
			g_.setCurrent();

			count_ = 0;
			text_ = new graphic::AsciiText;
			text_->setUp("Lucida Console", 10, graphic::AsciiFont::BOLD);
			wndcnt++;
			
			cam_.pos_ = Vector(0.0f, 0.5f, -2.0f);
			cam_.at_ = Vector(0.0f, 0.0f, 1.0f);
			cam_.up_ = Vector(0.0f, 1.0f, 0.0f);

			std::string fname("gradriel_stand_t.x");
			context_.load(fname.c_str());

			Handle<XFile> xfile = db()[fname];
			if(xfile) {
				for(XFile::iterator i = xfile->begin(); i != xfile->end(); ++i) {
					if(typeid(graphic::Model) == typeid(**i)) {
						if(!model) model = *i;
					}
					else if(typeid(MotionSet) == typeid(**i)) {
						if(!motion) motion = *i;
					}
					else if(typeid(Skeleton) == typeid(**i)) {
						if(!skeleton) skeleton = *i;
					}
				}
				if(model) {
					hr = model->useVS();
					if(!hr) GTRACE(hr);
				}
			}

			if(skeleton) work_skeleton = skeleton->dup();
			if(work_skeleton) {
				work_skeleton->setPosType(Motion::SPLINE);
				work_skeleton->setIsOpen(Motion::CLOSE);
			}

			fps_.reset();
			key_ = 0.0f;
			speed_ = 15.0f;
			return true;
		}
		return false;
	}

private:
	BEGIN_MSG_MAP_EX(Window)
		MSG_WM_TIMER(onTimer)
		MSG_WM_DESTROY(onDestroy)
	END_MSG_MAP()

	void onTimer(UINT id, TIMERPROC /*proc*/)
	{
		key_ += fps_.update()*speed_;
		if(g_) {
			g_.setCurrent();

			graphic::device().impl()->SetRenderState( D3DRS_CLIPPING, TRUE );
			graphic::device().impl()->SetRenderState( D3DRS_LIGHTING, TRUE );
			graphic::device().impl()->SetRenderState( D3DRS_DITHERENABLE, TRUE );
			graphic::device().impl()->SetRenderState( D3DRS_ZENABLE, TRUE );
			graphic::device().impl()->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
			graphic::device().impl()->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
			graphic::device().impl()->SetRenderState( D3DRS_LIGHTING, TRUE );
			graphic::device().impl()->SetRenderState( D3DRS_AMBIENT, 0xFFFFFFFF );
			graphic::device().impl()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR  );
			graphic::device().impl()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR  );
			graphic::device().impl()->SetRenderState( D3DRS_COLORVERTEX, FALSE );

			//work_skeleton = *skeleton;
			work_skeleton->setMotion(*motion, key_);
			
			graphic::clear();
			graphic::begin();

			graphic::setCamera(cam_);

			graphic::device().impl()->SetRenderState( D3DRS_LIGHTING, TRUE );
			graphic::device().impl()->SetRenderState( D3DRS_AMBIENT, 0xFFFFFFFF );
			graphic::device().impl()->SetRenderState( D3DRS_CULLMODE, (Camera::is_LH_)?D3DCULL_CCW:D3DCULL_CW );
			//teapot.draw(Matrix::identity());

			HRslt hr = model->draw(*work_skeleton);
			if(!hr) GTRACE(hr);

			text_->setPos(10, 10).setColor(D3DCOLOR_ARGB(255, 200, 200, 127)).out()
				<< "(" << graphic::getScreenSize().x << "," << graphic::getScreenSize().y << ")" << endl
				<< "fps  :" << fps_.latestave << "  " << key_ << endl << endl
/*				<< "yaw  :" << cam_.yaw_ << endl
				<< "pitch:" << cam_.pitch_ << endl
				<< "speed:" << cam_.speed_ << endl*/
				<< "fov  :" << cam_.fov_ << endl
				<< "pos  :" << cam_.pos_.x << " " << cam_.pos_.y << " " << cam_.pos_.z << endl << endl
/*				<< "mode : " << mesh_mode*/;

//			if(walkthru) text.setPos(10, 480-20).setColor(D3DCOLOR_ARGB(255, 200, 127, 200)).out()
//					<< "WALK THROUGH MODE";

			text_->draw(graphic::AsciiFont::FILTERED);

			graphic::end();
			graphic::present();
		}
	}

	// WM_DESTROYの処理
	void onDestroy()
	{
		g_.close();
		wndcnt--;
		if(!wndcnt) ::PostQuitMessage(0);
	}
};

int Window::wndcnt = 0;

extern "C" int APIENTRY WinMain(HINSTANCE self, HINSTANCE /*prev*/, LPSTR cmdline, int /*cmdshow*/)
{
	Timer::initialize();
	GCTP_ATTACH(XFile);
	GCTP_ATTACH(graphic::Texture);

	int ret = 1;
	_Module.Init(NULL, self);
	// メッセージループの設定
	CMessageLoop loop;
	_Module.AddMessageLoop(&loop);
	graphic::initialize();
	// メインウィンドウの作成と表示
	Window wnd;
	if(wnd.create()) {
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
#  pragma comment(lib, "d3dx9dt.lib")
#  pragma comment(lib, "gctpd.lib")
#  pragma comment(lib, "gctp_graphic_d.lib")
#  ifdef _DLL
#    pragma comment(lib, "boost_threadd.lib")
#  else
#    pragma comment(lib, "libboost_threadd.lib")
#  endif
#else
#  pragma comment(lib, "d3dx9.lib")
#  pragma comment(lib, "gctp.lib")
#  pragma comment(lib, "gctp_graphic.lib")
#  ifdef _DLL
#    pragma comment(lib, "boost_thread.lib")
#  else
#    pragma comment(lib, "libboost_thread.lib")
#  endif
#endif
