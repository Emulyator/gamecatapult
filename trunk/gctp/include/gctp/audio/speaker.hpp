#ifndef _GCTP_AUDIO_SPEAKER_HPP_
#define _GCTP_AUDIO_SPEAKER_HPP_
/** @file
 * GameCatapult �R�c�����N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: player.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include <gctp/def.hpp>
#include <gctp/audio/track.hpp>
#include <gctp/vector.hpp>

namespace gctp { namespace audio {

	class Clip;

	/** �Đ��n���h���N���X
	 *
	 * �����Player�Ɠ������Ă��܂��ėǂ��񂶂�Ȃ����H
	 * Track��Clip�؂�ւ��@�\�����Ă��炿���ƍl���悤
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/20 3:31:53
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Speaker {
	public:
		Speaker() {}
		Speaker(Pointer<Track> track, Pointer<Clip> clip = 0);

		bool play(int times = 1);
		bool isPlaying();
		void stop();
		void release();
		void reset();
		void setPosition(const Vector &pos);
		Vector getPosition();
		void setVelocity(const Vector &vel);
		Vector getVelocity();
		void setMaxDistance(float val);
		float getMaxDistance();
		void setMinDistance(float val);
		float getMinDistance();
		void setDirection(const Vector &);
		Vector getDirection();
		void setTheta(float val);
		float getTheta();
		void setPhi(float val);
		float getPhi();
		void setOutsideVolume(float val);
		float getOutsideVolume();
		void commit();

	private:
		Pointer<Track> track_;
		Pointer<Clip> clip_;
	};

}} // namespace gctp

#endif //_GCTP_AUDIO_SPEAKER_HPP_