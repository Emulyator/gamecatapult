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
#include <gctp/audio/dx/buffer.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace audio {

	/** コンストラクタ
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:23:53
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Player::Player(boost::shared_ptr<dx::Buffer> ptr) : ptr_(ptr)
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
		if(ptr_ && ptr_->isPlaying()) return true;
		return false;
	}

	/** 再生
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:24:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	bool Player::play(bool loop)
	{
		if(isPlaying()) {
			if(ptr_ && ptr_->rewind()) return true;
		}
		else {
			if(ptr_ && ptr_->play(loop)) return true;
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
		if(ptr_) {
			if(ptr_->isPlaying()) ptr_->stop();
			ptr_->rewind();
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
		if(ptr_) ptr_->stop();
	}

}} // namespace gctp
