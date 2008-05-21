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
 level 0
| 1 |

 level 1
| 2 | 3 |
| 4 | 5 |

 level 2
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
#include <gctp/types.hpp>

namespace gctp {

	/** �C�ӂ̐����̂Q�̑ΐ����A�����ŕԂ�
	 *
	 * �܂�0�Ŗ����ł���ʂ̃r�b�g�����𒲂ׂĕԂ�
	 *
	 * 0�̎�-1���Ԃ�̂Œ���
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/18 22:38:12
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<typename _INT>
	_INT intlog2(_INT x)
	{
		// �����Ƃ������@�Ȃ����񂩁c
		if(x) {
			x>>=1;
			for(_INT i = 0; i < sizeof(_INT)*8; i++, x>>=1) if(!x) return i;
		}
		return -1;
	}

	/** �l���؂̂��̐[�x�ɂ����镝
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 5:05:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline unsigned int quadwidth(unsigned int level)
	{
		return 1<<level;
	}

	/** �l���؂̐[�x����Z���ʐς����߂�
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 5:05:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline unsigned int quadsq(unsigned int level)
	{
		return 1<<(level<<1);
	}

	/** �l���؂̊e�[�x�̃Z���ʐς𑫂����ޓ��䋉���̒l
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 5:05:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline unsigned int quadprog(unsigned int level)
	{
		return ((1<<(level<<1))-1)/3+1;
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
		unsigned int ret = 0, level = intlog2(((std::max)((std::max)(p.x, p.y), 0)))+1;
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

	/** ���x�����ƃ��x�����̏�������l���؏����ɕϊ�
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/24 11:55:36
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline unsigned int quadindex(unsigned int level, unsigned int localidx)
	{
		return quadprog(level)+localidx;
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

	/** ��̎l���؏����̐�
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
	inline int quadand(unsigned int lhs, unsigned int rhs)
	{
		if(lhs && rhs) {
			unsigned int lhsl, lhsli, rhsl, rhsli;
			lhsl = quadlevel(lhs, lhsli);
			rhsl = quadlevel(rhs, rhsli);
			if(lhsl <= rhsl) {
				rhsli >>= ((rhsl-lhsl)>>1);
				return lhsli == rhsli ? rhs : 0;
			}
			else {
				lhsli >>= ((lhsl-rhsl)>>1);
				return lhsli == rhsli ? lhs : 0;
			}
		}
		else if(lhs) return lhs;
		else if(rhs) return rhs;
		return 0;
	}

	/** ��̎l���؏����̘a
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
	inline int quador(unsigned int lhs, unsigned int rhs)
	{
		if(lhs && rhs) {
			unsigned int lhsl, lhsli, rhsl, rhsli;
			lhsl = quadlevel(lhs, lhsli);
			rhsl = quadlevel(rhs, rhsli);
			if(lhsl <= rhsl) {
				rhsli >>= ((rhsl-lhsl)>>1);
				if(lhsli == rhsli) return lhs;
				while(lhsl > 0) {
					lhsl--; lhsli>>=2; rhsli>>=2;
					if(lhsli == rhsli) return quadindex(lhsl, lhsli);
				}
			}
			else {
				lhsli >>= ((lhsl-rhsl)>>1);
				if(lhsli == rhsli) return rhs;
				while(rhsl > 0) {
					rhsl--; rhsli>>=2; lhsli>>=2;
					if(lhsli == rhsli) return quadindex(rhsl, rhsli);
				}
			}
			return 1;
		}
		return 0;
	}

} //namespace gctp

#endif //_GCTP_QUAD_HPP_