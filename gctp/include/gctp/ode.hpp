#ifndef _GCTP_ODE_HPP_
#define _GCTP_ODE_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult �����Q�E�N�b�^�@�ɂ��A������e���v���[�g
 *
	�Ƃ肠�����d�͂Ƌ�C��R���T�|�[�g����Vector�p���֐��̗�
	struct StandardDerivative {
		gctp::Vector gravity;
		float resist;
		float mass;
		gctp::Vector operator()(const gctp::Vector &src, const gctp::Vector &dsrc, float t)
		{
			return dsrc+gravity*t-resist*dsrc/mass*t;
		}
	};
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 2:42:16
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
//#include <gctp/vector.hpp> // ���x�E�ʒu
//#include <gctp/quat.hpp>   // �p���x�Ɗp�����x�A�e���\���Ȃ�
//#include <gctp/matrix.hpp> // �e���\��

namespace gctp {

	/** Verlet�ϕ��@�e���v���[�g
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/09/07 15:54:08
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T, class _Derivative>
	void calcVerlet(_T &dst, _T &ddst, const _T &src, const _T &dsrc, const _T &prev, _Derivative derivative, float t)
	{
		ddst = derivative(src+dsrc*t, dsrc, t);
		dst = 2*(src + ddst*t) - prev;
	}
	
	/** Verlet�ϕ��@�e���v���[�g
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/09/07 15:54:08
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T, class _Derivative>
	void calcVerlet(_T &dst, _T &ddst, const _T &prev, _Derivative derivative, float t)
	{
		_T ret, dret;
		calcVerlet(ret, dret, dst, ddst, prev, derivative, t);
		dst = ret; ddst = dret;
	}
	
	/** �Q�������Q�N�b�^�@�e���v���[�g
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/09/07 15:54:08
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T, class _Derivative>
	void calcODE(_T &dst, _T &ddst, const _T &src, const _T &dsrc, _Derivative derivative, float t)
	{
		ddst = derivative(src+dsrc*t, dsrc, t);
		dst = src + (dsrc+ddst)/2*t;
	}
	
	/** �Q�������Q�N�b�^�@�e���v���[�g
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/09/07 15:54:08
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T, class _Derivative>
	void calcODE(_T &dst, _T &ddst, _Derivative derivative, float t)
	{
		_T ret, dret;
		calcODE(ret, dret, dst, ddst, derivative, t);
		dst = ret; ddst = dret;
	}
	
	/** �K�����ݕ��Q�������Q�N�b�^�@�e���v���[�g
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/09/07 15:54:08
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T, class _Derivative>
	void calcAdaptiveODE(_T &dst, _T &ddst, const _T &src, const _T &dsrc, _Derivative derivative, float t, float eps = 0.001f)
	{
		_T w, wd;
		calcODE(w, wd, src, dsrc, derivative, t);
		for(int div = 2; div < 32; div*=2) {
			dst = src; ddst = dsrc;
			float wt = t/(float)div;
			for(int i = 0; i < div; i++) {
				calcODE(dst, ddst, derivative, wt);
			}
			if((dst-w).length() <= eps) break;
			w = dst; wd = ddst;
		}
	}

	/** �K�����ݕ��Q�������Q�N�b�^�@�e���v���[�g
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/09/07 15:54:08
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T, class _Derivative>
	void calcAdaptiveODE(_T &dst, _T &ddst, _Derivative derivative, float t, float eps = 0.001f)
	{
		_T ret, dret;
		calcAdaptiveODE(ret, dret, dst, ddst, derivative, t, eps);
		dst = ret; ddst = dret;
	}
	
	/** �S�������Q�N�b�^�@�e���v���[�g
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/09/07 15:54:08
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T, class _Derivative>
	void calcODE4(_T &dst, _T &ddst, const _T &src, const _T &dsrc, _Derivative derivative, float t)
	{
		_T k2 = derivative(src+dsrc*(t/2), dsrc, t/2);
		_T k3 = derivative(src+k2*(t/2), dsrc, t/2); // src���l�����Ȃ����֐����Ɩ��ʂ������Ȃ�
		ddst = derivative(src+k3*t, dsrc, t);
		dst = src + (dsrc+k2*2+k3*2+ddst)/6*t;
	}
	
	/** �S�������Q�N�b�^�@�e���v���[�g
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/09/07 15:54:08
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T, class _Derivative>
	void calcODE4(_T &dst, _T &ddst, _Derivative derivative, float t)
	{
		_T ret, dret;
		calcODE4(ret, dret, dst, ddst, derivative, t);
		dst = ret; ddst = dret;
	}
	
	/** �K�����ݕ��S�������Q�N�b�^�@�e���v���[�g
	 *
	 * float�̐��x�ł͊ȒP�ɖ������[�v�ɗ����邩��
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/09/07 15:54:08
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T, class _Derivative>
	void calcAdaptiveODE4(_T &dst, _T &ddst, const _T &src, const _T &dsrc, _Derivative derivative, float t, float eps = 0.001f)
	{
		T w, wd;
		calcODE4(w, wd, src, dsrc, derivative, t);
		for(int div = 2; div < 32; div*=2) {
			dst = src; ddst = dsrc;
			float wt = t/(float)div;
			for(int i = 0; i < div; i++) {
				calcODE4(dst, ddst, derivative, wt);
			}
			if((dst-w).length() <= eps) break;
			w = dst; wd = ddst;
		}
	}

	/** �K�����ݕ��S�������Q�N�b�^�@�e���v���[�g
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/09/07 15:54:08
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T, class _Derivative>
	void calcAdaptiveODE4(_T &dst, _T &ddst, _Derivative derivative, float t, float eps = 0.001f)
	{
		_T ret, dret;
		calcAdaptiveODE4(ret, dret, dst, ddst, derivative, t, eps);
		dst = ret; ddst = dret;
	}
	
} //namespace gctp

#endif //_GCTP_ODE_HPP_
