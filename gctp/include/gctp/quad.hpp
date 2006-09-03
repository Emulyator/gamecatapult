#ifndef _GCTP_QUAD_HPP_
#define _GCTP_QUAD_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * �l���؂��������߂̃��[�e�B���e�B�[�֐�
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 2:42:16
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <boost/config.hpp>
#include <gctp/types.hpp>

namespace gctp {

#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
	template<typename _INT, int BIT>
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
	typedef int IntLog2Type;
	
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
	inline int quadsq(int level)
	{
		return 1<<(2*level);
	}

	/** �l���؂̊e�[�x�̃Z���ʐς𑫂����ފK������̒l
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 5:05:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline int quadprog(int level)
	{
		int ret = 0;
		while(level>=0) ret += quadsq(level--);
		return ret;
	}

	/** �l���؂̂���[�x�ɂ����鏘������A�Z�����W������o��
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 5:05:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline Point2 quadpos(int idx)
	{
		Point2 ret = {0, 0};
		for(int i = 0; idx; i++) {
			if(idx&1) ret.x += 1<<i; 
			if(idx&2) ret.y += 1<<i;
			idx>>=2;
		}
		return ret;
	}

	/** �l���؂̃Z�����W����A���̐[�x�ł̏�����Ԃ�
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 5:05:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline int quadidx(const Point2 &p)
	{
		int ret = 0, level = intlog2(std::max<int>(p.x, p.y));
		for(int i = 0; i < level; i++) {
			int w = 0;
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
	inline int quadlevel(int idx, int &localidx)
	{
		int ret = 0;
		while(idx >= quadsq(ret)) idx -= quadsq(ret++);
		localidx = idx;
		return ret;
	}

	/** �w�背�x���̎l���؃Z���ʐ�
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 20:18:53
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<int _N> class QuadSq {
	public:
		enum { value = (1<<(2*_N)) };
	};
	/** �w�背�x���̎l���ؗݐϒ�
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 20:18:53
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<int _N> class QuadProg {
	public:
		enum { value = QuadSq<_N>::value + QuadProg<_N-1>::value };
	};

	template<>
	class QuadProg<0> {
	public:
		enum { value = 1 };
	};

} //namespace gctp

#endif //_GCTP_QUAD_HPP_