#ifndef _GCTP_AUDIO_DX_BUFFER_HPP_
#define _GCTP_AUDIO_DX_BUFFER_HPP_
/** @file
 * GameCatapult DirectSoundBufferクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/25 19:35:25
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/def.hpp>
#include <gctp/hrslt.hpp>
#include <gctp/audio/dx/device.hpp>
#include <gctp/com_ptr.hpp>
#include <boost/shared_ptr.hpp>

namespace gctp { namespace audio { namespace dx {

	/** DirectSoundBuffer抽象基底クラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/19 19:29:37
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Buffer {
	public:
		virtual ~Buffer();

		virtual bool isPlaying() = 0;
		virtual HRslt play(bool loop) = 0;
		virtual HRslt stop() = 0;
		virtual HRslt rewind() = 0;
		virtual HRslt onNotified() = 0;
#ifndef GCTP_AUDIO_USE_TIMER
		virtual HANDLE event() = 0;
#endif
		virtual void doChangeVolume() = 0;
		virtual HRslt restore() = 0;
		virtual void cleanUp() = 0;
	};

	BufferPtr newStaticBuffer(IDirectSound8Ptr device, WavFile &wav, bool global_focus);
	BufferPtr newStreamingBuffer(IDirectSound8Ptr device, const _TCHAR *fname, bool global_focus);

}}} // namespace gctp

#endif //_GCTP_AUDIO_DX_BUFFER_HPP_