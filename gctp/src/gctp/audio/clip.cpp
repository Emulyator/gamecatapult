/** @file
 * GameCatapult オーディオクリップクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/25 19:21:07
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/audio.hpp>
#include <gctp/audio/clip.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace audio {

	/** オーディクリップ製作
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:22:01
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt Clip::setUp(const char *name)
	{
		HRslt hr;
		return hr;
	}
	
	/** AudioClipオブジェクトを破棄
	 *
	 */
	void Clip::tearDown()
	{
	}

}} // namespace gctp
