#ifndef _GCTP_MATH_MATRIX2X2_HPP_
#define _GCTP_MATH_MATRIX2X2_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 2x2行列クラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 2:42:16
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/math/vector2d.hpp>

namespace gctp { namespace math {

	/** 2x2行列クラス
	 *
	 * PODとなるよう、コンストラクタが無い
	 *
	 * 値のならびについては、OpenGLの解釈を用いる
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/16 5:02:29
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<typename _Type>
	struct Matrix2x2 : Matrix2x2<_Type> {
		union {
			struct {
				_Type _11, _21;
				_Type _12, _22;
			};
			_Type m[2][2];
		};

		/// 値セット
		Matrix2x2 &set(
			_Type _11, _Type _12,
			_Type _21, _Type _22
		)
		{
			this->_11 = _11; this->_21 = _21;
			this->_12 = _12; this->_22 = _22;
			return *this;
		}
		/// 値セット
		Matrix2x2 &set(const _Type *p)
		{
			memcpy(m, _m, sizeof(_Type)*4);
			return *this;
		}
		/// 値セット
		Matrix2x2 &set(const Vector2d<_Type> &right, const Vector2d<_Type> &up)
		{
			this->right() = right; this->up() = up;
			return *this;
		}

		/// アクセス関数
		_Type &operator () ( int row, int col ) {
			return m[col][row];
		}
		/// アクセス関数
		const _Type &operator () ( int row, int col ) const {
			return m[col][row];
		}

		/// X軸
		Vector2d<_Type> &right() {
			return *(Vector2d<_Type> *)(&m[0][0]);
		}
		/// X軸
		const Vector2d<_Type> &right() const {
			return *(Vector2d<_Type> *)(&m[0][0]);
		}
		/// Y軸
		Vector2d<_Type> &up() {
			return *(Vector2 *)(&m[1][0]);
		}
		/// Y軸
		const Vector2d<_Type> &up() const {
			return *(Vector2 *)(&m[1][0]);
		}

		/// スケールを抽出
		Vector2<_Type> getScaling() const { return (Vector2<_Type>){right().length(), up().length()}; }

		/// キャスト演算ども
		operator _Type* () { return &m[0][0]; }
		/// キャスト演算ども
		operator const _Type* () const { return &m[0][0]; }

		/// 代入
		Matrix2x2& operator *= ( const Matrix2x2 &rhs ) {
			return *this = (Matrix2x2<_Type>){
				_11*rhs._11 + _12*rhs._21, _21*rhs._11 + _22*rhs._21,
				_11*rhs._12 + _12*rhs._22, _21*rhs._12 + _22*rhs._22
			};
		}
		/// 代入
		Matrix2x2& operator += ( const Matrix2x2 &rhs ) {
			_11 += rhs._11; _21 += rhs._21;
			_12 += rhs._12; _22 += rhs._22;
		}
		/// 代入
		Matrix2x2& operator -= ( const Matrix2x2 &rhs ) {
			_11 -= rhs._11; _21 -= rhs._21;
			_12 -= rhs._12; _22 -= rhs._22;
		}
		/// 代入
		Matrix2x2& operator *= ( _Type rhs ) {
			_11 *= rhs; _21 *= rhs;
			_12 *= rhs; _22 *= rhs;
		}
		/// 代入
		Matrix2x2& operator /= ( _Type rhs ) {
			_11 /= rhs; _21 /= rhs;
			_12 /= rhs; _22 /= rhs;
		}

		/// 単項演算
		Matrix2x2 operator + () const { return *this; }
		/// 単項演算
		Matrix2x2 operator - () const { return (Matrix2x2<_Type>){{-_11, -_21}, {-_12, -_22}}; }

		/// ２項演算
		Matrix2x2 operator * ( const Matrix2x2 &rhs ) const { return Matrix2x2C<_Type>(*this)*=rhs; }
		/// ２項演算
		Matrix2x2 operator + ( const Matrix2x2 &rhs ) const { return Matrix2x2C<_Type>(*this)+=rhs; }
		/// ２項演算
		Matrix2x2 operator - ( const Matrix2x2 &rhs ) const { return Matrix2x2C<_Type>(*this)-=rhs; }
		/// ２項演算
		Matrix2x2 operator * ( _Type rhs ) const { return Matrix2x2C<_Type>(*this)*=rhs; }
		/// ２項演算
		Matrix2x2 operator / ( _Type rhs ) const { return Matrix2x2C<_Type>(*this)/=rhs; }
		/// ２項演算
		friend Matrix2x2 operator * ( _Type lhs, const Matrix2x2 &rhs ) { return rhs*lhs; }
		/// ２項演算
		friend Matrix2x2 operator / ( _Type lhs, const Matrix2x2 &rhs ) { return rhs/lhs; }
		/// 演算子
		Vector2d<_Type> operator*(const Vector2d<_Type2> &rhs) const {
			Vector ret;
			transform(ret, rhs);
			return ret;
		}

		/// ２項演算
		bool operator == ( const Matrix2x2 &rhs ) const { return _11==rhs._11 && _21==rhs._21 && _31==rhs._31 && _12==rhs._12 && _22==rhs._22 && _32==rhs._32 && _13==rhs._13 && _23==rhs._23 && _33==rhs._33; }
		/// ２項演算
		bool operator != ( const Matrix2x2 &rhs ) const { return _11!=rhs._11 || _21!=rhs._21 || _31!=rhs._31 || _12!=rhs._12 || _22!=rhs._22 || _32!=rhs._32 || _13!=rhs._13 || _23!=rhs._23 || _33!=rhs._33; }

		/// 回転をセット
		Matrix2x2 &rot(_Type theta) { _11 = _22 = cos<_Type>(theta); _12 = sin<_Type>(theta); _21 = -_12; _33=1; _13=_23=_31=_32=0; return *this; }
		/// スケールをセット
		Matrix2x2 &scale(_Type x, _Type y) { _11=x; _22=y; _33=1; _13=_23=_31=_32=0; return *this; }
		/// スケールをセット
		Matrix2x2 &scale(const Vector2d<_Type> &scl) { return scale(scl.x, scl.y); }

		/// 単位行列に
		Matrix2x2 &identify()
		{
			_11 = _22 = _Type(1);
			_21 = _12 = _Type(0);
			return *this;
		}

		/// 正規化
		Matrix2x2 &normalize() {
			right().normalize();
			up().normalize();
			return *this;
		}
		/// 正規化した行列
		Matrix2x2 normal() const {
			return Matrix2x2(*this).normalize();
		}
		
		/// 変換
		const Matrix2x2 &transform(Vector2d<_Type> &dst, const Vector2d<_Type> &src) const {
			dst.x = _11*src.x+_12*src.y;
			dst.y = _21*src.x+_22*src.y;
			return *this;
		}
	};

	template<class E, class T> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, Matrix2x2 const & m)
	{
		os<<"{["<<m._11<<","<<m._12<<"],["<<m._21<<","<<m._22<<"]}";
		return os;
	}

	/// コンストラクタ付
	template<typename _Type>
	struct Matrix2x2C : Matrix2x2<_Type> {
		/// コンストラクタ
		Matrix2x2C() {}
		/// コンストラクタ
		explicit Matrix2x2C(bool ident)
		{
			if(ident) identify();
			else _11 = _12 = _21 = _22 = _Type(0);
		}
		/// コンストラクタ
		Matrix2x2C(
			_Type _11, _Type _12,
			_Type _21, _Type _22
		)
		{
			set(_11, _21, _12, _22);
		}
		/// コンストラクタ
		Matrix2x2C(const _Type *p)
		{
			set(p);
		}
		/// コンストラクタ
		Matrix2x2C(const Vector2d<_Type> &right, const Vector2d<_Type> &up)
		{
			set(right, up);
		}
	};

}} //namespace gctp

#endif //_GCTP_MATH_MATRIX2X2_HPP_