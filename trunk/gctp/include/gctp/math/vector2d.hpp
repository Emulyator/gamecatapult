#ifndef _GCTP_MATH_VECTOR2D_HPP_
#define _GCTP_MATH_VECTOR2D_HPP_
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
#include <gctp/def.hpp>

#ifdef GCTP_USE_D3DXMATH
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <d3dx9.h>
#endif

namespace gctp { namespace math {

	/** ２次元ベクトル
	 *
	 * PODとなるよう、コンストラクタが無い
	 *
	 * この手の型には、PODになるようコンストラクタが無い方がいいと思う。
	 * かわりにsetメソッドを用意する。
	 */
	template<typename _Type>
	struct Vector2d {
		_Type x, y;

		/// 値セット
		Vector2d &set(const _Type *p)
		{
			x = p[0]; y = p[1];
			return *this;
		}
		/// 値セット
		Vector2d &set(_Type x, _Type y)
		{
			this->x = x; this->y = y;
			return *this;
		}
		/// 値セット
		template<typename _Type2>
		Vector2d &set(const Vector2d<_Type2> &src)
		{
			x = _Type(src.x); y = _Type(src.y);
			return *this;
		}

		/// 代入
		Vector2d &operator+=(const Vector2d &src) {
			x += src.x; y += src.y;
			return *this;
		}
		/// 代入
		Vector2d &operator-=(const Vector2d &src) {
			x -= src.x; y -= src.y;
			return *this;
		}
		/// 代入
		Vector2d &operator*=(const Vector2d &src) {
			x *= src.x; y *= src.y;
			return *this;
		}
		/// 代入
		Vector2d &operator/=(const Vector2d &src) {
			x /= src.x; y /= src.y;
			return *this;
		}
		/// 代入
		Vector2d &operator*=(Real src) {
			x = _Type(x*src); y = _Type(y*src);
			return *this;
		}
		/// 代入
		Vector2d &operator/=(Real src) {
			x = _Type(x/src); y = _Type(y/src);
			return *this;
		}
		/// 代入
		Vector2d &operator|=(const Vector2d &src) {
			if(x < src.x) x = src.x;
			if(y < src.y) y = src.y;
			return *this;
		}
		/// 代入
		Vector2d &operator&=(const Vector2d &src) {
			if(x > src.x) x = src.x;
			if(y > src.y) y = src.y;
			return *this;
		}
		
		/// 演算子
		Vector2d operator + () const { return *this; }
		/// 演算子
		Vector2d operator - () const { return Vector2d(-x, -y); }
		/// 演算子
		Vector2d operator+(const Vector2d &rhs) const { return Vector2d(*this)+=rhs; }
		/// 演算子
		Vector2d operator-(const Vector2d &rhs) const { return Vector2d(*this)-=rhs; }
		/// 演算子
		Vector2d operator/(const Vector2d &rhs) const { return Vector2d(*this)/=rhs; }
		/// 内積演算子
		_Type operator*(const Vector2d &rhs) const { return x*rhs.x+y*rhs.y; }
		/// 内積
		friend _Type dot(const Vector2d &lhs, const Vector2d &rhs) {
			return lhs * rhs;
		}
		/// 外積演算子(反時計回り)
		_Type operator%(const Vector2d &rhs) const { return x*rhs.y-y*rhs.x; }
		/// 外積(反時計回り)
		friend Vector2d cross(const Vector2d &lhs, const Vector2d &rhs) {
			return lhs % rhs;
		}
		/// スケール演算子
		Vector2d operator*(Real rhs) const { return Vector2d(*this)*=rhs; }
		/// スケール演算子
		friend Vector2d operator*(Real lhs, const Vector2d &rhs) { return Vector2d(rhs)*=lhs; }
		/// スケール演算子
		Vector2d operator/(Real rhs) const { return Vector2d(*this)/=rhs; }
		/// スケール演算子
		friend Vector2d operator/(Real lhs, const Vector2d &rhs) { return Vector2d(rhs)/=lhs; }
		/// 最大化演算子
		Vector2d operator|(const Vector2d &rhs) const { return Vector2d(*this)|=rhs; }
		/// 最小化演算子
		Vector2d operator&(const Vector2d &rhs) const { return Vector2d(*this)&=rhs; }

		// 比較演算子
		bool operator==(const Vector2d &rhs)
		{
			return (x == rhs.x && y == rhs.y);
		}
		bool operator!=(const Vector2d &rhs)
		{
			return (x != rhs.x || y != rhs.y);
		}

		/// 正規化
		Vector2d &normalize() {
			_Type l = length(); x /= l; y /= l;
			return *this;
		}
		/// 正規化されたベクトルを返す
		Vector2d normal() const {
			return Vector2d(*this).normalize();
		}

		/// ベクトルの長さ
		Real length() const {
			return sqrt((Real)length2());
		}
		/// ベクトルの長さの二乗
		_Type length2() const {
			return (*this)*(*this);
		}

		/// 線形補間した値をセット
		Vector2d &setLerp(const Vector2d& lhs, const Vector2d& rhs, _Type t) {
			*this = lhs + (rhs-lhs)*t;
			return *this;
		}

		/// エルミート補間
		Vector2d &setHermite(const Vector2d &ps, const Vector2d &ts, const Vector2d &pe, const Vector2d &te, _Type s) {
			BOOST_STATIC_ASSERT(0); // This is not implemented.
			return *this;
		}

		operator const _Type *() const
		{
			return &x;
		}

		operator _Type *()
		{
			return &x;
		}

		// D3DXライブラリサポート
		#ifdef GCTP_USE_D3DXMATH
		operator const D3DXVECTOR2 &() const
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return *reinterpret_cast<const D3DXVECTOR2 *>(this);
		}
		operator D3DXVECTOR2 &()
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return *reinterpret_cast<D3DXVECTOR2 *>(this);
		}
		operator const D3DXVECTOR2 *() const
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return reinterpret_cast<const D3DXVECTOR2 *>(this);
		}
		operator D3DXVECTOR2 *()
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return reinterpret_cast<D3DXVECTOR2 *>(this);
		}
		#endif
	};

	/// 幾何的意味の無い積算
	template<typename _Type, typename _Type1, typename _Type2>
	inline Vector2d<_Type> mult(const Vector2d<_Type1> &lhs, const Vector2d<_Type2> &rhs) { return Vector2d<_Type>(lhs)*=rhs; }

	template<class E, class T, typename _Type> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, Vector2d<_Type> const & v)
	{
		os<<"{"<<v.x<<","<<v.y<<"}";
		return os;
	}

	/// コンストラクタ付版
	template<typename _Type>
	struct Vector2dC : Vector2d<_Type> {
		Vector2dC() {}
		Vector2dC(const _Type *p)
		{
			set(p);
		}
		Vector2dC(_Type x, _Type y)
		{
			set(x, y);
		}
		template<typename _Type2>
		Vector2dC(const Vector2d<_Type2> &src)
		{
			set(src);
		}
		#ifdef GCTP_USE_D3DXMATH
		Vector2dC(const D3DXVECTOR2 &src)
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			set(src.x, src.y);
		}
		#endif
	};

}} // namespace gctp

// D3DXライブラリサポート
#ifdef GCTP_USE_D3DXMATH
#include <gctp/math/d3d/vector2d.inl>
#endif

#endif //_GCTP_MATH_VECTOR2D_HPP_