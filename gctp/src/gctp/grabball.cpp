/** @file
 * GameCatapult マウスドラッグ操作クラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:10:29
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/grabball.hpp>
#include <gctp/graphic.hpp>

using namespace std;

namespace gctp {

	GrabBall::GrabBall()
		:	rot_(QuatC(true)), rot_delta_(QuatC(true)), down_(QuatC(true)),
			vec_down_(VectorC(0,0,0)), trs_(VectorC(0,0,0)), trs_delta_(VectorC(0,0,0)),
			width_(400), height_(400), radius_(0.9f), radius2_(0.9f),
			view_mat_(MatrixC(true)),
			on_drag_(false)
	{
	}

	void GrabBall::reset()
	{
#ifdef _MT
		ScopedLock al(monitor_);
#endif
		down_ = rot_ = rot_delta_ = QuatC(true);
		vec_down_ = trs_ = trs_delta_ = VectorC(0,0,0);
	}

	/**
	 * 画面の情報をセット
	 *
	 * 中でロックかけます。呼び出し側がロックする必要なし。
	 */
	void GrabBall::setWindow( int w, int h )
	{
#ifdef _MT
		ScopedLock al(monitor_);
#endif
		// Set ArcBall info
		width_  = w;
		height_ = h;
	}

	void GrabBall::setView(const Matrix &view)
	{
		view_mat_ = view.inverse();
	}

	Vector GrabBall::getVectorFromScreen(const Point2 &scr)
	{
		// Scale to screen
		Vector v = {(scr.x - width_/2)  / (radius_*width_/2), -(scr.y - height_/2) / (radius_*height_/2), 0.0f};
		float mag = v.x*v.x + v.y*v.y;
		
		if( mag > 1.0f ) {
			float scale = 1.0f/sqrtf(mag);
			v.x *= scale;
			v.y *= scale;
		}
		else v.z = sqrtf( 1.0f - mag );
		
		return view_mat_.inverse().transformVector(v);
	}

	void GrabBall::setRadius( float radius )
	{
		radius_ = radius;
	}

	void GrabBall::setTransRadius( float radius2 )
	{
		radius2_ = radius2;
	}

	/** 移動ドラッグのためのリセット
	 *
	 * 標準では中・右ドラッグ開始時の初期化処理
	 */
	void GrabBall::resetMove(const Point2 &mouse_pos)
	{
#ifdef _MT
		ScopedLock al(monitor_);
#endif
		old_mouse_pos_ = mouse_pos;
	}

	/** 平行移動ドラッグ開始
	 *
	 * 標準では左ドラッグの処理
	 */
	void GrabBall::move(const Point2 &mouse_pos, bool dolly)
	{
#ifdef _MT
		ScopedLock al(monitor_);
#endif
		// Normalize based on size of window and bounding sphere radius
		float delta_x = ( old_mouse_pos_.x-mouse_pos.x ) * radius2_ / width_;
		float delta_y = ( old_mouse_pos_.y-mouse_pos.y ) * radius2_ / height_;
		
		if( !dolly )
		{
			trs_delta_.x = -2*delta_x;
			trs_delta_.y =  2*delta_y;
			trs_delta_.z = 0.0f;
		}
		else  // wParam & MK_MBUTTON
		{
			trs_delta_.x = trs_delta_.y = 0.0f;
			trs_delta_.z = 5*delta_y;
		}
		trs_ += view_mat_.transformVector(trs_delta_);
		
		// Store mouse coordinate
		old_mouse_pos_ = mouse_pos;
	}

	/** 回転ドラッグ開始
	 *
	 * 標準では左ドラッグ開始時の初期化処理
	 */
	void GrabBall::startRot(const Point2 &mouse_pos)
	{
#ifdef _MT
		ScopedLock al(monitor_);
#endif
		on_drag_ = true;
		vec_down_ = getVectorFromScreen(mouse_pos);
		down_ = rot_;
	}

	/** 回転
	 *
	 * 標準では左マウス移動の処理
	 */
	void GrabBall::rot(const Point2 &mouse_pos)
	{
#ifdef _MT
		ScopedLock al(monitor_);
#endif
		if( on_drag_ ) {
			rot_ = Quat::smallestArc(vec_down_, getVectorFromScreen(mouse_pos))*down_;
		}
	}

	/** 回転ドラッグ終了
	 *
	 * 標準では左マウスボタンアップの処理
	 */
	void GrabBall::endRot()
	{
#ifdef _MT
		ScopedLock al(monitor_);
#endif
		on_drag_ = false;
	}

	/**
	 * マウスメッセージを送信
	 *
	 * 中でロックかけます。呼び出し側がロックする必要なし。
	 * @return true -- 変更があった  false -- 変更無し
	 */
	bool GrabBall::handleMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		if(uMsg&MK_CONTROL || uMsg&MK_SHIFT) return false;
		// Current mouse position
		Point2 mouse_pos = {LOWORD(lParam), HIWORD(lParam)};
		
		switch( uMsg )
		{
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
			// Store off the position of the cursor when the button is pressed
			resetMove(mouse_pos);
			return true;
			
		case WM_LBUTTONDOWN:
			// Start drag mode
			startRot(mouse_pos);
			return true;
			
		case WM_LBUTTONUP:
			// End drag mode
			endRot();
			return true;
			
		case WM_MOUSEMOVE:
			// Drag object
			if( MK_LBUTTON&wParam )
			{
				rot(mouse_pos);
				return true;
			}
			else if( (MK_RBUTTON&wParam) || (MK_MBUTTON&wParam) )
			{
				move(mouse_pos, wParam & MK_RBUTTON ? false : true);
				return true;
			}
		}
		
		return false;
	}

} // namespace gctp
