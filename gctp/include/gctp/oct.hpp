#ifndef _GCTP_OCT_HPP_
#define _GCTP_OCT_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * 八分木を扱うためのユーティリティー関数
 *
 * テストしてないけど、多分こうでしょ
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 2:42:16
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/quad.hpp>

namespace gctp {

	/** 八分木の深度からセル体積を求める
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 5:05:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline unsigned int octcube(unsigned int level)
	{
		return 1<<(3*level);
	}

	/** 八分木の各深度のセル面積を足しこむ階差数列の値
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 5:05:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline unsigned int octprog(unsigned int level)
	{
		unsigned int ret = 1;
		while(level>0) ret += octcube(--level);
		return ret;
	}

	/** 八分木のある深度における序数から、セル座標を割り出す
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 5:05:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline Point3 octlocalpos(unsigned int localidx)
	{
		Point3 ret = {0, 0, 0};
		for(unsigned int i = 0; localidx; i++) {
			if(localidx&1) ret.x += 1<<i;
			if(localidx&2) ret.y += 1<<i;
			if(localidx&4) ret.z += 1<<i;
			localidx>>=3;
		}
		return ret;
	}

	/** 八分木のセル座標から、その深度での序数を返す
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 5:05:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline unsigned int octlocalidx(const Point3 &p)
	{
		unsigned int ret = 0, level = intlog2((std::max)((std::max)((std::max)(p.x, p.y),p.z), 0));
		for(unsigned int i = 0; i < level; i++) {
			unsigned int w = 0;
			if(p.x&(1<<i)) w |= 1;
			if(p.y&(1<<i)) w |= 2;
			if(p.z&(1<<i)) w |= 4;
			ret |= w<<(3*i);
		}
		return ret;
	}

	/** 八分木を幅優先トラバースしたときの序数からレベル数とレベル内の序数に変換
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/24 11:55:36
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline unsigned int octlevel(unsigned int idx, unsigned int &localidx)
	{
		if(idx > 0) {
			unsigned int ret = 0;
			unsigned int volume = 1;
			idx--;
			while(idx > volume) {
				idx -= volume;
				volume = octcube(++ret);
			}
			if(idx == volume) {
				ret++;
				localidx = 0;
			}
			else localidx = idx;
			return ret;
		}
		localidx = 0;
		return 0;
	}

	/** 指定レベルの八分木セル体積
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 20:18:53
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<unsigned int _LEVEL> class OctCube {
	public:
		enum { value = (1<<(3*_LEVEL)) };
	};
	/** 指定レベルの八分木累積長
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 20:18:53
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<unsigned int _LEVEL> class OctProg {
	public:
		enum { value = OctCube<_LEVEL-1>::value + OctProg<_LEVEL-2>::value };
	};

	template<>
	class OctProg<1> {
	public:
		enum { value = 2 };
	};

	template<>
	class OctProg<0> {
	public:
		enum { value = 1 };
	};

} //namespace gctp

#endif //_GCTP_OCT_HPP_