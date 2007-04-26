/** @file
 * GameCatapult DirectX Audioクラス
 *
 * Pointer/Handleでなくshared_ptr/weak_ptrなのはスレッドで触りまくるから、なんだけど、
 * パフォーマンス的に問題あるかも。
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/audio/dx/device.hpp>
#include <gctp/audio/dx/buffer.hpp>
#include <gctp/audio/dx/wavfile.hpp>
#ifdef GCTP_AUDIO_USE_TIMER
#include <gctp/timer.hpp>
#endif
#include <boost/weak_ptr.hpp>
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

	ulong Device::allow_streaming_threshold_ = 100*1024; // 1*1024*1024*1024;

	HRslt Device::open(HWND hwnd, DWORD coop_mode, bool global_focus) {
		global_focus_ = global_focus;
		HRslt hr = DirectSoundCreate8(NULL, &ptr_, NULL);
		if(hr && ptr_) {
			hr = ptr_->SetCooperativeLevel(hwnd, coop_mode);
			if(hr) {
				notify_thread_ = CreateThread( NULL, 0, handleEvents, this, 0, &notify_thread_id_ );
				PRNN(_T("オーディオスレッド製作 : ") << notify_thread_id_);
			}
			else ptr_ = 0;
		}
		return hr;
	}

	void Device::close()
	{
		if(notify_thread_) {
			PostThreadMessage( notify_thread_id_, WM_QUIT, 0, 0 );
			WaitForSingleObject( notify_thread_, INFINITE );
			CloseHandle( notify_thread_ );
			notify_thread_ = NULL;
		}
		cleanUp();
		buffers_.resize(0); // これだけでも良かったっけかな…
		ptr_ = 0;
	}

	void Device::setVolume(float volume) {
		Lock al(monitor_);
		volume_ = volume;
	}
	
	float Device::volume() {
		Lock al(monitor_);
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

	/** 再生するサウンドを用意する
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:44:56
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	BufferPtr Device::ready(const _TCHAR *fname) {
		BufferPtr ret;
		dx::WavFile wav;
		if(wav.open(fname)) {
			if(wav.size() < allow_streaming_threshold_) {
				ret = newStaticBuffer(ptr_, wav, global_focus_);
				add(BufferHandle(ret));
			}
			else {
				ret = newStreamingBuffer(ptr_, fname, global_focus_);
				add(BufferHandle(ret));
			}
		}
		else {
			GCTP_TRACE(_T("指定のファイルを読み込めませんでした :")<<fname);
		}
		return ret;
	}

	void Device::add(BufferHandle addee) {
		Lock al(monitor_);
		BufferPtr _addee = addee.lock();
		for(BufferList::iterator i = buffers_.begin(); i != buffers_.end();) {
			BufferPtr _i = i->lock();
			if(!_i) i = buffers_.erase(i);
			else {
				if(_i.get() == _addee.get()) return;
				++i;
			}
		}
		buffers_.push_back(addee);
	}

	/// デバイス消失時のリソース解放
	void Device::cleanUp() {
		Lock al(monitor_);
		for(BufferList::iterator i = buffers_.begin(); i != buffers_.end();) {
			BufferPtr _i = i->lock();
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
						Lock al(device->monitor_);
						for(BufferList::iterator i = device->buffers_.begin(); i != device->buffers_.end();) {
							BufferPtr ptr = i->lock();
							if(ptr) {
								if(ptr->isPlaying()) {
									if( !( hr = ptr->onNotified() ) ) {
										DXTRACE_ERR( TEXT("audio::dx::Device::handleEvents"), hr.i );
										done = true;
										break;
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
		vector<BufferPtr> bufs;
		DWORD result;
		
		bool done = false;
		while( !done ) {
			{
				Lock al(device->monitor_);
				for(BufferList::iterator i = device->buffers_.begin(); i != device->buffers_.end();) {
					BufferPtr ptr = i->lock();
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
