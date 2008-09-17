/**@file
 * GameCatapult 時間計測クラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/03/21 6:42:54
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/timer.hpp>
#include <gctp/dbgout.hpp>
#include <time.h>

#ifdef GCTP_USE_QueryPerformanceCounter

namespace gctp {

	namespace {
		clock_t clock_start = 0;
		Timer::TickType tick_start = {0};
		bool coinitialized = false;
		bool initialized = false;
		class TimerPeriod {
		public:
			TimerPeriod(UINT period) : period_(period) { timeBeginPeriod(period_); }
			~TimerPeriod() { timeEndPeriod(period_); }
			UINT period_;
		};
	}

	void Timer::coinitialize()
	{
		static TimerPeriod period(1);
		QueryPerformanceFrequency(&frequency_);
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
		initialized = true;
	}

	Timer::TickType Timer::frequency_ = {0};

} // namespace gctp

#else

namespace gctp {

	namespace {
		clock_t clock_start = 0;
		Timer::TickType tick_start = 0;
		bool coinitialized = false;
		bool initialized = false;
		class TimerPeriod {
		public:
			TimerPeriod(UINT period) : period_(period) { timeBeginPeriod(period_); }
			~TimerPeriod() { timeEndPeriod(period_); }
			UINT period_;
		};
	}

	void Timer::coinitialize()
	{
		static TimerPeriod period(1);
		tick_start = tick();
		clock_start = clock();
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
		Timer::TickType _tick = tick();
		clock_t _clock = clock();
		double ticks_per_sec = static_cast<double>((_tick-tick_start)*CLOCKS_PER_SEC)/static_cast<double>(_clock-clock_start);
		if(initialized) ticks_per_sec_ = (ticks_per_sec_+ticks_per_sec)/2;
		else {
			ticks_per_sec_ = ticks_per_sec;
			dbgout << "ticks_per_sec : " << ticks_per_sec_ << std::endl;
			initialized = true;
		}
	}

	double Timer::ticks_per_sec_ = 0;

} // namespace gctp

#endif
