#ifndef _GCFPSINFO_H_
#define _GCFPSINFO_H_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult FPS(Flip per second)�v���N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: fpsinfo.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include <float.h>
#include <gctp/timer.hpp>

namespace gctp {

	/** FPS�v���N���X
	 *
	 * ���Ԃ̒P�ʂ͕b
	 */
	class FPSInfo
	{
	public:
		FPSInfo() : update_delta(1.0) {
			reset();
		}
		FPSInfo(float update_span) : update_delta(update_span) {
			reset();
		}
		void reset() {
			timer.reset();
			FPS = maxFPS = totalave = latestave = w_totalave = w_latestave = 0;
			update_count = 0;
			minFPS = FLT_MAX;
			cnt_totalave = cnt_latestave = 0;
		}
		void reset_ex() {
			timer.reset();
			maxFPS = 0;
			minFPS = FLT_MAX;
			totalave = latestave;
		}

		float FPS;			///< �u�Ԃ�FPS
		float maxFPS;		///< �ő�FPS
		float minFPS;		///< �ŏ�FPS
		float totalave;		///< �v���J�n����̕���
		float latestave;	///< (update_delta�Ŏ������)���̍X�V�p�x�ɂ��Z����FPS
		float update_delta;	///< latestave�̍X�V�Ԋu
		
		/** �v��
		 * @return ���b�v�^�C��
		 */
		float update();
		
	private:
		Timer timer;
		float w_totalave; float w_latestave; float update_count;
		int cnt_totalave; int cnt_latestave;
	};

} //namespace gctp

#endif //_GCFPSINFO_H_