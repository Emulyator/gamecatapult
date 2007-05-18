#ifndef _GCTP_AUDIO_CLIP_HPP_
#define _GCTP_AUDIO_CLIP_HPP_
/** @file
 * GameCatapult オーディオクリップクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/25 19:11:31
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/def.hpp>
#include <gctp/pointer.hpp>

namespace gctp { namespace audio {

	namespace dx {
		class Buffer;
	}

	typedef dx::Buffer Clip;
	typedef Pointer<dx::Buffer> ClipPtr;
	typedef Handle<dx::Buffer> ClipHndl;

}} // namespace gctp

#endif //_GCTP_AUDIO_CLIP_HPP_