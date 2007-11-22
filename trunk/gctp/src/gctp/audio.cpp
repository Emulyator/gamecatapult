/** @file
 * GameCatapult DirectX Audio�N���X
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/audio.hpp>
#include <gctp/audio/dx/device.hpp>
#include <gctp/extension.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace audio {

	Device::Device() : impl_(new dx::Device) {}

	/** audio::Device�̏�����
	 *
	 * ������Ăяo���O�ɁACoInitializeEx(NULL, 0) ������Ă������ƁB
	 * COM�֌W�̏������A�������ɂ́Aaudio::Device�N���X�͊ւ��Ȃ��B
	 */
	HRslt Device::open(
		HWND hwnd,				///< �t�H�[�J�X�E�B���h�E
		bool play_focus_lost,	///< �t�H�[�J�X�������Ă��Đ����Â��邩�H
		bool priority			///< �����D�挠���l�����邩�H
	) {
		return impl_->open(hwnd, priority ? DSSCL_PRIORITY : DSSCL_NORMAL, play_focus_lost);
	}

	void Device::close()
	{
		impl_->close();
	}

	bool Device::isOpen() const
	{
		return impl_->ptr() ? true : false;
	}

	Device::~Device()
	{
		if(current_ == this) current_ = NULL;
	}
	
	void Device::setVolume(float volume)
	{
		if(impl_) impl_->setVolume(volume);
	}

	float Device::volume()
	{
		return impl_->volume();
	}

	HRslt Device::setFormat(int channel_num, int freq, int bitrate)
	{
		if(impl_) return impl_->setFormat(channel_num, freq, bitrate);
		return CO_E_NOTINITIALIZED;
	}
	
	HRslt Device::getFormat(int &channel_num, int &freq, int &bitrate)
	{
		if(impl_) return impl_->getFormat(channel_num, freq, bitrate);
		return CO_E_NOTINITIALIZED;
	}

	Device* Device::current_;

	Player Device::ready(const _TCHAR *fname, bool streaming)
	{
		if(impl_) {
			Pointer<Clip> clip = new Clip;
			if(clip && clip->open(fname)) {
				if(streaming) return Player(impl_->newStream(clip), clip);
				else return Player(impl_->newBuffer(clip));
			}
			else {
				GCTP_TRACE(_T("�w��̃t�@�C����ǂݍ��߂܂���ł��� :")<<fname);
			}
		}
		return Player();
	}

	Player Device::ready(Clip &clip, bool streaming)
	{
		if(impl_) {
			if(streaming) return Player(impl_->newStream(&clip), &clip);
			else return Player(impl_->newBuffer(&clip));
		}
		return Player();
	}

}} // namespace gctp
