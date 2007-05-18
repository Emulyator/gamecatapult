#ifndef _GCTP_GRABBALL_HPP_
#define _GCTP_GRABBALL_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult �}�E�X�h���b�O����N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.) <sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2001/8/6
 * Copyright (C) 2001 SAM (T&GG, Org.). All rights reserved.
 */
#ifdef _MT
#include <gctp/mutex.hpp>
#endif
#include <float.h>
#include <gctp/types.hpp>
#include <gctp/matrix.hpp>
#include <gctp/vector.hpp>
#include <gctp/quat.hpp>

namespace gctp {

	/** �}�E�X�h���b�O����N���X
	 *
	 * ����A�ǂ��g���񂾁H
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/18 21:31:22
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class GrabBall
	{
#ifdef _MT
		Mutex	monitor_;
#endif
	public:
		/// Windows���b�Z�[�W�̏���
		bool handleMessage( HWND, UINT, WPARAM, LPARAM );
		/// Windows���b�Z�[�W�̏���(WTL�Ή�)
		/** WTL�̃��b�Z�[�W�}�b�v��CHAIN_MSG_MAP_MEMBER�Œǉ��ł���B
		 */
		bool ProcessWindowMessage( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT& lresult )
		{
			lresult = 0;
			return handleMessage(hwnd, msg, wparam, lparam);
		}
		
		const Quat &rot() {
#ifdef _MT
			ScopedLock al(monitor_);
#endif
			return rot_;
		}
		void setRot(const Quat &src) {
#ifdef _MT
			ScopedLock al(monitor_);
#endif
			rot_ = src;
		}
		const Quat &rotDelta() {
#ifdef _MT
			ScopedLock al(monitor_);
#endif
			return rot_delta_;
		}
		const Vector &trans() {
#ifdef _MT
			ScopedLock al(monitor_);
#endif
			return trs_;
		}
		void setTrans(const Vector &src) {
#ifdef _MT
			ScopedLock al(monitor_);
#endif
			trs_ = src;
		}
		const Vector &transDelta() {
#ifdef _MT
			ScopedLock al(monitor_);
#endif
			return trs_delta_;
		}
		bool isBeingDragged() {
#ifdef _MT
			ScopedLock al(monitor_);
#endif
			return on_drag_;
		}
		void reset();
		
		void setView(const Matrix &view);
		/// ��]���̉��z���̑傫��
		void setRadius( float rad );
		/// �X�N���[���̑傫��
		void setWindow( int w, int h );
		/// �ړ����̉��z���̑傫��
		void setTransRadius( float rad );
		
		GrabBall();
		
		void resetMove(const Point2 &mouse_pos);
		void move(const Point2 &mouse_pos, bool dolly);
		void startRot(const Point2 &mouse_pos);
		void rot(const Point2 &mouse_pos);
		void endRot();

	private:
		int		width_;			// �E�B���h�E��
		int		height_;		// �E�B���h�E����
		Point2  center_;        // ���z���̒��S�i��Őݒ肵�����z�E�B���h�E�Łj
		float	radius_;		// ��]����ł̉��z���̑傫��
		float	radius2_;		// ���s�ړ�����ł̉��z���̑傫��
		
		Quat	rot_;
		Quat	rot_delta_;
		Vector  trs_;
		Vector  trs_delta_;

		bool	on_drag_;		// Whether user is dragging arcball
		Vector  vec_down_;
		Quat	down_;			// Quat before button down
		Vector getVectorFromScreen(const Point2 &scr);

		Matrix view_mat_;
		Point2  old_mouse_pos_;
	};

} //namespace gctp

#endif //_GCTP_GRABBALL_HPP_
