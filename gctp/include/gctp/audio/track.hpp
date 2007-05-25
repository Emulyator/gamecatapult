#ifndef _GCTP_AUDIO_TRACK_HPP_
#define _GCTP_AUDIO_TRACK_HPP_
/** @file
 * GameCatapult �I�[�f�B�I�g���b�N�N���X
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: track.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include <gctp/pointer.hpp>

namespace gctp { namespace audio {

	namespace dx {
		class Buffer;
	}

	/** �I�[�f�B�I�g���b�N�N���X
	 *
	 * ���̂Ƃ���dx::Buffer�̃V�m�j��
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/19 19:29:37
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	typedef dx::Buffer Track;

}} // namespace gctp

#endif //_GCTP_AUDIO_TRACK_HPP_