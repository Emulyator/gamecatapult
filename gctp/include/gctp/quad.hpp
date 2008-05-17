#ifndef _GCTP_QUAD_HPP_
#define _GCTP_QUAD_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * �l���؂��������߂̃��[�e�B���e�B�[�֐�
 *
 * �l���؏�����0�𖳌��l�Ƃ��āA
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
 * �ƂȂ�悤�Ȃ��́B
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

	/** �C�ӂ̐����̂Q�̑ΐ����A�����ŕԂ�
	 *
	 * 0�Ŗ����ł���ʂ̃r�b�g�����𒲂ׂĕԂ�
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

	/** �C�ӂ̐����̂Q�̑ΐ����A�����ŕԂ�
	 *
	 * 0�Ŗ����ł���ʂ̃r�b�g�����𒲂ׂĕԂ�
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

	/** �l���؂̐[�x����Z���ʐς����߂�
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 5:05:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline unsigned int quadsq(unsigned int level)
	{
		return 1<<(2*level);
	}

	/** �l���؂̊e�[�x�̃Z���ʐς𑫂����ފK������̒l
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

	/** �l���؂̂���[�x�ɂ����鏘������A�Z�����W������o��
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

	/** �l���؂̃Z�����W����A���̐[�x�ł̏�����Ԃ�
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

	/** �l���؂𕝗D��g���o�[�X�����Ƃ��̏������烌�x�����ƃ��x�����̏����ɕϊ�
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

	/** �w�背�x���̎l���؃Z���ʐ�
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 20:18:53
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<int _LEVEL> class QuadSq {
	public:
		enum { value = (1<<(2*_LEVEL)) };
	};

	/** �w�背�x���̎l���ؗݐϒ�
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