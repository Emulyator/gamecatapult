#ifndef _GCTP_AUDIO_DX_BUFFER_HPP_
#define _GCTP_AUDIO_DX_BUFFER_HPP_
/** @file
 * GameCatapult DirectSoundBufferクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/25 19:35:25
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/audio/dx/device.hpp>
#include <gctp/vector.hpp>

namespace gctp { namespace audio {
	class Clip;
}}

namespace gctp { namespace audio { namespace dx {

	/** DirectSoundBuffer抽象基底クラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/19 19:29:37
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Buffer : public Object {
	public:
		virtual ~Buffer();

		virtual bool isPlaying() = 0;
		virtual HRslt play(int times) = 0;
		virtual HRslt stop() = 0;
		virtual HRslt rewind() = 0;
		virtual HRslt setVolume(float) = 0;
		virtual float getVolume() = 0;
		virtual HRslt setPan(float) = 0;
		virtual float getPan() = 0;
		virtual HRslt onNotified() = 0;
#ifndef GCTP_AUDIO_USE_TIMER
		virtual HANDLE event() = 0;
#endif
		virtual HRslt restore() = 0;
		virtual void cleanUp() = 0;

		// 3Dサウンド系
		virtual void setPosition(const Vector &) = 0;
		virtual Vector getPosition() = 0;
		virtual void setVelocity(const Vector &) = 0;
		virtual Vector getVelocity() = 0;
		virtual void setMaxDistance(float) = 0;
		virtual float getMaxDistance() = 0;
		virtual void setMinDistance(float) = 0;
		virtual float getMinDistance() = 0;
		virtual void setDirection(const Vector &) = 0;
		virtual Vector getDirection() = 0;
		virtual void setTheta(float) = 0;
		virtual float getTheta() = 0;
		virtual void setPhi(float) = 0;
		virtual float getPhi() = 0;
		virtual void setOutsideVolume(float) = 0;
		virtual float getOutsideVolume() = 0;
		virtual void commit() = 0;
	};

	Pointer<Buffer> newStaticBuffer(IDirectSound8Ptr device, Handle<Clip> clip, bool global_focus);
	Pointer<Buffer> newStreamingBuffer(IDirectSound8Ptr device, Handle<Clip> clip, bool global_focus);
	Pointer<Buffer> newStatic3DBuffer(IDirectSound8Ptr device, Handle<Clip> clip, bool global_focus);
	Pointer<Buffer> newStreaming3DBuffer(IDirectSound8Ptr device, Handle<Clip> clip, bool global_focus);

}}} // namespace gctp

#endif //_GCTP_AUDIO_DX_BUFFER_HPP_