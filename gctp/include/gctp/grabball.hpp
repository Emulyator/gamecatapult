#ifndef _GCTP_GRABBALL_HPP_
#define _GCTP_GRABBALL_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult マウスドラッグ操作クラスヘッダファイル
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

	/** マウスドラッグ操作クラス
	 *
	 * これ、どう使うんだ？
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
		/// Windowsメッセージの処理
		bool handleMessage( HWND, UINT, WPARAM, LPARAM );
		/// Windowsメッセージの処理(WTL対応)
		/** WTLのメッセージマップにCHAIN_MSG_MAP_MEMBERで追加できる。
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
		/// 回転時の仮想球の大きさ
		void setRadius( float rad );
		/// スクリーンの大きさ
		void setWindow( int w, int h );
		/// 移動時の仮想球の大きさ
		void setTransRadius( float rad );
		
		GrabBall();
		
		void resetMove(const Point2 &mouse_pos);
		void move(const Point2 &mouse_pos, bool dolly);
		void startRot(const Point2 &mouse_pos);
		void rot(const Point2 &mouse_pos);
		void endRot();

	private:
		int		width_;			// ウィンドウ幅
		int		height_;		// ウィンドウ高さ
		Point2  center_;        // 仮想球の中心（上で設定した仮想ウィンドウで）
		float	radius_;		// 回転操作での仮想球の大きさ
		float	radius2_;		// 平行移動操作での仮想球の大きさ
		
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
