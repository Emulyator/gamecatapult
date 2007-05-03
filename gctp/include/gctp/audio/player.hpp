#ifndef _GCTP_AUDIO_PLAYER_HPP_
#define _GCTP_AUDIO_PLAYER_HPP_
/** @file
 * GameCatapult �v���C���[�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: player.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include <gctp/def.hpp>
#include <boost/shared_ptr.hpp>

namespace gctp { namespace audio {
	namespace dx{
		class Buffer;
		class WavFile;
	}

	/** �Đ��n���h���N���X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/20 3:31:53
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Player {
	public:
		Player() {}
		Player(boost::shared_ptr<dx::Buffer> ptr);

		bool play(bool loop = false);
		bool isPlaying();
		void stop();
		void release();
		void reset();
	private:
		boost::shared_ptr<dx::Buffer> ptr_;
	};

}} // namespace gctp

#endif //_GCTP_AUDIO_PLAYER_HPP_