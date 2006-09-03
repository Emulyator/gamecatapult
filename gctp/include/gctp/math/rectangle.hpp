#ifndef _GCTP_MATH_RECTANGLE_HPP_
#define _GCTP_MATH_RECTANGLE_HPP_
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

#include <gctp/math/vector2d.hpp>

namespace gctp { namespace math {

	/** 矩形テンプレート
	 *
	 * PODとなるよう、コンストラクタが無い
	 *
	 * この手の型には、PODになるようコンストラクタが無い方がいいと思う。
	 * かわりにsetメソッドを用意する。
	 */
	template<typename _Type>
	struct Rectangle {
		_Type left, top, right, bottom;

		/// 値セット
		Rectangle &set(const _Type *p)
		{
			left = p[0]; top = p[1]; right = p[2]; bottom = p[3];
			return *this;
		}
		/// 値セット
		Rectangle &set(_Type left, _Type top, _Type right, _Type bottom)
		{
			this->left = left; this->top = top; this->right = right; this->bottom = bottom;
			return *this;
		}
		/// 値セット
		template<typename _Type2>
		Rectangle &set(const Rectangle<_Type2> &src)
		{
			left = _Type(src.left); top = _Type(src.top); right = _Type(src.right); bottom = _Type(src.bottom);
			return *this;
		}

		/// 代入
		template<typename _Type2>
		Rectangle &operator+=(const Vector2d<_Type2> &rhs) {
			left += _Type(rhs.left); top += _Type(rhs.top);
			right += _Type(rhs.right); bottom += _Type(rhs.bottom);
			return *this;
		}
		/// 代入
		template<typename _Type2>
		Rectangle &operator-=(const Vector2d<_Type2> &src) {
			left -= _Type(rhs.left); top -= _Type(rhs.top);
			right -= _Type(rhs.right); bottom -= _Type(rhs.bottom);
			return *this;
		}
		/// 代入
		Rectangle &operator*=(Real src) {
			left = _Type(left*src); top = _Type(top*src);
			right = _Type(right*src); bottom = _Type(bottom*src);
			return *this;
		}
		/// 代入
		Rectangle &operator/=(Real src) {
			Real invsrc = Real(1)/src;
			left = _Type(left*invsrc); top = _Type(top*invsrc);
			right = _Type(right*invsrc); bottom = _Type(bottom*invsrc);
			return *this;
		}
		/// 代入
		Rectangle &operator|=(const Rectangle &src) {
			if(left > src.left) left = src.left;
			if(top > src.top) top = src.top;
			if(right < src.right) right = src.right;
			if(bottom < src.bottom) bottom = src.bottom;
			return *this;
		}
		/// 代入
		Rectangle &operator&=(const Rectangle &src) {
			if(left < src.left) left = src.left;
			if(top < src.top) top = src.top;
			if(right > src.right) right = src.right;
			if(bottom > src.bottom) bottom = src.bottom;
			return *this;
		}
		
		/// 演算子
		Rectangle operator - () const { return Rectangle().set(-left, -top, -right, -bottom); }
		/// 演算子
		Rectangle operator+(const Rectangle &rhs) const { return Rectangle(*this)+=rhs; }
		/// 演算子
		Rectangle operator-(const Rectangle &rhs) const { return Rectangle(*this)-=rhs; }
		/// 演算子
		Rectangle operator/(const Rectangle &rhs) const { return Rectangle(*this)/=rhs; }
		/// スケール演算子
		Rectangle operator*(Real rhs) const { return Rectangle(*this)*=rhs; }
		/// スケール演算子
		friend Rectangle operator*(Real lhs, const Rectangle &rhs) { return Rectangle(rhs)*=lhs; }
		/// スケール演算子
		Rectangle operator/(Real rhs) const { return Rectangle(*this)/=rhs; }
		/// スケール演算子
		friend Rectangle operator/(Real lhs, const Rectangle &rhs) { return Rectangle(rhs)/=lhs; }
		/// 最大化演算子
		Rectangle operator|(const Rectangle &rhs) const { return Rectangle(*this)|=rhs; }
		/// 最小化演算子
		Rectangle operator&(const Rectangle &rhs) const { return Rectangle(*this)&=rhs; }

		// 比較演算子
		bool operator==(const Rectangle &rhs)
		{
			return (left == rhs.left && top == rhs.top && right == rhs.right && bottom == rhs.bottom);
		}
		bool operator!=(const Rectangle &rhs)
		{
			return (left != rhs.left || top != rhs.top || right != rhs.right || bottom != rhs.bottom);
		}

		_Type height() const { return abs(bottom-top); }
		_Type width() const { return abs(right-left); }
		Rectangle &regularize() {
			if(left>right) { (std::swap)(left, right); }
			if(top>bottom) { (std::swap)(top, bottom); }
			return *this;
		}
		Rectangle regular() const {
			return Rectangle(*this).regular();
		}
		bool isHit(const Vector2d<_Type> &p) const {
			return (left <= p.x && p.x <= right && top <= p.y && p.y <= bottom);
		}
		bool isHitLogical(const Vector2d<_Type> &p) const {
			return (left <= p.x && p.x < right && top <= p.y && p.y < bottom);
		}
		bool isHit(const Rectangle &rc) const {
			Rectangle united = *this | rc;
			return (united.height() < (height()+rc.height()) && united.width() < (width()+rc.width()));
		}

		// D3DXライブラリサポート
		#ifdef GCTP_USE_D3DXMATH
		operator const RECT &() const
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, unsigned long>::value));
			return *reinterpret_cast<const RECT *>(this);
		}
		operator RECT &()
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, unsigned long>::value));
			return *reinterpret_cast<RECT *>(this);
		}
		operator const RECT *() const
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, unsigned long>::value));
			return reinterpret_cast<const RECT *>(this);
		}
		operator RECT *()
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, unsigned long>::value));
			return reinterpret_cast<RECT *>(this);
		}
		#endif
	};

	template<class E, class T, typename _Type> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, Rectangle<_Type> const & rc)
	{
		os<<"{"<<rc.left<<","<<rc.top<<","<<rc.right<<","<<rc.bottom<<"}";
		return os;
	}

	/// コンストラクタ付版
	template<typename _Type>
	struct RectangleC : Rectangle<_Type> {
		RectangleC() {}
		RectangleC(const _Type *p)
		{
			set(p);
		}
		RectangleC(_Type left, _Type top, _Type right, _Type bottom)
		{
			set(left, top, right, bottom);
		}
		RectangleC(const Vector2d<_Type> &left_top, const Vector2d<_Type> &right_bottom)
		{
			set(left_top.x, left_top.y, right_bottom.x, right_bottom.y);
		}
		template<typename _Type2>
		RectangleC(const Rectangle<_Type2> &src)
		{
			set(src);
		}
		#ifdef GCTP_USE_D3DXMATH
		RectangleC(const RECT &src)
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, unsigned long>::value));
			set(src.left, src.top, src.right, src.bottom);
		}
		#endif
	};

}} // namespace gctp

#endif //_GCTP_MATH_RECTANGLE_HPP_