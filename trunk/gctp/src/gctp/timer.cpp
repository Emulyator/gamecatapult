/**@file
 * GameCatapult 時間計測クラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/03/21 6:42:54
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/timer.hpp>
#include <time.h>

namespace gctp {

	namespace {
		clock_t clock_start = 0;
		Timer::TickType tick_start = 0;
		bool coinitialized = false;
		bool initialized = false;
		class TimerPeriod {
		public:
			TimerPeriod() { timeBeginPeriod(1); }
			~TimerPeriod() { timeEndPeriod(1); }
		};
	}

	void Timer::coinitialize()
	{
		static TimerPeriod period;
		clock_start = clock();
		tick_start = tick();
		coinitialized = true;
	}

	/** 初期化
	 *
	 * チックカウントを秒数に換算するための時間計測を行う。
	 * システム開始時に一回やればよい。
	 * 
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 18:08:19
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void Timer::initialize()
	{
		if(coinitialized) coinitialized = false;
		else {
			coinitialize();
			::Sleep(500);
		}
		double ticks_per_sec = static_cast<double>(static_cast<Timer::TickType>((tick()-tick_start)*CLOCKS_PER_SEC/(clock()-clock_start)));
		if(initialized) ticks_per_sec_ = (ticks_per_sec_+ticks_per_sec)/2;
		else {
			ticks_per_sec_ = ticks_per_sec;
			initialized = true;
		}
	}

	double Timer::ticks_per_sec_ = 0;

} // namespace gctp
