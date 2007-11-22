#ifndef _GCTP_AUDIO_DX_DEVICE_HPP_
#define _GCTP_AUDIO_DX_DEVICE_HPP_
/** @file
 * GameCatapult DirectX Audioクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/04 23:19:54
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/def.hpp>
#include <gctp/pointer.hpp>
#include <gctp/hrslt.hpp>
#include <gctp/mutex.hpp>
#include <gctp/com_ptr.hpp>
#include <vector>

// 割り込み(DirectSoundNotify)でなくタイマーで通知制御にするには以下のフラグをオンにする
#define GCTP_AUDIO_USE_TIMER

namespace gctp { namespace audio {
	class Clip;
}}

namespace gctp { namespace audio { namespace dx {

	class Buffer;

	TYPEDEF_DXCOMPTR(IDirectSound8);

	/** オーディオデバイスクラス
	 *
	 * DirectSoundをあらわすクラス。<BR>
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 10:11:11
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Device {
	public:
#ifdef GCTP_AUDIO_USE_TIMER
		enum {
			NOTIFY_PERSEC = 4,
		};
#endif
		Device();

		~Device();

		HRslt open(HWND hwnd, DWORD coop_mode, bool global_focus);

		void close();

		HRslt setFormat(int channel_num, int freq, int bitrate);

		HRslt getFormat(int &channel_num, int &freq, int &bitrate);

		Pointer<Buffer> newStream(Handle<Clip> clip);
		Pointer<Buffer> newBuffer(Handle<Clip> clip);

		void setVolume(float volume);
		
		float volume();
		
		/// デバイス消失時のリソース解放
		void cleanUp();

		IDirectSound8Ptr ptr() { return ptr_; }
	private:
		void add(Handle<Buffer> buffer);
		/// 通知イベント監視スレッドの処理
		static DWORD WINAPI handleEvents( void *param );

		DWORD  notify_thread_id_;
		HANDLE notify_thread_;
		Mutex monitor_;

		typedef std::vector< Handle<Buffer> > BufferList;
		BufferList buffers_;

		float volume_;	///< トラックボリューム
		bool global_focus_;

		IDirectSound8Ptr ptr_;
	};

}}} // namespace gctp

#endif //_GCTP_AUDIO_DX_DEVICE_HPP_