#ifndef _GCTP_QUAD_HPP_
#define _GCTP_QUAD_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * 四分木を扱うためのユーティリティー関数
 *
 * 四文木序数は0を無効値として、
 * @pre
| 1 |

| 2 | 3 |
| 4 | 5 |

|  6 |  7 | 10 | 11 |
|  8 |  9 | 12 | 13 |
| 14 | 15 | 18 | 19 |
| 16 | 17 | 20 | 21 |
   :
 * @endpre
 * となるようなもの。
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 2:42:16
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <boost/config.hpp>
#include <gctp/types.hpp>

namespace gctp {

#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
	template<typename _INT, unsigned int BIT>
	class IntLog2 {
	public:
		static _INT value(_INT x)
		{
			return (x&(1<<(BIT-1)))?BIT:IntLog2<_INT, BIT-1>::value(x);
		}
	};

	template<typename _INT>
	class IntLog2<_INT, 1> {
	public:
		static _INT value(_INT x)
		{
			return (x&1)?1:0;
		}
	};

	/** 任意の整数の２の対数を、整数で返す
	 *
	 * 0で無い最も上位のビット桁数を調べて返す
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/18 22:38:12
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<typename _INT>
	_INT intlog2(_INT x)
	{
		return IntLog2<_INT, sizeof(_INT)*8>::value(x);
	}
#else
	typedef unsigned int IntLog2Type;
	
	template<int BIT>
	class IntLog2 {
	public:
		static IntLog2Type value(IntLog2Type x)
		{
			return (x&(1<<(BIT-1)))?BIT:IntLog2<BIT-1>::value(x);
		}
	};

	template<>
	class IntLog2<1> {
	public:
		static int value(IntLog2Type x)
		{
			return (x&1)?1:0;
		}
	};

	/** 任意の整数の２の対数を、整数で返す
	 *
	 * 0で無い最も上位のビット桁数を調べて返す
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/18 22:38:12
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<typename _Int>
	_Int intlog2(_Int x)
	{
		return IntLog2<sizeof(_Int)*8>::value(x);
	}
#endif

	/** 四分木の深度からセル面積を求める
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 5:05:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline unsigned int quadsq(unsigned int level)
	{
		return 1<<(2*level);
	}

	/** 四分木の各深度のセル面積を足しこむ階差数列の値
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 5:05:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline unsigned int quadprog(unsigned int level)
	{
		unsigned int ret = 1;
		while(level>0) ret += quadsq(--level);
		return ret;
	}

	/** 四分木のある深度における序数から、セル座標を割り出す
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 5:05:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline Point2 quadlocalpos(unsigned int localidx)
	{
		Point2 ret = {0, 0};
		for(unsigned int i = 0; localidx; i++) {
			if(localidx&1) ret.x += 1<<i;
			if(localidx&2) ret.y += 1<<i;
			localidx>>=2;
		}
		return ret;
	}

	/** 四分木のセル座標から、その深度での序数を返す
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 5:05:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline unsigned int quadlocalidx(const Point2 &p)
	{
		unsigned int ret = 0, level = intlog2((std::max)((std::max)(p.x, p.y), 0));
		for(unsigned int i = 0; i < level; i++) {
			unsigned int w = 0;
			if(p.x&(1<<i)) w |= 1;
			if(p.y&(1<<i)) w |= 2;
			ret |= w<<(2*i);
		}
		return ret;
	}

	/** 四分木を幅優先トラバースしたときの序数からレベル数とレベル内の序数に変換
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/24 11:55:36
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline unsigned int quadlevel(unsigned int idx, unsigned int &localidx)
	{
		if(idx > 0) {
			unsigned int ret = 0;
			unsigned int area = 1;
			idx--;
			while(idx > area) {
				idx -= area;
				area = quadsq(++ret);
			}
			if(idx == area) {
				ret++;
				localidx = 0;
			}
			else localidx = idx;
			return ret;
		}
		localidx = 0;
		return 0;
	}

	/** 指定レベルの四分木セル面積
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 20:18:53
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<int _LEVEL> class QuadSq {
	public:
		enum { value = (1<<(2*_LEVEL)) };
	};

	/** 指定レベルの四分木累積長
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 20:18:53
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<unsigned int _LEVEL> class QuadProg {
	public:
		enum { value = QuadSq<_LEVEL-1>::value + QuadProg<_LEVEL-2>::value };
	};

	template<>
	class QuadProg<1> {
	public:
		enum { value = 2 };
	};

	template<>
	class QuadProg<0> {
	public:
		enum { value = 1 };
	};

} //namespace gctp

#endif //_GCTP_QUAD_HPP_