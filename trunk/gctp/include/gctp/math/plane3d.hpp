#ifndef _GCTP_MATH_PLANE3D_HPP_
#define _GCTP_MATH_PLANE3D_HPP_
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
#include <gctp/math/vector3d.hpp>

#ifdef GCTP_USE_D3DXMATH
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <d3dx9.h>
#endif

namespace gctp { namespace math {

	/** 平面
	 *
	 * PODとなるよう、コンストラクタが無い
	 *
	 * この手の型には、PODになるようコンストラクタが無い方がいいと思う。
	 * かわりにsetメソッドを用意する。
	 */
	template<typename _Type>
	struct Plane3d {
		_Type a, b, c, d;
		
		Vector3d<_Type> &vector() { return *reinterpret_cast<Vector3d<_Type> *>(&a); }
		const Vector3d<_Type> &vector() const { return *reinterpret_cast<Vector3d<_Type> *>(&a); }

		Plane3d &set(const _Type *pf)
		{
			a = pf[0]; b = pf[1]; c = pf[2]; d = pf[3];
		}

		Plane3d &set(_Type a, _Type b, _Type c, _Type d)
		{
			this->a = a; this->b = b; this->c = c; this->d = d;
		}
		
		Plane3d &set(const Vector3d<_Type> &pos, const Vector3d<_Type> &vec)
		{
			vector() = vec;
			d = -pos*vec;
			return *this;
		}
		
		Plane3d &set(const Vector3d<_Type> &p1, const Vector3d<_Type> &p2, const Vector3d<_Type> &p3)
		{
			Vector3d<_Type> edge1 = p2 - p1;
			Vector3d<_Type> edge2 = p3 - p1;
			vector() = (edge1 % edge2).normal();
			d = -vector() * p1;
			return *this;
		}

		Plane3d &normalize()
		{
			Real invlen = _Type(1)/vector().length();
			vector() *= invlen;
			d *= invlen;
			return *this;
		}

		Plane3d normal() const
		{
			return Plane3d(*this).normalize();
		}

		_Type operator*(const Vector3d<_Type> &rhs) const
		{
			return a*rhs.x+b*rhs.y+c*rhs.z+d;
		}

		friend _Type operator*(const Vector3d<_Type> &lhs, const Plane3d &rhs)
		{
			return rhs*lhs;
		}

		// D3DXライブラリサポート
		#ifdef GCTP_USE_D3DXMATH
		operator const D3DXPLANE &() const
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return *reinterpret_cast<const D3DXPLANE *>(this);
		}
		operator D3DXPLANE &()
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return *reinterpret_cast<D3DXPLANE *>(this);
		}
		operator const D3DXPLANE *() const
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return reinterpret_cast<const D3DXPLANE *>(this);
		}
		operator D3DXPLANE *()
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return reinterpret_cast<D3DXPLANE *>(this);
		}
		#endif
	};
	
	/// 点と平面の距離
	template<typename _Type>
	inline Real distance(const Plane3d<_Type> &pl, const Vector3d<_Type> &p, Vector3d<_Type> *closest = 0)
	{
		_Type dot = pl * p;
		if(closest) *closest = p - dot*pl.vector();
		return ((Real)abs(dot))/pl.vector().length();
	}

	template<typename _Type>
	struct Plane3dC : Plane3d<_Type> {
		Plane3dC() {}
		explicit Plane3dC(const _Type *pf) { set(pf[0], pf[1], pf[2], pf[3]); }
		Plane3dC(_Type a, _Type b, _Type c, _Type d) { set(a, b, c, d); }
		Plane3dC(const Vector3d<_Type> &pos, const Vector3d<_Type> &vec) { set(pos, vec); }
		Plane3dC(const Vector3d<_Type> &p1, const Vector3d<_Type> &p2, const Vector3d<_Type> &p3) { set(p1, p2, p3); }
		// D3DXライブラリサポート
		#ifdef GCTP_USE_D3DXMATH
		Plane3dC(const D3DXPLANE &src) { set(src.a, src.b, src.c, src.d); }
		#endif
	};

}} // namespace gctp

// D3DXライブラリサポート
#ifdef GCTP_USE_D3DXMATH
#include <gctp/math/d3d/plane3d.inl>
#endif

#endif //_GCTP_MATH_PLANE3D_HPP_