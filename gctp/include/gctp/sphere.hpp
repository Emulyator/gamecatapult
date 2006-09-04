#ifndef _GCTP_SPHERE_HPP_
#define _GCTP_SPHERE_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 球クラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 2:42:16
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/line.hpp>

namespace gctp {

	/// 球クラス
	struct Sphere {
		Vector	c; ///< 中心点
		float	r; ///< 半径
		
		/// コンストラクタ
		Sphere() {}
		/// コンストラクタ
		Sphere(const Vector &_c, const float _r) : c(_c), r(_r) {}
		
		/// 自分とrhsを包括する球に
		Sphere &operator |= (const Sphere &rhs)
		{
			Vector diff = c - rhs.c;
			float d = diff.length();
			if(d+rhs.r > r) {
				diff.normalize();
				r = (std::max)(r, rhs.r) + d/2;
				Vector p1 = c+diff*r, p2 = rhs.c-diff*rhs.r;
				r = (std::max)(r, distance(p1, p2));
				c = (p1+p2)/2;
			}
			return *this;
		}

		/// 自分とrhsを包括する球を返す
		Sphere operator | (const Sphere &rhs) const
		{
			return Sphere(*this)|=rhs;
		}

		bool isColliding(const Sphere &with, Vector &inc) const
		{
			inc = Vector(with.c - c);
			return (inc.length() <= r + with.r);
		}
		bool isColliding(const Sphere &with) const
		{
			return (distance(c, with.c) <= r + with.r);
		}
	};

	/// レイと球の交差テスト
	inline bool intersection(const RayLine &ray, const Sphere &sphere)
	{
	    float dist2 = distance2(sphere.c, ray);
		return dist2 <= sphere.r*sphere.r;
	}

	/// レイと球の交差
	inline bool intersection(const RayLine &ray, const Sphere &sphere, int &quantity, Vector nearest[2])
	{
		// set up quadratic Q(t) = a*t^2 + 2*b*t + c
		Vector diff = ray.s - sphere.c;
		float A = ray.v.length2();
		float B = diff*ray.v;
		float C = diff.length2()-sphere.r*sphere.r;

		float T[2];
		float discr = B*B - A*C;
		if( discr < 0.0f ) {
			quantity = 0;
			return false;
		}
		else if( discr > 0.0f ) {
			float root = sqrtf(discr);
			float invA = 1.0f/A;
			T[0] = (-B - root)*invA;
			T[1] = (-B + root)*invA;

			if( T[0] >= 0.0f ) {
				quantity = 2;
				nearest[0] = ray.s + T[0]*ray.v;
				nearest[1] = ray.s + T[1]*ray.v;
				return true;
			}
			else if( T[1] >= 0.0f ) {
				quantity = 1;
				nearest[0] = ray.s + T[1]*ray.v;
				return true;
			}
			else {
				quantity = 0;
				return false;
			}
		}
		else {
			T[0] = -B/A;
			if( T[0] >= 0.0f ) {
				quantity = 1;
				nearest[0] = ray.s + T[0]*ray.v;
				return true;
			}
			else {
				quantity = 0;
				return false;
			}
		}
	}
} //namespace gctp

#endif //_GCTP_SPHERE_HPP_