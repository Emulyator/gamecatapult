#ifndef _GCTP_LINE_HPP_
#define _GCTP_LINE_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 直線・線分・レイクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 2:42:16
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/vector.hpp>

namespace gctp {

	/// 直線クラス
	struct Line {
		Vector	s;	///< 始点
		Vector	v;	///< 方向ベクトル

		Line() {}
		Line(const Vector &_s, const Vector &_v) : s(_s), v(_v) {}
	};

	/// レイクラス
	struct RayLine {
		Vector	s;	///< 始点
		Vector	v;	///< 方向ベクトル

		RayLine() {}
		RayLine(const Vector &_s, const Vector &_v) : s(_s), v(_v) {}

		Line &toLine() { return *(Line *)this; }
		const Line &toLine() const { return *(Line *)this; }
	};

	/// 線分クラス
	struct LineSeg {
		Vector	s;	///< 始点
		Vector	e;	///< 終点

		LineSeg() {}
		LineSeg(const Vector &_s, const Vector &_e) : s(_s), e(_e) {}

		Line toLine() const { return Line(s, e-s); }
		Vector direction() const { return e - s; }
		float length2() const { return direction().length2(); }
		float length() const { return direction().length(); }
	};

	//@{
	/// 直線と点の距離
	float distance2(const Line &lhs, const Point3f &rhs, float *t = 0);
	inline float distance2(const Point3f &rhs, const Line &lhs, float *t = 0) { return distance2(lhs, rhs, t); }
	inline float distance(const Line &lhs, const Point3f &rhs, float *t = 0) { return sqrtf(distance2(lhs, rhs, t)); }
	inline float distance(const Point3f &rhs, const Line &lhs, float *t = 0) { return sqrtf(distance2(lhs, rhs, t)); }
	//@}

	//@{
	/// レイと点の距離
	float distance2(const RayLine &lhs, const Point3f &rhs, float *t);
	inline float distance2(const Point3f &rhs, const RayLine &lhs, float *t = 0) { return distance2(lhs, rhs, t); }
	inline float distance(const RayLine &lhs, const Point3f &rhs, float *t = 0) { return sqrtf(distance2(lhs, rhs, t)); }
	inline float distance(const Point3f &rhs, const RayLine &lhs, float *t = 0) { return sqrtf(distance2(lhs, rhs, t)); }
	//@}

	//@{
	/// 線分と点の距離
	float distance2(const LineSeg &lhs, const Point3f &rhs, float *t = 0);
	inline float distance2(const Point3f &rhs, const LineSeg &lhs, float *t = 0) { return distance2(lhs, rhs, t); }
	inline float distance(const LineSeg &lhs, const Point3f &rhs, float *t = 0) { return sqrtf(distance2(lhs, rhs, t)); }
	inline float distance(const Point3f &rhs, const LineSeg &lhs, float *t = 0) { return sqrtf(distance2(lhs, rhs, t)); }
	//@}

	//@{
	/// ２直線の距離を返す
	float distance2(const Line &lhs, const Line &rhs, float *t1=0, float *t2=0);
	inline float distance(const Line &lhs, const Line &rhs, float *t1=0, float *t2=0) { return sqrtf(distance2(lhs, rhs, t1, t2)); }
	//@}

	//@{
	/// 直線とレイの距離を返す
	float distance2(const Line &lhs, const RayLine &rhs, float *t1=0, float *t2=0);
	inline float distance2(const RayLine &lhs, const Line &rhs, float *t1=0, float *t2=0) { return distance2(rhs, lhs, t2, t1); }
	inline float distance(const Line &lhs, const RayLine &rhs, float *t1=0, float *t2=0) { return sqrtf(distance2(lhs, rhs, t1, t2)); }
	inline float distance(const RayLine &lhs, const Line &rhs, float *t1=0, float *t2=0) { return sqrtf(distance2(lhs, rhs, t1, t2)); }
	//@}

	//@{
	/// 直線と線分の距離を返す
	float distance2(const Line &lhs, const LineSeg &rhs, float *t1=0, float *t2=0);
	inline float distance2(const LineSeg &lhs, const Line &rhs, float *t1=0, float *t2=0) { return distance2(rhs, lhs, t2, t1); }
	inline float distance(const Line &lhs, const LineSeg &rhs, float *t1=0, float *t2=0) { return sqrtf(distance2(lhs, rhs, t1, t2)); }
	inline float distance(const LineSeg &lhs, const Line &rhs, float *t1=0, float *t2=0) { return sqrtf(distance2(lhs, rhs, t1, t2)); }
	//@}

	//@{
	/// ２つのレイの距離を返す
	float distance2(const RayLine &lhs, const RayLine &rhs, float *t1=0, float *t2=0);
	inline float distance(const RayLine &lhs, const RayLine &rhs, float *t1=0, float *t2=0) { return sqrtf(distance2(lhs, rhs, t1, t2)); }
	//@}

	//@{
	/// レイと線分の距離を返す
	float distance2(const RayLine &lhs, const LineSeg &rhs, float *t1=0, float *t2=0);
	inline float distance2(const LineSeg &lhs, const RayLine &rhs, float *t1=0, float *t2=0) { return distance2(rhs, lhs, t2, t1); }
	inline float distance(const RayLine &lhs, const LineSeg &rhs, float *t1=0, float *t2=0) { return sqrtf(distance2(lhs, rhs, t1, t2)); }
	inline float distance(const LineSeg &lhs, const RayLine &rhs, float *t1=0, float *t2=0) { return sqrtf(distance2(lhs, rhs, t1, t2)); }
	//@}

	//@{
	/// ２線分の距離を返す
	float distance2(const LineSeg &lhs, const LineSeg &rhs, float *t1=0, float *t2=0);
	inline float distance(const LineSeg &lhs, const LineSeg &rhs, float *t1=0, float *t2=0) { return sqrtf(distance2(lhs, rhs, t1, t2)); }
	//@}

} //namespace gctp

#endif //_GCTP_LINE_HPP_