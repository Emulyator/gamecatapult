#ifndef _GCTP_AUDIO_TRACK_HPP_
#define _GCTP_AUDIO_TRACK_HPP_
/** @file
 * GameCatapult オーディオトラッククラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: track.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include <gctp/object.hpp>
#include <gctp/hrslt.hpp>

namespace gctp { namespace audio {

	/** オーディオトラッククラス
	 *
	 * ボイスのグループ。同時再生なのか今なっているどれかの音を消して再生するのか、
	 * ボリュームの一括変更…などを行うためのインターフェース
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/19 19:29:37
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Track : public Object {
		Track();
	};

}} // namespace gctp

#endif //_GCTP_AUDIO_TRACK_HPP_