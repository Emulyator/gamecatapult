/** @file
 * GameCatapult FPS(Flips Per Second)計測クラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:10:18
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/fpsinfo.hpp>

namespace gctp {

	/** 計測
	 *
	 * @return 有効が値が得られるか？
	 */
	float FPSInfo::update()
	{
		float lap = timer.elapsed();
		timer.reset();
		if(lap>0.0f) {
			FPS = 1.0f/lap;
			if(FPS > maxFPS) maxFPS = FPS;
			if(FPS < minFPS) minFPS = FPS;
			w_totalave += FPS; cnt_totalave++;
			totalave = w_totalave / cnt_totalave;
			w_latestave += FPS; cnt_latestave++;
			update_count += lap;
			if(update_count > update_delta) {
				latestave = w_latestave / cnt_latestave;
				w_latestave = 0.0f;	cnt_latestave = 0;
				update_count -= update_delta;
				if(update_count > update_delta) update_count = 0.0f;
			}
		}
		return lap;
	}

} // namespace gctp
