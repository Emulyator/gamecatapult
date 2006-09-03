#ifndef _GCTP_TIMER_HPP_
#define _GCTP_TIMER_HPP_
#include <gctp/config.hpp>
#include <gctp/def.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 時間計測クラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 18:09:11
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */

// Win32専用
#include <mmsystem.h>

namespace gctp {

	/** 低レベルカウンタークラス
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
		/// ストップウォッチをリセット
		inline void reset() { start_ = tick(); }
		/// ストップウォッチのラップボタン（その前のreset()との経過時間を秒で返す）
		inline float elapsed() { return static_cast<float>(static_cast<int64_t>(tick()-start_)/ticks_per_sec_); }

		/** チックカウントを返す
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/01/29 18:09:35
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		inline static TickType tick()
		{
			uint32_t _low, _high;
			__asm {
				rdtsc
				mov	_low, eax
				mov	_high, edx
			}
			return ((TickType)_high << 32) | (TickType)_low;
		}

		/// 1tickが何秒に相当するか？
		inline static double ticks_per_sec() { return ticks_per_sec_; }

		/// 起動からの経過時間をミリ秒単位で取得
		inline static ulong time() { return timeGetTime(); }

		/** Windows特有
		 *
		 * クロックカウントが何秒に相当するかを計測するための準備
		 */
		static void coinitialize();
		static void initialize(); ///< coinitialize呼出し後500ms以上経ってから呼び出す

	private:
		TickType start_;
		static double ticks_per_sec_;
	};

} //namespace gctp

#endif //_GCTP_TIMER_HPP_