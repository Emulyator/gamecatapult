/** @file
 * GameCatapult DirectX Audioクラス
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
	Player::Player(Pointer<Track> track, Pointer<Clip> clip) : track_(track), clip_(clip)
	{
	}

	/** 再生中か？
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:24:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	bool Player::isPlaying()
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
	bool Player::play(int times)
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
	void Player::reset()
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
	void Player::stop()
	{
		if(track_) track_->stop();
	}

	void Player::setVolume(float volume)
	{
		if(track_) {
			HRslt hr = track_->setVolume(volume);
			if(!hr) GCTP_TRACE(hr);
		}
	}

	float Player::getVolume()
	{
		if(track_) return track_->getVolume();
		return 0;
	}

	void Player::setPan(float pan)
	{
		if(track_) {
			HRslt hr = track_->setPan(pan);
			if(!hr) GCTP_TRACE(hr);
		}
	}

	float Player::getPan()
	{
		if(track_) return track_->getPan();
		return 0;
	}

}} // namespace gctp
