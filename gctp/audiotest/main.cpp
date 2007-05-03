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
		p1_ = a_.ready(_T("../../../media/pang.wav"));
		p2_ = a_.ready(_T("../../../media/hugeraw.wav"));
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
	locale::global(locale(locale::classic(), locale(""), LC_CTYPE));
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

#pragma comment(lib, "DxErr9.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dxof.lib")
#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dinput.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")