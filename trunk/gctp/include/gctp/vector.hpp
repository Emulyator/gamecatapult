#ifndef _GCTP_VECTOR_HPP_
#define _GCTP_VECTOR_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult ベクトルクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: vector.hpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include <iosfwd>               // for std::basic_ostream

#include <gctp/math/vector2d.hpp>
#include <gctp/math/vector3d.hpp>
#include <gctp/math/vector4d.hpp>

namespace gctp {

	typedef math::Vector2d<Real> Vector2;
	typedef math::Vector3d<Real> Vector;
	typedef math::Vector4d<Real> Vector4;
	typedef math::Vector2dC<Real> Vector2C;
	typedef math::Vector3dC<Real> VectorC;
	typedef math::Vector4dC<Real> Vector4C;
	typedef Vector2 Point2f;
	typedef Vector2C Point2fC;
	typedef Vector2 Size2f;
	typedef Vector2C Size2fC;
	typedef Vector Point3f;
	typedef VectorC Point3fC;
	typedef Vector Size3f;
	typedef VectorC Size3fC;

	/// ２点間の距離
	inline float distance2(const Point2f &lhs, const Point2f &rhs) { return fabsf((rhs-lhs).length2()); }
	/// ２点間の距離
	inline float distance(const Point2f &lhs, const Point2f &rhs) { return sqrtf(distance2(rhs, lhs)); }

	// @{
	/// ２点間の距離
	inline float distance2(const Point3f &lhs, const Point3f &rhs) { return fabsf((rhs-lhs).length2()); }
	inline float distance(const Point3f &lhs, const Point3f &rhs) { return sqrtf(distance2(rhs, lhs)); }
	// @}

} // namespace gctp

#endif //_GCTP_VECTOR_HPP_