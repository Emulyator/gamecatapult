#ifndef _GCTP_MUTEX_HPP_
#define _GCTP_MUTEX_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 排他制御クラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/09 23:00:42
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/def.hpp>
#include <windows.h>

namespace gctp {

	/** GameCatapult Mutexオブジェクト
	 *
	 * boost::mutexと決別
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
		// ロック取得
		void lock()
		{
			::EnterCriticalSection(&cs_);
		}
		// ロック解除
		void unlock()
		{
			::LeaveCriticalSection(&cs_);
		}
	private:
		CRITICAL_SECTION cs_;
	};

	/// スコープによる自動ロック解除
	class ScopedLock {
	public:
		ScopedLock(Mutex &mutex) : mutex_(mutex) { mutex_.lock(); }
		~ScopedLock() { mutex_.unlock(); }
	private:
		Mutex &mutex_;
	};

} // namespace gctp

#endif //_GCTP_MUTEX_HPP_