/** @file
 * GameCatapult DirectSoundBufferクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/25 19:35:25
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/audio/dx/buffer.hpp>
#include <gctp/audio/clip.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace audio { namespace dx {

	namespace {
		TYPEDEF_DXCOMPTR(IDirectSoundBuffer);
		TYPEDEF_DXCOMPTR(IDirectSoundBuffer8);
		TYPEDEF_DXCOMPTR(IDirectSound3DBuffer8);
		TYPEDEF_DXCOMPTR(IDirectSound3DListener8);
		TYPEDEF_DXCOMPTR(IDirectSoundNotify);

		const float min_gain = -5000;
	}

	Buffer::~Buffer()
	{
	}

	namespace {
		class StaticBuffer : public Buffer {
		public:
			HRslt setUp(IDirectSound8Ptr device, bool global_focus)
			{
				if( !device ) return CO_E_NOTINITIALIZED;
				PCMWAVEFORMAT pcmwf;
				DSBUFFERDESC dsbd;
 
				// ウェーブ フォーマット構造体を設定する。
				memset(&pcmwf, 0, sizeof(PCMWAVEFORMAT));
				pcmwf.wf.wFormatTag = WAVE_FORMAT_PCM;
				pcmwf.wf.nChannels = 2;
				pcmwf.wf.nSamplesPerSec = 22050;
				pcmwf.wf.nBlockAlign = 4;
				pcmwf.wf.nAvgBytesPerSec = pcmwf.wf.nSamplesPerSec * pcmwf.wf.nBlockAlign;
				pcmwf.wBitsPerSample = 16;
 
				// DSBUFFERDESC 構造体を設定する。
				memset(&dsbd, 0, sizeof(DSBUFFERDESC));
				dsbd.dwSize = sizeof(DSBUFFERDESC);
				dsbd.dwFlags = DSBCAPS_LOCDEFER | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
				dsbd.dwBufferBytes = 3 * pcmwf.wf.nAvgBytesPerSec; // 約３秒分
				dsbd.lpwfxFormat = (LPWAVEFORMATEX)&pcmwf;
				if(global_focus) dsbd.dwFlags |= DSBCAPS_GLOBALFOCUS;

				// バッファを作成する。
				IDirectSoundBufferPtr ptr;
				HRslt hr = device->CreateSoundBuffer(&dsbd, &ptr, NULL);
				if(!hr) GCTP_TRACE(hr);
				else ptr_ = ptr;
				buffersize_ = 3 * pcmwf.wf.nAvgBytesPerSec;

				return hr;
			}

			HRslt setUp(IDirectSound8Ptr device, Clip &clip, bool global_focus)
			{
				if( !device ) return CO_E_NOTINITIALIZED;
				DSBUFFERDESC dsbd;
 
				// DSBUFFERDESC 構造体を設定する。
				memset(&dsbd, 0, sizeof(DSBUFFERDESC));
				dsbd.dwSize = sizeof(DSBUFFERDESC);
				dsbd.dwFlags = DSBCAPS_LOCDEFER | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
				buffersize_ = dsbd.dwBufferBytes = (DWORD)clip.size();
				dsbd.lpwfxFormat = const_cast<WAVEFORMATEX *>(clip.format());
				if(global_focus) dsbd.dwFlags |= DSBCAPS_GLOBALFOCUS;
 
				// バッファを作成する。
				IDirectSoundBufferPtr ptr;
				HRslt hr = device->CreateSoundBuffer(&dsbd, &ptr, NULL);
				if(!hr) return hr;
				else ptr_ = ptr;
				return load(clip);
			}

			HRslt load(Clip &clip)
			{
				HRslt hr;
				void *buf = NULL;
				ulong bufsize = 0;
				size_t readsize = 0;

				if(!ptr_) return CO_E_NOTINITIALIZED;
				if(!clip.isOpen()) return E_INVALIDARG;

				hr = restore();
				if(!hr) return DXTRACE_ERR(TEXT("restore"), hr.i);

				// Lock the buffer down
				hr = ptr_->Lock(0, (DWORD)buffersize_, &buf, &bufsize, NULL, NULL, 0L );
				if(!hr) return DXTRACE_ERR(TEXT("Lock"), hr.i);

				hr = clip.read(buf, bufsize, &readsize);
				if(!hr) return DXTRACE_ERR(TEXT("Read"), hr.i);
				if( readsize < bufsize ) // 空白埋め
					memset((uint8_t *)buf + readsize, clip.format()->wBitsPerSample == 8 ? 128 : 0, bufsize - readsize);
				// Unlock the buffer, we don't need it anymore.
				ptr_->Unlock(buf, bufsize, NULL, 0);
				return S_OK;
			}

			virtual bool isPlaying()
			{
				if(!ptr_) return false;
				HRslt hr;
				DWORD status;
				hr = ptr_->GetStatus(&status);
				if(hr) return status&DSBSTATUS_PLAYING ? true : false;
				else GCTP_TRACE(hr);
				return false;
			}

			virtual HRslt play(int times)
			{
				if(!ptr_) return CO_E_NOTINITIALIZED;
				HRslt hr;
				hr = ptr_->Play(0, 0, times == 0 ? DSBPLAY_LOOPING : 0);
				if(!hr) GCTP_TRACE(hr);
				return hr;
			}

			virtual HRslt stop() {
				if(!ptr_) return CO_E_NOTINITIALIZED;
				HRslt hr;
				hr = ptr_->Stop();
				if(!hr) GCTP_TRACE(hr);
				return hr;
			}

			virtual HRslt rewind()
			{
				if(!ptr_) return CO_E_NOTINITIALIZED;
				HRslt hr;
				hr = ptr_->SetCurrentPosition(0);
				if(!hr) GCTP_TRACE(hr);
				return hr;
			}

			virtual HRslt restore()
			{
				if(!ptr_) return CO_E_NOTINITIALIZED;

				HRslt hr;
				DWORD dwStatus;
				hr = ptr_->GetStatus( &dwStatus );
				if(!hr) return DXTRACE_ERR(TEXT("GetStatus"), hr.i);

				if( dwStatus & DSBSTATUS_BUFFERLOST ) {
					do {
						hr = ptr_->Restore();
						if(hr == DSERR_BUFFERLOST) Sleep( 10 );
					} while( ( hr = ptr_->Restore() ) == DSERR_BUFFERLOST );

					return S_OK;
				}
				return S_FALSE;
			}

			HRslt onNotified() { return S_OK; }
#ifndef GCTP_AUDIO_USE_TIMER
			HANDLE event() { return 0; }
#endif

			virtual void cleanUp()
			{
				ptr_ = 0;
			}

			virtual HRslt setVolume(float volume)
			{
				// MIN 0～1 MAX
				if(!ptr_) return CO_E_NOTINITIALIZED;
				LONG arg = 0;
				if(volume >= 1) arg = DSBVOLUME_MAX;
				else if(volume <= 0) arg = DSBVOLUME_MIN;
				else if(volume > 0) arg = (LONG)((1-volume)*min_gain);
				return ptr_->SetVolume(arg);
			}

			virtual float getVolume()
			{
				LONG ret;
				HRslt hr;
				if(!ptr_) hr = CO_E_NOTINITIALIZED;
				else hr = ptr_->GetVolume(&ret);
				if(hr) {
					float volume;
					if(ret >= DSBVOLUME_MAX) volume = 1;
					if(ret <= DSBVOLUME_MIN) volume = 0;
					else if(ret > 1) volume = (min_gain-ret)/min_gain;
					if(volume < 0) volume = 0;
					else if(volume > 1) volume = 1;
					return volume;
				}
				else GCTP_TRACE(hr);
				return 0;
			}

			virtual HRslt setPan(float pan)
			{
				/// 左-1～1右
				if(!ptr_) return CO_E_NOTINITIALIZED;
				LONG arg = 0;
				if(pan >= 1) arg = DSBPAN_RIGHT;
				else if(pan <= -1) arg = DSBPAN_LEFT;
				else if(pan != 0) arg = (LONG)(-pan*min_gain);
				return ptr_->SetPan(arg);
			}

			virtual float getPan()
			{
				LONG ret;
				HRslt hr;
				if(!ptr_) hr = CO_E_NOTINITIALIZED;
				else hr = ptr_->GetPan(&ret);
				if(hr) {
					float pan = 0;
					if(ret == DSBPAN_RIGHT) pan = 1;
					else if(ret > 0) pan = -ret/min_gain;
					else if(ret == DSBPAN_LEFT) pan = -1;
					else if(ret < 0) pan = ret/min_gain;
					if(pan < -1) pan = -1;
					else if(pan > 1) pan = 1;
					return pan;
				}
				else GCTP_TRACE(hr);
				return 0;
			}

		protected:
			size_t buffersize_;
			IDirectSoundBuffer8Ptr ptr_;
		};

		/** ストリーミング用バッファ
		 *
		 * ループ素材もこれでやる必要あり
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/01/23 4:14:56
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		class StreamingBuffer : public StaticBuffer {
			enum {
#ifdef GCTP_AUDIO_USE_TIMER
				NOTIFY_PERSEC = Device::NOTIFY_PERSEC,
#else
				NOTIFY_PERSEC = 8,
#endif
				NOTIFY_BUF_LEN_SEC = 1,
				NOTIFY_COUNT = NOTIFY_PERSEC * NOTIFY_BUF_LEN_SEC
			};
		public:
			StreamingBuffer()
				: prev_pcur_(0), prev_wcur_(0), llast_(0), last_written_(0),
#ifndef GCTP_AUDIO_USE_TIMER
				notified_event_(NULL),
#endif
				in_coda_(false), times_(1), count_(0)
			{
				synchronize(true);
			}

#ifndef GCTP_AUDIO_USE_TIMER
			~StreamingBuffer()
			{
				if(notified_event_) CloseHandle(notified_event_);
			}
#endif

			HRslt setUp(IDirectSound8Ptr device, Handle<Clip> clip, bool global_focus)
			{
				HRslt hr;

				if( !device ) return CO_E_NOTINITIALIZED;
				if( !clip || !clip->isOpen() ) return E_INVALIDARG;
				clip_ = clip;

				// Figure out how big the DSound buffer should be
				buffersize_ = clip->format()->nSamplesPerSec * NOTIFY_BUF_LEN_SEC * clip->format()->nBlockAlign;

				DSBUFFERDESC dsbd;
				ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
				dsbd.dwSize          = sizeof(DSBUFFERDESC);
				dsbd.dwFlags         = DSBCAPS_LOCDEFER | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME
#ifndef GCTP_AUDIO_USE_TIMER
									   | DSBCAPS_CTRLPOSITIONNOTIFY
#endif
									   /*| DSBCAPS_GETCURRENTPOSITION2*/; // 正確な再生カーソルなどいるだろうか？
				dsbd.dwBufferBytes   = (DWORD)buffersize_;
				//dsbd.guid3DAlgorithm = guid3DAlgorithm;
				dsbd.lpwfxFormat     = const_cast<WAVEFORMATEX *>(clip->format());
				if(global_focus) dsbd.dwFlags |= DSBCAPS_GLOBALFOCUS;

				IDirectSoundBufferPtr ptr;
				if( !( hr = device->CreateSoundBuffer( &dsbd, &ptr, NULL ) ) ) {
					// If wave format isn't then it will return 
					// either DSERR_BADFORMAT or E_INVALIDARG
					return DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr.i );
				}
				ptr_ = ptr;
				if(!ptr_) return DXTRACE_ERR( TEXT("CreateSoundBuffer"), E_POINTER );

#ifndef GCTP_AUDIO_USE_TIMER
				notified_event_ = CreateEvent( NULL, FALSE, FALSE, NULL );
				if( !notified_event_ ) return DXTRACE_ERR( TEXT("CreateEvent"), E_OUTOFMEMORY );

				IDirectSoundNotifyPtr notify = ptr;
				if( !notify ) return DXTRACE_ERR( TEXT("QueryInterface"), E_POINTER );

				DSBPOSITIONNOTIFY pos_notify[NOTIFY_COUNT];
				//if( pos_notify.size() != NOTIFY_COUNT ) return E_OUTOFMEMORY;

				for( int i = 0; i < NOTIFY_COUNT; i++ ) {
					pos_notify[i].dwOffset     = buffersize_/NOTIFY_COUNT*(i+1) - 1;
					pos_notify[i].hEventNotify = notified_event_;
				}
    
				if( !( hr = notify->SetNotificationPositions( NOTIFY_COUNT, pos_notify ) ) ) {
					return DXTRACE_ERR( TEXT("SetNotificationPositions"), hr.i );
				}
#endif
				return S_OK;
			}

			// サウンドデータ書き込み
			HRslt load(ulong size)
			{
				HRslt hr;
				ulong pcur, wcur; // 現在の再生カーソル位置
				if( !( hr = ptr_->GetCurrentPosition( &pcur, &wcur ) ) ) {
					DXTRACE_ERR( TEXT("GetCurrentPosition"), hr.i );
					return S_FALSE;
				}
				void *buf1 = NULL, *buf2 = NULL;
				ulong buf1size, buf2size;
				if( !( hr = ptr_->Lock( llast_, size, &buf1, &buf1size, &buf2, &buf2size, 0L ) ) ) {
					GCTP_TRACE("ptr->Lock("<<llast_<<","<<size<<","<<buffersize_<<")");
					DXTRACE_ERR( TEXT("Lock"), hr.i );
					return S_FALSE;
				}

				if( in_coda_ ) {
					fillWithSilence(buf1, buf1size);
					if(buf2) fillWithSilence(buf2, buf2size);
				}
				else {
					last_written_ = llast_;
					fill(buf1, buf1size);
					if(buf2) {
						if(!in_coda_) last_written_ = 0;
						fill(buf2, buf2size);
					}
				}
				ptr_->Unlock( buf1, buf1size, buf2, buf2size );
				if(buf2) llast_ = buf2size;
				else llast_ += buf1size;
				if(llast_ == buffersize_) llast_ = 0;
				return S_OK;
			}

			/// Notificationに対する処理
			HRslt onNotified()
			{
				ScopedLock al(monitor_);
				Pointer<Clip> clip = clip_.lock();
				if( !ptr_ || !clip || !clip->isOpen() ) return CO_E_NOTINITIALIZED;
				HRslt hr;
				if( !( hr = restore() ) ) return DXTRACE_ERR( TEXT("restore"), hr.i );

//				if( hr == S_OK ) {
//					if( !( hr = load(wav_) ) ) return DXTRACE_ERR( TEXT("load"), hr.i );
//					return S_OK;
//				} // レストア後はバッファ全体をサウンドデータで埋めてやらなきゃいけないのだが…めんどいので省く
				
				// なんか他の割り込みに反応してしまうことがあるようなので、本当に更新する必要があるのか
				// 確かめる
				ulong notify_size = (ulong)buffersize_/NOTIFY_COUNT/clip->format()->nBlockAlign*clip->format()->nBlockAlign; ///< １回のonNotifiedで処理されるはずのサイズ
				ulong pcur, wcur; // 現在の再生カーソル位置
				if( !( hr = ptr_->GetCurrentPosition( &pcur, &wcur ) ) ) {
					DXTRACE_ERR( TEXT("GetCurrentPosition"), hr.i );
					return S_FALSE;
				}

				if(in_coda_) {
					//PRNN("in coda "<<last_written_<<","<<pcur);
					if((prev_pcur_ < pcur && prev_pcur_ < last_written_ && last_written_ <= pcur)
					|| (prev_pcur_ > pcur && (last_written_ <= pcur || prev_pcur_ < last_written_))) {
						return ptr_->Stop();
					}
				}

				ulong size, maxsize;
				if( pcur <= wcur ) {
					maxsize = (ulong)(buffersize_-wcur+pcur)/clip->format()->nBlockAlign*clip->format()->nBlockAlign;
					if(pcur <= llast_ && llast_ < wcur) llast_ = wcur;
				}
				else {
					maxsize = (ulong)(pcur-wcur)/clip->format()->nBlockAlign*clip->format()->nBlockAlign;
					if(pcur <= llast_ || llast_ < wcur) llast_ = wcur;
				}
				// これだとレイテンシでかいなぁ…あ、レイテンシを下げたい場合は、バッファサイズを小さくしなさい、でいいか
				if( pcur < llast_ ) size = (ulong)(buffersize_-llast_+pcur)/clip->format()->nBlockAlign*clip->format()->nBlockAlign;
				else size = (pcur-llast_)/clip->format()->nBlockAlign*clip->format()->nBlockAlign;
				if(size < notify_size) {
					//PRNN("通知サイズに達してないのに来た？ : " << /*notified_event_ << "," << */wav_.fname() << "(" << size << "/" << notify_size << ")");
					return S_FALSE; // 通知サイズに達してないのに来た？
				}
				//PRNN("通知イベントサービス : " << /*notified_event_ << "," << */wav_.fname() << "(" << size << "/" << notify_size << ")");
				if(size > maxsize) {
					llast_ = wcur;
					size = maxsize;
				}
				// サウンドデータ書き込み
				hr = load(size);
				prev_pcur_ = pcur; prev_wcur_ = wcur;
				return hr;
			}

			HRslt rewind()
			{
				Pointer<Clip> clip = clip_.lock();
				if( !ptr_ || !clip || !clip->isOpen() ) return CO_E_NOTINITIALIZED;

				HRslt hr;
				ScopedLock al(monitor_);
				prev_pcur_ = prev_wcur_ = llast_ = last_written_ = 0; in_coda_ = false;
				clip->rewind();
				return S_OK;
			}

			virtual HRslt play(int times)
			{
				if(!ptr_) return CO_E_NOTINITIALIZED;
				HRslt hr;
				times_ = times;
				count_ = 0;
				in_coda_ = false;
				if(!isPlaying()) {
					hr = rewind();
					if(!hr) GCTP_TRACE(hr);
					hr = ptr_->SetCurrentPosition(0);
					if(!hr) GCTP_TRACE(hr);
					prev_pcur_ = prev_wcur_ = llast_ = last_written_ = 0;
					Pointer<Clip> clip = clip_.lock();
					hr = load((ulong)buffersize_/NOTIFY_COUNT/clip->format()->nBlockAlign*clip->format()->nBlockAlign*2);
					if(!hr) {
						GCTP_TRACE(hr);
						return hr;
					}
				}
				hr = ptr_->Play(0, 0, DSBPLAY_LOOPING);
				if(!hr) GCTP_TRACE(hr);
				return hr;
			}

#ifndef GCTP_AUDIO_USE_TIMER
			HANDLE event() { return notified_event_; }
#endif

		protected:
			HRslt fill(void *buf, ulong size)
			{
				Pointer<Clip> clip = clip_.lock();
				if( !ptr_ || !clip || !clip->isOpen() ) return CO_E_NOTINITIALIZED;
				HRslt hr;
				size_t written_size;
				// Fill the DirectSound buffer with wav data
				if( !( hr = clip->read( buf, size, &written_size ) ) ) return DXTRACE_ERR( TEXT("Read"), hr.i );
				last_written_ += (ulong)written_size;
				if( written_size < size ) {
					count_++;
					if( times_ == 0 || count_ < times_ ) {
						if( !( hr = ptr_->GetCurrentPosition( NULL, &last_written_ ) ) ) {
							DXTRACE_ERR( TEXT("GetCurrentPosition"), hr.i );
							return S_FALSE;
						}
						DWORD total_read = (DWORD)written_size;
						while( total_read < size ) {
							if( !( hr = clip->rewind() ) )
								return DXTRACE_ERR( TEXT("Clip::rewind"), hr.i );
							if( !( hr = clip->read( (BYTE*)buf + total_read, size - total_read, &written_size ) ) )
								return DXTRACE_ERR( TEXT("Clip::read"), hr.i );
							total_read += (DWORD)written_size;
							last_written_ += (ulong)written_size;
						}
					}
					else {
						fillWithSilence( (BYTE*)buf + written_size, size - written_size );
						in_coda_ = true;
					}
				}
				return S_OK;
			}

			void fillWithSilence(void *buf, ulong size)
			{
				if( !ptr_ ) return;
				WAVEFORMATEX format;
				format.wBitsPerSample = 0;
				ptr_->GetFormat(&format, sizeof(WAVEFORMATEX), 0);
				memset( buf, format.wBitsPerSample == 8 ? 128 : 0, size);
			}

			Handle<Clip> clip_;
			ulong prev_pcur_;		///< 前回通知時の再生カーソル位置
			ulong prev_wcur_;		///< 前回通知時の書き込みカーソル位置
			ulong llast_;			///< バッファローカルでの処理済位置

#ifndef GCTP_AUDIO_USE_TIMER
			HANDLE notified_event_;	///< 通知イベントのハンドル
#endif
			ulong last_written_;	///< 最後に空白以外のウェーブを書き込んだバッファローカル位置
			bool in_coda_;			///< （書き込む処理が）ウェーブの最後に達したか？
			int times_;				///< 再生回数設定(0は無限)
			int count_;				///< 再生カウント
			Mutex monitor_;
		};
	}

	/** 普通のバッファを製作して返す
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:43:00
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Pointer<Buffer> newStaticBuffer(IDirectSound8Ptr device, Handle<Clip> clip, bool global_focus)
	{
		if(clip) {
			Pointer<StaticBuffer> buffer = new StaticBuffer;
			if(buffer) {
				HRslt hr = buffer->setUp(device, *clip, global_focus);
				if(hr) return buffer;
				else GCTP_TRACE(hr);
			}
		}
		return 0;
	}

	/** ストリーミングバッファを製作して返す
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:43:00
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Pointer<Buffer> newStreamingBuffer(IDirectSound8Ptr device, Handle<Clip> clip, bool global_focus)
	{
		if(clip) {
			Pointer<StreamingBuffer> buffer = new StreamingBuffer;
			if(buffer) {
				HRslt hr = buffer->setUp(device, clip, global_focus);
				if(hr) return buffer;
				else GCTP_TRACE(hr);
			}
		}
		return 0;
	}

}}} // namespace gctp
