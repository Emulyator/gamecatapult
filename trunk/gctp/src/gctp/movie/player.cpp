/** @file
 * GameCatapult 動画再生クラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/movie/player.hpp>
#include <gctp/movie/dx/player.hpp>
#include <gctp/graphic/texture.hpp>
#include <gctp/graphic/spritebuffer.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace movie {

	/** コンストラクタ
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:24:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Player::Player()
	{
		ptr_ = new dx::Player;
	}

	/** テクスチャをターゲットにオープン
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:24:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt Player::openForTexture(const _TCHAR *name, bool audio_on)
	{
		return ptr_->openForTexture(name, audio_on);
	}

	/** ムービーを開く
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:24:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt Player::open(HWND hwnd, int notify_msgid, const _TCHAR *name, bool audio_on)
	{
		return ptr_->open(hwnd, notify_msgid, name, audio_on);
	}

	/** 再生中か？
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:24:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	bool Player::isPlaying()
	{
		return ptr_->isPlaying();
	}

	/** 再生
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:24:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	bool Player::play(int loop)
	{
		return ptr_->play(loop) ? true : false;
	}

	/** リセット
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:24:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void Player::reset()
	{
	}

	/** 停止
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:24:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void Player::stop()
	{
		ptr_->stop();
	}

	void Player::setVolume(float volume)
	{
		ptr_->setVolume(volume);
	}

	float Player::getVolume()
	{
		return ptr_->getVolume();
	}

	void Player::setPan(float pan)
	{
		ptr_->setVolume(pan);
	}

	float Player::getPan()
	{
		return ptr_->getPan();
	}

	/** テクスチャを取得
	 *
	 * openForTextureを使用した場合のみ有効
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:24:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Handle<graphic::Texture> Player::getTexture()
	{
		return ptr_->getTexture();
	}

	Rectf Player::getUVRect()
	{
		RectfC ret(0, 0, 0, 0);
		Handle<graphic::Texture> tex = getTexture();
		if(tex) {
			Point2 tex_size = tex->size();
			int movie_w = width();
			int movie_h = height();
			if(movie_w > 0) {
				ret.left = 0;
				ret.right = (float)(movie_w+0.5f)/(float)tex_size.x;
			}
			else {
				ret.left = (float)(-movie_w-0.5f)/(float)tex_size.x;
				ret.right = 0;
			}
			if(movie_h > 0) {
				ret.top = 0;
				ret.bottom = (float)(movie_h+0.5f)/(float)tex_size.y;
			}
			else {
				ret.top = (float)(-movie_h-0.5f)/(float)tex_size.y;
				ret.bottom = 0;
			}
		}
		return ret;
	}

	graphic::SpriteDesc Player::getDesc()
	{
		graphic::SpriteDesc ret;
		ret.setPos(RectC(0, 0, abs(width()), abs(height())));
		ret.setUV(getUVRect());
		ret.setColor(Color32(255, 255, 255));
		ret.setHilight(Color32(0, 0, 0));
		return ret;
	}

	HRslt Player::checkStatus()
	{
		return ptr_->checkStatus();
	}

	int Player::width()
	{
		return ptr_->width();
	}

	int Player::height()
	{
		return ptr_->height();
	}

}} // namespace gctp
