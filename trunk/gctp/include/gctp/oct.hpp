#ifndef _GCTP_OCT_HPP_
#define _GCTP_OCT_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * �����؂��������߂̃��[�e�B���e�B�[�֐�
 *
 * �e�X�g���ĂȂ����ǁA���������ł���
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 2:42:16
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/quad.hpp>

namespace gctp {

	/** �����؂̂��̐[�x�ɂ����镝
	 *
	 * quadwidth�Ƃ܂�������������
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 5:05:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline unsigned int octwidth(unsigned int level)
	{
		return 1<<level;
	}

	/** �����؂̐[�x����Z���̐ς����߂�
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 5:05:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline unsigned int octcube(unsigned int level)
	{
		return 1<<(3*level);
	}

	/** �����؂̊e�[�x�̃Z���ʐς𑫂����ޓ��䋉���̒l
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 5:05:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline unsigned int octprog(unsigned int level)
	{
		return ((1<<(level*3))-1)/7+1;
	}

	/** �����؂̂���[�x�ɂ����鏘������A�Z�����W������o��
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

	/** �����؂̃Z�����W����A���̐[�x�ł̏�����Ԃ�
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 5:05:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline unsigned int octlocalidx(const Point3 &p)
	{
		unsigned int ret = 0, level = intlog2((std::max)((std::max)((std::max)(p.x, p.y),p.z), 0))+1;
		for(unsigned int i = 0; i < level; i++) {
			unsigned int w = 0;
			if(p.x&(1<<i)) w |= 1;
			if(p.y&(1<<i)) w |= 2;
			if(p.z&(1<<i)) w |= 4;
			ret |= w<<(3*i);
		}
		return ret;
	}

	/** �����؂𕝗D��g���o�[�X�����Ƃ��̏������烌�x�����ƃ��x�����̏����ɕϊ�
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

	/** ���x�����ƃ��x�����̏������甪���؏����ɕϊ�
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/24 11:55:36
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline unsigned int octindex(unsigned int level, unsigned int localidx)
	{
		return octprog(level)+localidx;
	}

	/** �w�背�x���̔����؃Z���̐�
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 20:18:53
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<unsigned int _LEVEL> class OctCube {
	public:
		enum { value = (1<<(3*_LEVEL)) };
	};
	/** �w�背�x���̔����ؗݐϒ�
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

	/** ��̔����؏����̐�
	 *
	 * �d�Ȃ��Ă����ꍇ�͂ǂ��炩�̈�̏������ق��i�����̑傫���ق��j
	 *
	 * �d�Ȃ��Ă��Ȃ����0���Ԃ�B
	 *
	 * �ǂ��炩�������l�ł���΁A�L���Ȃق����Ԃ�B
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 20:18:53
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline int octand(unsigned int lhs, unsigned int rhs)
	{
		if(lhs && rhs) {
			unsigned int lhsl, lhsli, rhsl, rhsli;
			lhsl = octlevel(lhs, lhsli);
			rhsl = octlevel(rhs, rhsli);
			if(lhsl <= rhsl) {
				rhsli >>= (rhsl-lhsl)*3;
				return lhsli == rhsli ? rhs : 0;
			}
			else {
				lhsli >>= (lhsl-rhsl)*3;
				return lhsli == rhsli ? lhs : 0;
			}
		}
		else if(lhs) return lhs;
		else if(rhs) return rhs;
		return 0;
	}

	/** ��̔����؏����̘a
	 *
	 * �d�Ȃ��Ă����ꍇ�͂ǂ��炩�̈�̑傫���ق��i�����̏������ق��j
	 *
	 * �d�Ȃ��Ă��Ȃ���Γ�����鏘����Ԃ��B
	 *
	 * �ǂ��炩�������l�ł���΁A�����l���Ԃ�B
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 20:18:53
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline int octor(unsigned int lhs, unsigned int rhs)
	{
		if(lhs && rhs) {
			unsigned int lhsl, lhsli, rhsl, rhsli;
			lhsl = octlevel(lhs, lhsli);
			rhsl = octlevel(rhs, rhsli);
			if(lhsl <= rhsl) {
				rhsli >>= (rhsl-lhsl)*3;
				if(lhsli == rhsli) return lhs;
				while(lhsl > 0) {
					lhsl--; lhsli>>=3; rhsli>>=3;
					if(lhsli == rhsli) return octindex(lhsl, lhsli);
				}
			}
			else {
				lhsli >>= (lhsl-rhsl)*3;
				if(lhsli == rhsli) return rhs;
				while(rhsl > 0) {
					rhsl--; rhsli>>=3; lhsli>>=3;
					if(lhsli == rhsli) return octindex(rhsl, rhsli);
				}
			}
			return 1;
		}
		return 0;
	}

} //namespace gctp

#endif //_GCTP_OCT_HPP_