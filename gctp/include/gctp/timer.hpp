#ifndef _GCTP_TIMER_HPP_
#define _GCTP_TIMER_HPP_
#include <gctp/config.hpp>
#include <gctp/def.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult ���Ԍv���N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 18:09:11
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */

#define GCTP_USE_QueryPerformanceCounter

#ifdef GCTP_USE_QueryPerformanceCounter
#include <windows.h>

namespace gctp {

	/** �჌�x���J�E���^�[�N���X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 18:09:26
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Timer
	{
	public:
		typedef LARGE_INTEGER TickType;
		/// �X�g�b�v�E�H�b�`�����Z�b�g
		inline void reset() { QueryPerformanceCounter(&start_); }
		/// �X�g�b�v�E�H�b�`�̃��b�v�{�^���i���̑O��reset()�Ƃ̌o�ߎ��Ԃ�b�ŕԂ��j
		inline float elapsed() { return static_cast<float>(static_cast<double>(tick().QuadPart-start_.QuadPart)/frequency_.QuadPart); }
		
		/** �`�b�N�J�E���g��Ԃ�
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/01/29 18:09:35
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		inline static TickType tick()
		{
			TickType ret;
			QueryPerformanceCounter(&ret);
			return ret;
		}

		/// 1tick�����b�ɑ������邩�H
		inline static double ticks_per_sec() { return 1.0/(double)frequency_.QuadPart; }

		/// �N������̌o�ߎ��Ԃ��~���b�P�ʂŎ擾
		inline static ulong time() { return timeGetTime(); }

		/** Windows���L
		 *
		 * �N���b�N�J�E���g�����b�ɑ������邩���v�����邽�߂̏���
		 */
		static void coinitialize();
		static void initialize();

	private:
		TickType start_;
		static TickType frequency_;
	};

} //namespace gctp

#else
// Win32��p
#include <mmsystem.h>

namespace gctp {

	/** �჌�x���J�E���^�[�N���X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 18:09:26
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Timer
	{
	public:
#ifdef _MSC_VER
# if _MSC_VER > 1200
		typedef uint64_t TickType;
# else
		typedef int64_t TickType;
# endif
#else
		typedef uint32_t TickType;
#endif
		/// �X�g�b�v�E�H�b�`�����Z�b�g
		inline void reset() { start_ = tick(); }
		/// �X�g�b�v�E�H�b�`�̃��b�v�{�^���i���̑O��reset()�Ƃ̌o�ߎ��Ԃ�b�ŕԂ��j
		inline float elapsed() { return static_cast<float>(static_cast<int64_t>(tick()-start_)/ticks_per_sec_); }

		/** �`�b�N�J�E���g��Ԃ�
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/01/29 18:09:35
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		inline static TickType __fastcall tick()
		{
			__asm {
				cpuid
				rdtsc
			}
		};

		/// 1tick�����b�ɑ������邩�H
		inline static double ticks_per_sec() { return ticks_per_sec_; }

		/// �N������̌o�ߎ��Ԃ��~���b�P�ʂŎ擾
		inline static ulong time() { return timeGetTime(); }

		/** Windows���L
		 *
		 * �N���b�N�J�E���g�����b�ɑ������邩���v�����邽�߂̏���
		 */
		static void coinitialize();
		static void initialize(); ///< coinitialize�ďo����500ms�ȏ�o���Ă���Ăяo��

	private:
		TickType start_;
		static double ticks_per_sec_;
	};

} //namespace gctp

#endif

#endif //_GCTP_TIMER_HPP_