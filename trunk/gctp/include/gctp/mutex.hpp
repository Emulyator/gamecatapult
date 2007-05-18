#ifndef _GCTP_MUTEX_HPP_
#define _GCTP_MUTEX_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult �r������N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/09 23:00:42
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/def.hpp>
#include <windows.h>

namespace gctp {

	/** GameCatapult Mutex�I�u�W�F�N�g
	 *
	 * boost::mutex�ƌ���
	 */
	class Mutex {
	public:
		Mutex()
		{
			::InitializeCriticalSection(&cs_);
		}
		~Mutex()
		{
			::DeleteCriticalSection(&cs_);
		}
		// ���b�N�擾
		void lock()
		{
			::EnterCriticalSection(&cs_);
		}
		// ���b�N����
		void unlock()
		{
			::LeaveCriticalSection(&cs_);
		}
	private:
		CRITICAL_SECTION cs_;
	};

	/// �X�R�[�v�ɂ�鎩�����b�N����
	class ScopedLock {
	public:
		ScopedLock(Mutex &mutex) : mutex_(mutex) { mutex_.lock(); }
		~ScopedLock() { mutex_.unlock(); }
	private:
		Mutex &mutex_;
	};

} // namespace gctp

#endif //_GCTP_MUTEX_HPP_