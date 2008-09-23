/** @file
 * GameCatapult ３Ｄ音源クラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/audio/player.hpp>
#include <gctp/audio.hpp>
#include <gctp/audio/dx/buffer.hpp>
//#include <gctp/audio/track.hpp> // いつかこうなる…
#include <gctp/audio/clip.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace audio {

	/** コンストラクタ
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:23:53
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Speaker::Speaker(Pointer<Track> track, Pointer<Clip> clip) : track_(track), clip_(clip)
	{
	}

	/** 再生中か？
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:24:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	bool Speaker::isPlaying()
	{
		if(track_ && track_->isPlaying()) return true;
		return false;
	}

	/** 再生
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:24:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	bool Speaker::play(int times)
	{
		if(isPlaying()) {
			if(track_ && track_->rewind() && track_->play(times)) return true;
		}
		else {
			if(track_ && track_->play(times)) return true;
		}
		return false;
	}

	/** リセット
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:24:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void Speaker::reset()
	{
		if(track_) {
			if(track_->isPlaying()) track_->stop();
			track_->rewind();
		}
	}

	/** 停止
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:24:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void Speaker::stop()
	{
		if(track_) track_->stop();
	}

	void Speaker::setVolume(float volume)
	{
		if(track_) {
			HRslt hr = track_->setVolume(volume);
			if(!hr) GCTP_TRACE(hr);
		}
	}

	float Speaker::getVolume()
	{
		if(track_) return track_->getVolume();
		return 0;
	}

	void Speaker::setFrequency(ulong freq)
	{
		if(track_) {
			HRslt hr = track_->setFrequency(freq);
			if(!hr) GCTP_TRACE(hr);
		}
	}

	ulong Speaker::getFrequency()
	{
		if(track_) track_->getFrequency();
		return 0;
	}

	void Speaker::setPosition(const Vector &pos)
	{
		if(track_) track_->setPosition(pos);
	}

	Vector Speaker::getPosition()
	{
		if(track_) return track_->getPosition();
		return VectorC(0,0,0);
	}

	void Speaker::setVelocity(const Vector &vel)
	{
		if(track_) track_->setVelocity(vel);
	}

	Vector Speaker::getVelocity()
	{
		if(track_) return track_->getVelocity();
		return VectorC(0,0,0);
	}

	void Speaker::setMaxDistance(float val)
	{
		if(track_) return track_->setMaxDistance(val);
	}

	float Speaker::getMaxDistance()
	{
		if(track_) return track_->getMaxDistance();
		return 0;
	}

	void Speaker::setMinDistance(float val)
	{
		if(track_) return track_->setMinDistance(val);
	}

	float Speaker::getMinDistance()
	{
		if(track_) return track_->getMinDistance();
		return 0;
	}

	void Speaker::setDirection(const Vector &dir)
	{
		if(track_) track_->setDirection(dir);
	}

	Vector Speaker::getDirection()
	{
		if(track_) return track_->getDirection();
		return VectorC(0,0,0);
	}

	void Speaker::setTheta(float val)
	{
		if(track_) return track_->setTheta(val);
	}

	float Speaker::getTheta()
	{
		if(track_) return track_->getTheta();
		return 0;
	}

	void Speaker::setPhi(float val)
	{
		if(track_) return track_->setPhi(val);
	}

	float Speaker::getPhi()
	{
		if(track_) return track_->getPhi();
		return 0;
	}

	void Speaker::setOutsideVolume(float val)
	{
		if(track_) return track_->setOutsideVolume(val);
	}

	float Speaker::getOutsideVolume()
	{
		if(track_) return track_->getMaxDistance();
		return 0;
	}

	void Speaker::commit()
	{
		if(track_) track_->commit();
	}

}} // namespace gctp
