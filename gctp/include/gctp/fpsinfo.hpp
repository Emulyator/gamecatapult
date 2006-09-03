#ifndef _GCFPSINFO_H_
#define _GCFPSINFO_H_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult FPS(Flip per second)計測クラスヘッダファイル
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

	/** FPS計測クラス
	 *
	 * 時間の単位は秒
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

		float FPS;			///< 瞬間のFPS
		float maxFPS;		///< 最大FPS
		float minFPS;		///< 最小FPS
		float totalave;		///< 計測開始からの平均
		float latestave;	///< (update_deltaで示される)一定の更新頻度による短期間FPS
		float update_delta;	///< latestaveの更新間隔
		
		/** 計測
		 * @return ラップタイム
		 */
		float update();
		
	private:
		Timer timer;
		float w_totalave; float w_latestave; float update_count;
		int cnt_totalave; int cnt_latestave;
	};

} //namespace gctp

#endif //_GCFPSINFO_H_