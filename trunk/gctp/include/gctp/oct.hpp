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

	/** �����؂̐[�x����Z���̐ς����߂�
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 5:05:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline int octcube(int level)
	{
		return 1<<(3*level);
	}

	/** �����؂̊e�[�x�̃Z���ʐς𑫂����ފK������̒l
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 5:05:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline int octprog(int level)
	{
		int ret = 0;
		while(level>=0) ret += octcube(level--);
		return ret;
	}

	/** �����؂̂���[�x�ɂ����鏘������A�Z�����W������o��
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 5:05:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline Point3 octpos(int idx)
	{
		Point3 ret = {0, 0, 0};
		for(int i = 0; idx; i++) {
			if(idx&1) ret.x += 1<<i;
			if(idx&2) ret.y += 1<<i;
			if(idx&4) ret.z += 1<<i;
			idx>>=3;
		}
		return ret;
	}

	/** �����؂̃Z�����W����A���̐[�x�ł̏�����Ԃ�
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 5:05:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	inline int octidx(const Point3 &p)
	{
		int ret = 0, level = intlog2(std::max<int>(std::max<int>(p.x, p.y),p.z));
		for(int i = 0; i < level; i++) {
			int w = 0;
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
	inline int octlevel(int idx, int &localidx)
	{
		int ret = 0;
		while(idx >= octcube(ret)) idx -= octcube(ret++);
		localidx = idx;
		return ret;
	}

	/** �w�背�x���̔����؃Z���̐�
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 20:18:53
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<int _N> class OctCube {
	public:
		enum { value = (1<<(3*_N)) };
	};
	/** �w�背�x���̔����ؗݐϒ�
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/19 20:18:53
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<int _N> class OctProg {
	public:
		enum { value = OctCube<_N>::value + OctCube<_N-1>::value };
	};

	template<>
	class OctProg<0> {
	public:
		enum { value = 1 };
	};

} //namespace gctp

#endif //_GCTP_OCT_HPP_