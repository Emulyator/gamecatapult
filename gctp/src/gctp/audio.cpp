/** @file
 * GameCatapult DirectX Audioクラス
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

	/** audio::Deviceの初期化
	 *
	 * これを呼び出す前に、CoInitializeEx(NULL, 0) をやっておくこと。
	 * COM関係の初期化、末期化には、Audioクラスは関わらない。
	 */
	HRslt Device::open(
		HWND hwnd,				///< フォーカスウィンドウ
		bool play_focus_lost,	///< フォーカスを失っても再生しつづけるか？
		bool priority			///< 高い優先権を獲得するか？
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
	
	void setStreamingThreshold(ulong threshold) { dx::Device::setStreamingThreshold(threshold); }
	ulong streamingThreshold() { return dx::Device::streamingThreshold(); }

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

	Player Device::ready(const char *fname) {
		if(impl_) return Player(impl_->ready(fname));
		return Player();
	}

}} // namespace gctp
