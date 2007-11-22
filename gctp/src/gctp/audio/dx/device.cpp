/** @file
 * GameCatapult DirectX Audioクラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/audio/dx/device.hpp>
#include <gctp/audio/dx/buffer.hpp>
#include <gctp/audio/clip.hpp>
#ifdef GCTP_AUDIO_USE_TIMER
#include <gctp/timer.hpp>
#endif
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace audio { namespace dx {

	namespace {
		TYPEDEF_DXCOMPTR(IDirectSoundBuffer);
	}

	////////////////////
	// Device
	//

	Device::Device() : notify_thread_id_(0), notify_thread_(NULL)
	{
	}

	Device::~Device()
	{
		close();
	}

	HRslt Device::open(HWND hwnd, DWORD coop_mode, bool global_focus) {
		global_focus_ = global_focus;
		HRslt hr = ::DirectSoundCreate8(NULL, &ptr_, NULL);
		if(hr && ptr_) {
			hr = ptr_->SetCooperativeLevel(hwnd, coop_mode);
			if(hr) {
				notify_thread_ = ::CreateThread( NULL, 0, handleEvents, this, 0, &notify_thread_id_ );
				PRNN(_T("オーディオスレッド製作 : ") << notify_thread_id_);
			}
			else ptr_ = 0;
		}
		return hr;
	}

	void Device::close()
	{
		if(notify_thread_) {
			::PostThreadMessage( notify_thread_id_, WM_QUIT, 0, 0 );
			::WaitForSingleObject( notify_thread_, INFINITE );
			::CloseHandle( notify_thread_ );
			notify_thread_ = NULL;
		}
		cleanUp();
		buffers_.resize(0); // これだけでも良かったっけかな…
		ptr_ = 0;
	}

	void Device::setVolume(float volume) {
		ScopedLock al(monitor_);
		volume_ = volume;
	}
	
	float Device::volume() {
		ScopedLock al(monitor_);
		return volume_;
	}
	
	HRslt Device::setFormat(int channel_num, int freq, int bitrate)
	{
		if(ptr_) {
			// Get the primary buffer 
			DSBUFFERDESC dsbd;
			ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
			dsbd.dwSize        = sizeof(DSBUFFERDESC);
			dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
			dsbd.dwBufferBytes = 0;
			dsbd.lpwfxFormat   = NULL;
			
			HRslt hr;
			IDirectSoundBufferPtr primary;
			if(hr = ptr_->CreateSoundBuffer(&dsbd, &primary, NULL)) {
				WAVEFORMATEX wfx;
				ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
				wfx.wFormatTag      = (WORD) WAVE_FORMAT_PCM;
				wfx.nChannels       = (WORD) channel_num;
				wfx.nSamplesPerSec  = (DWORD) freq;
				wfx.wBitsPerSample  = (WORD) bitrate;
				wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
				wfx.nAvgBytesPerSec = (DWORD) (wfx.nSamplesPerSec * wfx.nBlockAlign);
				hr = primary->SetFormat(&wfx);
				if(!hr) GCTP_TRACE(hr);
			}
			return hr;
		}
		return CO_E_NOTINITIALIZED;
	}
	
	HRslt Device::getFormat(int &channel_num, int &freq, int &bitrate)
	{
		if(ptr_) {
			// Get the primary buffer
			DSBUFFERDESC dsbd;
			ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
			dsbd.dwSize        = sizeof(DSBUFFERDESC);
			dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
			dsbd.dwBufferBytes = 0;
			dsbd.lpwfxFormat   = NULL;
			
			HRslt hr;
			IDirectSoundBufferPtr primary;
			if(hr = ptr_->CreateSoundBuffer(&dsbd, &primary, NULL)) {
				WAVEFORMATEX wfx;
				ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
				hr = primary->GetFormat(&wfx, sizeof(WAVEFORMATEX), NULL);
				if(hr) {
					channel_num = wfx.nChannels;
					freq = wfx.nSamplesPerSec;
					bitrate = wfx.wBitsPerSample;
				}
				else GCTP_TRACE(hr);
			}
			return hr;
		}
		return CO_E_NOTINITIALIZED;
	}

	/** サウンドバッファを用意する
	 *
	 * ストリーミングサウンドバッファを作ってクリップに関連付けして返す
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:44:56
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Pointer<Buffer> Device::newStream(Handle<Clip> clip) {
		Pointer<Buffer> ret;
		if(clip && clip->isOpen()) {
			ret = newStreamingBuffer(ptr_, clip, global_focus_);
			add(Handle<Buffer>(ret));
		}
		return ret;
	}

	/** ワンショット用サウンドバッファを用意する
	 *
	 * 静的サウンドバッファを作ってクリップに関連付けして返す
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:44:56
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Pointer<Buffer> Device::newBuffer(Handle<Clip> clip) {
		Pointer<Buffer> ret;
		if(clip && clip->isOpen()) {
			ret = newStaticBuffer(ptr_, clip, global_focus_);
			add(Handle<Buffer>(ret));
		}
		return ret;
	}

	void Device::add(Handle<Buffer> addee) {
		ScopedLock al(monitor_);
		Pointer<Buffer> _addee = addee.lock();
		for(BufferList::iterator i = buffers_.begin(); i != buffers_.end();) {
			Pointer<Buffer> _i = i->lock();
			if(!_i) i = buffers_.erase(i);
			else {
				if(_i.get() == _addee.get()) return;
				++i;
			}
		}
		buffers_.push_back(addee);
		if(addee) addee->synchronize(true);
	}

	/// デバイス消失時のリソース解放
	void Device::cleanUp() {
		ScopedLock al(monitor_);
		for(BufferList::iterator i = buffers_.begin(); i != buffers_.end();) {
			Pointer<Buffer> _i = i->lock();
			if(!_i) i = buffers_.erase(i);
			else {
				_i->cleanUp();
				++i;
			}
		}
	}

#ifdef GCTP_AUDIO_USE_TIMER
	DWORD WINAPI Device::handleEvents( void *param ) {
		HRslt hr;
		Device *device = reinterpret_cast<Device *>(param);
		MSG msg;
		ulong start = Timer::time();

		bool done = false;
		while( !done ) {
			DWORD wait = 1000/NOTIFY_PERSEC;
			if(wait > 50) wait -= 50;
			else wait = 1;
			MsgWaitForMultipleObjects(0, NULL, FALSE, wait, QS_ALLEVENTS);
			// Messages are available
			while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
				if( msg.message == WM_QUIT ) done = true;
			}
			if(!done) {
				for(;;) {
					if(start-Timer::time() > 1000/NOTIFY_PERSEC) {
						ScopedLock al(device->monitor_);
						for(BufferList::iterator i = device->buffers_.begin(); i != device->buffers_.end();) {
							Pointer<Buffer> ptr = i->lock();
							if(ptr) {
								if(ptr->isPlaying()) {
									if( !( hr = ptr->onNotified() ) ) {
										GCTP_TRACE(hr);
										//DXTRACE_ERR( TEXT("audio::dx::Device::handleEvents"), hr.i );
										//done = true;
										//break;
									}
								}
								++i;
							}
							else i = device->buffers_.erase(i);
						}
						start = Timer::time();
						break;
					}
					::Sleep(0);
				}
			}
		}

		return 0;
	}
#else
	DWORD WINAPI Device::handleEvents( void *param ) {
		HRslt hr;
		Device *device = reinterpret_cast<Device *>(param);
		MSG msg;
		vector<HANDLE> events;
		vector< Pointer<Buffer> > bufs;
		DWORD result;
		
		bool done = false;
		while( !done ) {
			{
				Lock al(device->monitor_);
				for(BufferList::iterator i = device->buffers_.begin(); i != device->buffers_.end();) {
					Pointer<Buffer> ptr = i->lock();
					if(ptr) {
						if(ptr->event()) {
							bufs.push_back(ptr);
							events.push_back(ptr->event());
						}
						++i;
					}
					else i = device->buffers_.erase(i);
				}
			}
			
			if( events.size() )	result = MsgWaitForMultipleObjects(events.size(), &events[0], FALSE, INFINITE, QS_ALLEVENTS );
			if( events.size() && WAIT_OBJECT_0 <= result && result < WAIT_OBJECT_0+events.size() ) {
				//PRNN(_T("イベントシグナル ") << result << "," << events[result-WAIT_OBJECT_0] << "," << bufs[result-WAIT_OBJECT_0]->event());
				if( !( hr = bufs[result-WAIT_OBJECT_0]->onNotified() ) ) {
					DXTRACE_ERR( TEXT("audio::dx::Device::handleEvents"), hr.i );
					done = true;
				}
				bufs.resize(0);
				events.resize(0);
			}
			else {
				// Messages are available
				while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
					if( msg.message == WM_QUIT ) done = true;
				}
			}
		}

		return 0;
	}
#endif
}}} // namespace gctp
