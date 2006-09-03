/**@file
 * �I�[�f�B�I�@�\�̃e�X�g
 * @author osamu takasugi
 * @date 2003/07/24 14:00:31
 * $Id: main.cpp,v 1.1 2003/08/04 20:10:12 SAM Exp $
 */
#include "stdafx.h"
#include <gctp/dbgout.hpp>
#include <time.h>
#include <gctp/audio.hpp>
#include <gctp/audio/vorbis.hpp>

using namespace gctp;
using namespace gctp::audio;
using namespace std;

#if 1
using namespace SmartWin;

class TestWindow : public WidgetFactory<WidgetWindow, TestWindow>
{
	typedef TestWindow Self;
public:

	void setUp()
	{
		createWindow();
		setText( _T("Audio Test") );
		onClosing( &Self::doOnClosing );

		HRslt hr = a_.open(handle());
		if(!hr) GCTP_TRACE(hr);
		a_.setCurrent();
		p1_ = a_.ready("../media/pang.wav");
		p2_ = a_.ready("../media/hugeraw.wav");
		p2_.play(true);
//		ogg_.UseDirectSound(a_);
//		ogg_.OpenOgg("media/ogg.ogg");
//		ogg_.Play(0, true);

		// handlers
		onLeftMouseUp( &Self::doOnLButtonUp );
		onRightMouseUp( &Self::doOnRButtonUp );
	}

private:
	void doOnLButtonUp(const MouseEventResult &mouse)
	{
		GCTP_TRACE("onLButtonUp");
		p1_.play(); // �S�N���b�N����ƁABGM���x��܂��c�ǂ��ɂ����Ȃ���
	}

	void doOnRButtonUp(const MouseEventResult &mouse)
	{
		GCTP_TRACE("onRButtonUp");
		if(p2_.isPlaying()) p2_.stop();
		else p2_.play(true);
	}

	bool doOnClosing()
	{
		a_.close();
		return true;
	}

	Device a_;

	Player p1_;
	Player p2_;
//	OggPlayer ogg_;
};

int SmartWinMain( Application & app )
{
	locale::global(locale("japanese"));
	// OggVorbis�ݒ�
	VorbisAcmDriver vorbis;
	vorbis.initialize();
	TestWindow *test_window = new TestWindow;
	test_window->setUp();
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
class Window : public CWindowImpl<Window> {
	Device a_;
	Player p1_;
	Player p2_;
//	OggPlayer ogg_;
public:
	bool create()
	{
		if(Create(NULL, CWindow::rcDefault, "audiotest", WS_OVERLAPPEDWINDOW)) {
			CenterWindow();
			ShowWindow(SW_SHOW);
			UpdateWindow();
			HRslt hr = a_.open(m_hWnd);
			if(!hr) GCTP_TRACE(hr);
			a_.setCurrent();
			p1_ = a_.ready("../media/pang.wav");
			p2_ = a_.ready("../media/hugeraw.wav");
			p2_.play(true);
//			ogg_.UseDirectSound(a_);
//			ogg_.OpenOgg("media/ogg.ogg");
//			ogg_.Play(0, true);
			return true;
		}
		return false;
	}
private:
	BEGIN_MSG_MAP_EX(Window)
		MSG_WM_LBUTTONUP(onLButtonUp)
		MSG_WM_RBUTTONUP(onRButtonUp)
		MSG_WM_DESTROY(onDestroy)
	END_MSG_MAP()

	void onLButtonUp(UINT keys, CPoint pt)
	{
		GCTP_TRACE("onLButtonUp");
		p1_.play(); // �S�N���b�N����ƁABGM���x��܂��c�ǂ��ɂ����Ȃ���
		if(!p2_.isPlaying()) {
			p2_.play(true);
		}
	}

	void onRButtonUp(UINT keys, CPoint pt)
	{
		GCTP_TRACE("onRButtonUp");
		p2_.stop();
	}

	void onDestroy()
	{
		a_.close();
		::PostQuitMessage(0);
	}
};

CAppModule _Module;

extern "C" int APIENTRY WinMain(HINSTANCE self, HINSTANCE /*prev*/, LPSTR cmdline, int /*cmdshow*/)
{
	int ret = 1;
	_Module.Init(NULL, self);

	// ���b�Z�[�W���[�v�̐ݒ�
	CMessageLoop loop;
	_Module.AddMessageLoop(&loop);
	// �X�v���b�V���E�B���h�E��\�����Ă���ԂɁA���낢��񋓁B
	//initialize();
    CoInitialize(NULL);
	// OggVorbis�ݒ�
	VorbisAcmDriver vorbis;
	vorbis.initialize();
	// �N���I�v�V���������[�U�[�ɖ₢���킹
	//bool is_fs; UINT mode; Graphic::RefreshRateType rrtype;
	//if(configdialog(is_fs, mode, rrtype)) {
	{
		// ���C���E�B���h�E�̍쐬�ƕ\��
		Window wnd;
		if(wnd.create()) {
			// ���b�Z�[�W���[�v
			ret = loop.Run();
		}
		else AtlMessageBox(NULL, "DirectX���쎸�s");
	}
	// �I������
    CoUninitialize();

	_Module.RemoveMessageLoop();
	_Module.Term();
	return ret;
}
#endif

#pragma comment(lib, "DxErr9.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dxof.lib")
#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dinput.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
