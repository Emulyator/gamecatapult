/** @file
 * GameCatapult DirectSoundBufferクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/25 19:35:25
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/audio/dx/buffer.hpp>
#include <gctp/audio/dx/wavfile.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace audio { namespace dx {

	namespace {
		TYPEDEF_DXCOMPTR(IDirectSoundBuffer);
		TYPEDEF_DXCOMPTR(IDirectSoundBuffer8);
		TYPEDEF_DXCOMPTR(IDirectSound3DBuffer8);
		TYPEDEF_DXCOMPTR(IDirectSound3DListener8);
		TYPEDEF_DXCOMPTR(IDirectSoundNotify);
	}

	Buffer::~Buffer()
	{
	}

	namespace {
		class StaticBuffer : public Buffer {
		public:
			HRslt setUp(IDirectSound8Ptr device, bool global_focus) {
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

			HRslt setUp(IDirectSound8Ptr device, WavFile &wav, bool global_focus) {
				if( !device ) return CO_E_NOTINITIALIZED;
				DSBUFFERDESC dsbd;
 
				// DSBUFFERDESC 構造体を設定する。
				memset(&dsbd, 0, sizeof(DSBUFFERDESC));
				dsbd.dwSize = sizeof(DSBUFFERDESC);
				dsbd.dwFlags = DSBCAPS_LOCDEFER | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
				buffersize_ = dsbd.dwBufferBytes = wav.size();
				dsbd.lpwfxFormat = const_cast<WAVEFORMATEX *>(wav.format());
				if(global_focus) dsbd.dwFlags |= DSBCAPS_GLOBALFOCUS;
 
				// バッファを作成する。
				IDirectSoundBufferPtr ptr;
				HRslt hr = device->CreateSoundBuffer(&dsbd, &ptr, NULL);
				if(!hr) return hr;
				else ptr_ = ptr;
				return load(wav);
			}

			HRslt load(WavFile &wav) {
				HRslt hr;
				void *buf = NULL;
				ulong bufsize = 0;
				size_t readsize = 0;

				if(!ptr_) return CO_E_NOTINITIALIZED;
				if(!wav.isOpen()) return E_INVALIDARG;

				hr = restore();
				if(!hr) return DXTRACE_ERR(TEXT("restore"), hr.i);

				// Lock the buffer down
				hr = ptr_->Lock(0, buffersize_, &buf, &bufsize, NULL, NULL, 0L );
				if(!hr) return DXTRACE_ERR(TEXT("Lock"), hr.i);

				wav.rewind();

				hr = wav.read(buf, bufsize, &readsize);
				if(!hr) return DXTRACE_ERR(TEXT("Read"), hr.i);
				if( readsize < bufsize ) // 空白埋め
					memset((uint8_t *)buf + readsize, wav.format()->wBitsPerSample == 8 ? 128 : 0, bufsize - readsize);
				// Unlock the buffer, we don't need it anymore.
				ptr_->Unlock(buf, bufsize, NULL, 0);
				return S_OK;
			}

			virtual bool isPlaying() {
				if(!ptr_) return false;
				HRslt hr;
				DWORD status;
				hr = ptr_->GetStatus(&status);
				if(hr) return status&DSBSTATUS_PLAYING ? true : false;
				else GCTP_TRACE(hr);
				return false;
			}

			virtual HRslt play(bool loop) {
				if(!ptr_) return CO_E_NOTINITIALIZED;
				HRslt hr;
				hr = ptr_->Play(0, 0, loop ? DSBPLAY_LOOPING : 0);
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

			virtual void doChangeVolume() {
			}

			virtual HRslt restore() {
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

			virtual void cleanUp() {
				ptr_ = 0;
			}

		protected:
			float volume_;	///< トラックボリューム
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
			typedef boost::mutex::scoped_lock Lock;
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
				: glast_(0), llast_(0),
#ifndef GCTP_AUDIO_USE_TIMER
				notified_event_(NULL),
#endif
				in_coda_(false), do_loop_(false)
			{
			}

#ifndef GCTP_AUDIO_USE_TIMER
			~StreamingBuffer()
			{
				if(notified_event_) CloseHandle(notified_event_);
			}
#endif

			HRslt setUp(IDirectSound8Ptr device, const _TCHAR *fname, bool global_focus) {
				HRslt hr;

				if( !device ) return CO_E_NOTINITIALIZED;
				if( fname == NULL) return E_INVALIDARG;
				if( !( hr = wav_.open(fname) ) ) return hr;

				// Figure out how big the DSound buffer should be
				buffersize_ = wav_.format()->nSamplesPerSec * NOTIFY_BUF_LEN_SEC * wav_.format()->nBlockAlign;

				DSBUFFERDESC dsbd;
				ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
				dsbd.dwSize          = sizeof(DSBUFFERDESC);
				dsbd.dwFlags         = DSBCAPS_LOCDEFER | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME
#ifndef GCTP_AUDIO_USE_TIMER
									   | DSBCAPS_CTRLPOSITIONNOTIFY
#endif
									   /*| DSBCAPS_GETCURRENTPOSITION2*/; // 正確な再生カーソルなどいるだろうか？
				dsbd.dwBufferBytes   = buffersize_;
				//dsbd.guid3DAlgorithm = guid3DAlgorithm;
				dsbd.lpwfxFormat     = const_cast<WAVEFORMATEX *>(wav_.format());
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
				glast_ = buffersize_;
				return load(wav_);
			}

			/// Notificationに対する処理
			HRslt onNotified() {
				Lock al(monitor_);
				if( !ptr_ || !wav_.isOpen() ) return CO_E_NOTINITIALIZED;
				HRslt hr;
				if( !( hr = restore() ) ) return DXTRACE_ERR( TEXT("restore"), hr.i );

//				if( hr == S_OK ) {
//					if( !( hr = load(wav_) ) ) return DXTRACE_ERR( TEXT("load"), hr.i );
//					return S_OK;
//				} // レストア後はバッファ全体をサウンドデータで埋めてやらなきゃいけないのだが…めんどいので省く
				
				// なんか他の割り込みに反応してしまうことがあるようなので、本当に更新する必要があるのか
				// 確かめる
				ulong notify_size = buffersize_/NOTIFY_COUNT/wav_.format()->nBlockAlign*wav_.format()->nBlockAlign; ///< １回のonNotifiedで処理されるはずのサイズ
				ulong size, maxsize, current, writebegin; // 現在の再生カーソル位置
				if( !( hr = ptr_->GetCurrentPosition( &current, &writebegin ) ) ) {
					DXTRACE_ERR( TEXT("GetCurrentPosition"), hr.i );
					return S_FALSE;
				}
				if( current <= writebegin ) {
					maxsize = (buffersize_-writebegin+current)/wav_.format()->nBlockAlign*wav_.format()->nBlockAlign;
					if(current <= llast_ && llast_ < writebegin) llast_ = writebegin;
				}
				else {
					maxsize = (current-writebegin)/wav_.format()->nBlockAlign*wav_.format()->nBlockAlign;
					if(current <= llast_ || llast_ < writebegin) llast_ = writebegin;
				}
				if( current < llast_ ) size = (buffersize_-llast_+current)/wav_.format()->nBlockAlign*wav_.format()->nBlockAlign;
				else size = (current-llast_)/wav_.format()->nBlockAlign*wav_.format()->nBlockAlign;
				if(size < notify_size) {
					//PRNN("通知サイズに達してないのに来た？ : " << /*notified_event_ << "," << */wav_.fname() << "(" << size << "/" << notify_size << ")");
					return S_FALSE; // 通知サイズに達してないのに来た？
				}
				//PRNN("通知イベントサービス : " << /*notified_event_ << "," << */wav_.fname() << "(" << size << "/" << notify_size << ")");
				if(size > maxsize) {
					llast_ = writebegin;
					size = maxsize;
				}

				void *buf1 = NULL, *buf2 = NULL;
				ulong buf1size, buf2size;
				if( !( hr = ptr_->Lock( llast_, size, &buf1, &buf1size, &buf2, &buf2size, 0L ) ) ) {
					GCTP_TRACE("ptr->Lock("<<llast_<<","<<size<<","<<buffersize_<<","<<current<<")");
					DXTRACE_ERR( TEXT("Lock"), hr.i );
					return S_FALSE;
				}

				if( in_coda_ ) {
					fillWithSilence(buf1, buf1size);
					if(buf2) fillWithSilence(buf2, buf2size);
				}
				else {
					fill(buf1, buf1size);
					if(buf2) fill(buf2, buf2size);
					glast_ += size;
				}
				llast_ += size;
				if(llast_ >= buffersize_) llast_ = llast_ % buffersize_;
				ptr_->Unlock( buf1, buf1size, buf2, buf2size );

				if(in_coda_ && glast_ >= wav_.size()) ptr_->Stop();
				return S_OK;
			}

			HRslt rewind() {
				if(!ptr_ || !wav_.isOpen() ) return CO_E_NOTINITIALIZED;

				HRslt hr;
				Lock al(monitor_);
				glast_ = llast_ = 0; in_coda_ = false;
				wav_.rewind();
				if( !( hr = load(wav_) ) ) return DXTRACE_ERR( TEXT("load"), hr.i );
				return ptr_->SetCurrentPosition(0);
			}

			virtual HRslt play(bool loop) {
				if(!ptr_) return CO_E_NOTINITIALIZED;
				HRslt hr;
				if(in_coda_) {
					hr = rewind();
					if(!hr) GCTP_TRACE(hr);
				}
				do_loop_ = loop;
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
				HRslt hr;
				size_t written_size;
				// Fill the DirectSound buffer with wav data
				if( !( hr = wav_.read( buf, size, &written_size ) ) ) return DXTRACE_ERR( TEXT("Read"), hr.i );
				if( written_size < size ) {
					if( do_loop_ ) {
						DWORD total_read = written_size;
						while( total_read < size ) {
							if( !( hr = wav_.rewind() ) )
								return DXTRACE_ERR( TEXT("WavFile::rewind"), hr.i );
							if( !( hr = wav_.read( (BYTE*)buf + total_read, size - total_read, &written_size ) ) )
								return DXTRACE_ERR( TEXT("WavFile::read"), hr.i );
							total_read += written_size;
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
				memset( buf, wav_.format()->wBitsPerSample == 8 ? 128 : 0, size);
			}

			WavFile wav_;
			ulong glast_;			///< ストリーム全体での処理済位置(バッファ終端のグローバル位置を示す。ストリーム全体での再生カーソル位置はglast_+abs((current-llast_)%buffersize_)で求まる)
			ulong llast_;			///< バッファローカルでの処理済位置

#ifndef GCTP_AUDIO_USE_TIMER
			HANDLE notified_event_;	///< 通知イベントのハンドル
#endif
			bool in_coda_;			///< ウェーブの最後に達したか？
			bool do_loop_;			///< ループ再生するか？
			boost::mutex monitor_;
		};
	}

	/** 普通のバッファを製作して返す
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:43:00
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	BufferPtr newStaticBuffer(IDirectSound8Ptr device, WavFile &wav, bool global_focus)
	{
		StaticBuffer *buffer = new StaticBuffer;
		buffer->setUp(device, wav, global_focus);
		return BufferPtr(buffer);
	}

	/** ストリーミングバッファを製作して返す
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:43:00
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	BufferPtr newStreamingBuffer(IDirectSound8Ptr device, const _TCHAR *fname, bool global_focus)
	{
		StreamingBuffer *buffer = new StreamingBuffer;
		buffer->setUp(device, fname, global_focus);
		return BufferPtr(buffer);
	}

}}} // namespace gctp
