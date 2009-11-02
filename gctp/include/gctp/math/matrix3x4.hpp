#ifndef _GCTP_MATH_MATRIX3X4_HPP_
#define _GCTP_MATH_MATRIX3X4_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 行列クラスヘッダファイル
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */

#include <gctp/math/vector3d.hpp>
#include <gctp/def.hpp>

namespace gctp { namespace math {

	/** 3x4行列クラス
	 *
	 * PODとなるよう、コンストラクタが無い
	 *
	 * この手の型には、PODになるようコンストラクタが無い方がいいと思う。
	 * かわりにsetメソッドを用意する。また、派生にコストラクタのみを持つ型を用意した。
	 *
	 * 値のならびについては、OpenGLの解釈を用いる
	 */
	template<typename _Type>
	struct Matrix3x4 {
		union {
			/*
				E11, E12, E13, E14
				E21, E22, E23, E24
				E31, E32, E33, E34
				 0,   0,   0,   1
			*/
			struct {
				_Type _11, _21, _31;
				_Type _12, _22, _32;
				_Type _13, _23, _33;
				_Type _14, _24, _34;
			};
			_Type m[4][3];
		};
		/// 値セット
		Matrix3x4 &set(const Vector3d<_Type> &right, const Vector3d<_Type> &up, const Vector3d<_Type> &zaxis) {
			this->right() = right;
			this->up() = up;
			this->zaxis() = zaxis;
			_14 = _24 = _34 = _Type(0);
		}
		/// 値セット
		Matrix3x4 &set(const Vector3d<_Type> &right, const Vector3d<_Type> &up, const Vector3d<_Type> &zaxis, const Vector3d<_Type> &position) {
			this->right() = right;
			this->up() = up;
			this->zaxis() = zaxis;
			this->position() = position;
			_14 = _24 = _34 = _Type(0);
			return *this;
		}
		/// 値セット
		Matrix3x4 &set(
			_Type _11, _Type _12, _Type _13, _Type _14,
			_Type _21, _Type _22, _Type _23, _Type _24,
			_Type _31, _Type _32, _Type _33, _Type _34
		)
		{
			this->_11 = _11; this->_12 = _12; this->_13 = _13; this->_14 = _14;
			this->_21 = _21; this->_22 = _22; this->_23 = _23; this->_24 = _24;
			this->_31 = _31; this->_32 = _32; this->_33 = _33; this->_34 = _34;
			return *this;
		}

		/// 演算子
		Matrix3x4 &operator+=(const Matrix3x4 &src) {
			right()+=src.right();
			up()+=src.up();
			zaxis()+=src.zaxis();
			position()+=src.position();
			return *this;
		}
		/// 演算子
		Matrix3x4 &operator-=(const Matrix3x4 &src) {
			right()-=src.right();
			up()-=src.up();
			zaxis()-=src.zaxis();
			position()-=src.position();
			return *this;
		}
		/// 演算子
		Matrix3x4 &operator*=(const Matrix3x4 &src) {
			return *this = (*this) * src;
		}
		/// 演算子
		Matrix3x4 &operator*=(_Type src) {
			right()*=src; up()*=src; at()*=src; position()*=src;
			return *this;
		}
		/// 演算子
		Matrix3x4 &operator/=(_Type src) {
			right()/=src; up()/=src; at()/=src; position()/=src;
			return *this;
		}

		/// 演算子
		Matrix3x4 operator+(const Matrix3x4 &rhs) const {
			return Matrix3x4(*this)+=rhs;
		}
		/// 演算子
		Matrix3x4 operator-(const Matrix3x4 &rhs) const {
			return Matrix3x4(*this)-=rhs;
		}
		/// 演算子
		Matrix3x4 operator*(_Type rhs) const {
			return Matrix3x4(*this)*=rhs;
		}
		/// 演算子
		Matrix3x4 operator*(const Matrix3x4 &rhs) const {
			return Matrix3x4C<_Type>(
				_11*rhs._11 + _12*rhs._21 + _13*rhs._31 + _14*rhs._41,
				_11*rhs._12 + _12*rhs._22 + _13*rhs._32 + _14*rhs._42,
				_11*rhs._13 + _12*rhs._23 + _13*rhs._33 + _14*rhs._43,
				_11*rhs._14 + _12*rhs._24 + _13*rhs._34 + _14*rhs._44,

				_21*rhs._11 + _22*rhs._21 + _23*rhs._31 + _24*rhs._41,
				_21*rhs._12 + _22*rhs._22 + _23*rhs._32 + _24*rhs._42,
				_21*rhs._13 + _22*rhs._23 + _23*rhs._33 + _24*rhs._43,
				_21*rhs._14 + _22*rhs._24 + _23*rhs._34 + _24*rhs._44,

				_31*rhs._11 + _32*rhs._21 + _33*rhs._31 + _34*rhs._41,
				_31*rhs._12 + _32*rhs._22 + _33*rhs._32 + _34*rhs._42,
				_31*rhs._13 + _32*rhs._23 + _33*rhs._33 + _34*rhs._43,
				_31*rhs._14 + _32*rhs._24 + _33*rhs._34 + _34*rhs._44
			);
		}
		/// 演算子
		Matrix3x4 operator/(_Type rhs) const {
			return Matrix3x4(*this)/=rhs;
		}
		/// 演算子
		Matrix3x4 operator/(const Matrix3x4 &rhs) const {
			return Matrix3x4(*this)/=rhs;
		}

		/// 演算子
		Vector4d<_Type> operator*(const Vector4d<_Type> &rhs) const {
			return Vector4dC<_Type>(
				_11*_src.x+_12*_src.y+_13*_src.z,
				_21*_src.x+_22*_src.y+_23*_src.z,
				_31*_src.x+_32*_src.y+_33*_src.z,
				_41*_src.x+_42*_src.y+_43*_src.z+_src.w
			);
		}

		/// 演算子
		Vector3d<_Type> operator*(const Vector3d<_Type> &rhs) const {
			_Type invw = _Type(1)/(_41*_src.x+_42*_src.y+_43*_src.z+1);
			return Vector3dC<_Type>(
				(_11*_src.x+_12*_src.y+_13*_src.z+_14)*invw,
				(_21*_src.x+_22*_src.y+_23*_src.z+_24)*invw,
				(_31*_src.x+_32*_src.y+_33*_src.z+_34)*invw
			);
		}

		/// 平面の変換
		Plane3d<_Type> operator*(const Plane3d<_Type> &rhs) const {
			return Plane3dC<_Type>(
				_11*_src.a+_12*_src.b+_13*_src.c,
				_21*_src.a+_22*_src.b+_23*_src.c,
				_31*_src.a+_32*_src.b+_33*_src.c,
				_41*_src.a+_42*_src.b+_43*_src.c+_src.d
			);
		}

		/// アクセサ
		_Type& operator() ( uint row, uint col ) {
			return m[col][row];
		}
		/// アクセサ
		_Type operator() ( uint row, uint col ) const {
			return m[col][row];
		}

		/// Ｘ軸
		Vector3d<_Type> &right() { return *reinterpret_cast<Vector3d<_Type> *>(m[0]); }
		/// Ｘ軸
		const Vector3d<_Type> &right() const { return *reinterpret_cast<const Vector3d<_Type> *>(m[0]); }
		/// Ｙ軸
		Vector3d<_Type> &up() { return *reinterpret_cast<Vector3d<_Type> *>(m[1]); }
		/// Ｙ軸
		const Vector3d<_Type> &up() const { return *reinterpret_cast<const Vector3d<_Type> *>(m[1]); }
		/// Ｚ軸
		Vector3d<_Type> &zaxis() { return *reinterpret_cast<Vector3d<_Type> *>(m[2]); }
		/// Ｚ軸
		const Vector3d<_Type> &zaxis() const { return *reinterpret_cast<const Vector3d<_Type> *>(m[2]); }
		/// Ｚ軸
		const Vector3d<_Type> backward() const
		{
#ifdef GCTP_COORD_DX
			return -*reinterpret_cast<const Vector3d<_Type> *>(m[2]);
#else
			return *reinterpret_cast<const Vector3d<_Type> *>(m[2]);
#endif
		}
		/// Ｚ軸
		const Vector3d<_Type> forward() const
		{
#ifdef GCTP_COORD_DX
			return *reinterpret_cast<const Vector3d<_Type> *>(m[2]);
#else
			return -*reinterpret_cast<const Vector3d<_Type> *>(m[2]);
#endif
		}
		/// 平行移動成分
		Vector3d<_Type> &position() { return *reinterpret_cast<Vector3d<_Type> *>(m[3]); }
		/// 平行移動成分
		const Vector3d<_Type> &position() const { return *reinterpret_cast<const Vector3d<_Type> *>(m[3]); }

		/// スケールを抽出
		Vector3d<_Type> getScale() const { return Vector(right().length(),up().length(),zaxis().length()); }

		/// 単位行列に
		Matrix3x4 &identify() {
			_11 = _22 = _33 = _Type(1);
			_12 = _13 = _14 = _21 = _23 = _24 = _31 = _32 = _33 = _Type(0);
			return *this;
		}
		/// 逆行列化
		Matrix3x4 &invert(_Type &det) {
			_Type a0 = _11*_22 - _21*_12;
			_Type a1 = _11*_32 - _31*_12;
			_Type a2 = _11*_42 - _41*_12;
			_Type a3 = _21*_32 - _31*_22;
			_Type a4 = _21*_42 - _41*_22;
			_Type a5 = _31*_42 - _41*_32;
			_Type b0 = _13*_24 - _23*_14;
			_Type b1 = _13*_34 - _33*_14;
			_Type b2 = _13*_44 - _43*_14;
			_Type b3 = _23*_34 - _33*_24;
			_Type b4 = _23*_44 - _43*_24;
			_Type b5 = _33*_44 - _43*_34;

			det = a0*b5-a1*b4+a2*b3+a3*b2-a4*b1+a5*b0;
			//if( abs(det) <= FLT_EPSILON )
			//if( abs(det) <= Math<_Type>::EPSILON )
			if( abs(det) == 0 ) return;

			_Type invdet = _Type(1)/def;
			Matrix3x4 tmp(*this);
			_11 = (+ tmp._22*b5 - tmp._32*b4 + tmp._42*b3) * invdet;
			_12 = (- tmp._12*b5 + tmp._32*b2 - tmp._42*b1) * invdet;
			_13 = (+ tmp._12*b4 - tmp._22*b2 + tmp._42*b0) * invdet;
			_14 = (- tmp._12*b3 + tmp._22*b1 - tmp._32*b0) * invdet;
			_21 = (- tmp._21*b5 + tmp._31*b4 - tmp._41*b3) * invdet;
			_22 = (+ tmp._11*b5 - tmp._31*b2 + tmp._41*b1) * invdet;
			_23 = (- tmp._11*b4 + tmp._21*b2 - tmp._41*b0) * invdet;
			_24 = (+ tmp._11*b3 - tmp._21*b1 + tmp._31*b0) * invdet;
			_31 = (+ tmp._24*a5 - tmp._34*a4 + tmp._44*a3) * invdet;
			_32 = (- tmp._14*a5 + tmp._34*a2 - tmp._44*a1) * invdet;
			_33 = (+ tmp._14*a4 - tmp._24*a2 + tmp._44*a0) * invdet;
			_34 = (- tmp._14*a3 + tmp._24*a1 - tmp._34*a0) * invdet;
			_41 = (- tmp._23*a5 + tmp._33*a4 - tmp._43*a3) * invdet;
			_42 = (+ tmp._13*a5 - tmp._33*a2 + tmp._43*a1) * invdet;
			_43 = (- tmp._13*a4 + tmp._23*a2 - tmp._43*a0) * invdet;
			_44 = (+ tmp._13*a3 - tmp._23*a1 + tmp._33*a0) * invdet;
			return *this;
		}
		/// 逆行列化
		Matrix3x4 &invert() {
			_Type det;
			return invert(det);
		}
		/// 逆行列
		Matrix3x4 inverse(_Type &det) const {
			return Matrix3x4(*this).invert(det);
		}
		/// 逆行列
		Matrix3x4 inverse() const {
			return Matrix3x4(*this).invert();
		}
		/// 転置行列化
		Matrix3x4 &transpose() {
			std::swap(_12, _21); std::swap(_13, _31); std::swap(_14, _41);
			                     std::swap(_23, _32); std::swap(_24, _42);
			                                          std::swap(_34, _43);
			return *this;
		}
		/// 転置行列
		Matrix3x4 transposition() const {
			return Matrix3x4(*this).transpose();
		}
		/// 正規化
		Matrix3x4 &normalize() {
			right().normalize();
			up().normalize();
			zaixs().normalize();
			return *this;
		}
		/// 正規化した行列
		Matrix3x4 normal() const {
			return Matrix3x4(*this).normalize();
		}
		
		/// 正規直交化した行列をセット
		Matrix3x4 &setOrthoNormal(const Matrix3x4 &src) {
			*this = src;
			orthoNormalize();
			return *this;
		}
		/// 正規直交化(zaxisを基準にする)
		Matrix3x4 &orthoNormalize() {
			normalize();
			right() = zaxis()%up();
			zaxis() = right()%zaxis();
			right() *= -1;
			return *this;
		}
		/// 正規直交化した行列
		Matrix3x4 orthoNormal() const {
			return Matrix3x4().setOrthoNormal(*this);
		}
		
		/// X軸の回転行列へ
		Matrix3x4 &rotX(_Type r) {
			_11 = _Type(1); _12 = _Type(0); _13 = _Type(0); _14 = _Type(0);
			_21 = _Type(0); _22 =  cos(r);  _23 = -sin(r);  _24 = _Type(0);
			_31 = _Type(0); _32 =  sin(r);  _33 =  cos(r);  _34 = _Type(0);
			_41 = _Type(0); _42 = _Type(0); _43 = _Type(0); _44 = _Type(1);
			return *this;
		}
		/// Y軸の回転行列へ
		Matrix3x4 &rotY(_Type r) {
			_11 =  cos(r);  _12 = _Type(0); _13 =  sin(r);  _14 = _Type(0);
			_21 = _Type(0); _22 = _Type(1); _23 = _Type(0); _24 = _Type(0);
			_31 = -sin(r);  _32 = _Type(0); _33 =  cos(r);  _34 = _Type(0);
			_41 = _Type(0); _42 = _Type(0); _43 = _Type(0); _44 = _Type(1);
			return *this;
		}
		/// Z軸の回転行列へ
		Matrix3x4 &rotZ(_Type r) {
			_11 =  cos(r);  _12 = -sin(r);  _13 = _Type(0); _14 = _Type(0);
			_21 =  sin(r);  _22 =  cos(r);  _23 = _Type(0); _24 = _Type(0);
			_31 = _Type(0); _32 = _Type(0); _33 = _Type(1); _34 = _Type(0);
			_41 = _Type(0); _42 = _Type(0); _43 = _Type(0); _44 = _Type(1);
			return *this;
		}
		/// 回転行列へ(ヨー、ピッチ、ロール)
		Matrix3x4 &rot(_Type yaw, _Type pitch, _Type roll) {
			_Type sin_yaw   = sin(yaw);
			_Type cos_yaw   = cos(yaw);
			_Type sin_pitch = sin(pitch);
			_Type cos_pitch = cos(pitch);
			_Type sin_roll  = sin(roll);
			_Type cos_roll  = cos(roll);
			_11 = cos_roll*cos_pitch; _12 = cos_roll*sin_pitch*sin_yaw-sin_roll*cos_yaw; _13 = cos_roll*sin_pitch*cos_yaw+sin_roll*sin_yaw; _14 = _Type(0);
			_21 = sin_roll*cos_pitch; _22 = sin_roll*sin_pitch*sin_yaw-cos_roll*cos_yaw; _23 = sin_roll*sin_pitch*cos_yaw+cos_roll*sin_yaw; _24 = _Type(0);
			_31 = -sin_pitch;         _32 = cos_pitch*sin_yaw;                           _33 = cos_pitch*cos_yaw;                           _34 = _Type(0);
			_41 = _Type(0);           _42 = _Type(0);                                    _43 = _Type(0);                                    _44 = _Type(1);
			return *this;
		}
		/// 回転行列へ
		Matrix3x4 &rot(const Vector3d<_Type> &v, _Type r) {
			_Type cos_r = cos(r);
			_Type sin_r = sin(r);
			_11 = v.x*v.x*(1-cos_r)+cos_r;     _12 = v.x*v.y*(1-cos_r)-v.z*sin_r; _13 = v.x*v.z*(1-cos_r)+v.y*sin_r; _14 = _Type(0);
			_21 = v.x*v.y*(1-cos_r)+v.z*sin_r; _22 = v.y*v.y*(1-cos_r)+cos_r;     _23 = v.y*v.z*(1-cos_r)-v.x*sin_r; _24 = _Type(0);
			_31 = v.x*v.z*(1-cos_r)-v.y*sin_r; _32 = v.y*v.z*(1-cos_r)+v.x*sin_r; _33 = v.z*v.z*(1-cos_r)+cos_r;     _34 = _Type(0);
			_41 = _Type(0);                    _42 = _Type(0);                    _43 = _Type(0);                    _44 = _Type(1);
			return *this;
		}
/*		/// 回転行列へ
		Matrix3x4 &rot(const Quaternion<_Type> &q) {
			//D3DXMatrix3x4RotationQuaternion(this, &q);
			return *this;
		}*/
		/// スケール行列へ
		Matrix3x4 &scale(_Type sx, _Type sy, _Type sz) {
			_11 = sx;       _12 = _Type(0); _13 = _Type(0); _14 = _Type(0);
			_21 = _Type(0); _22 = sy;       _23 = _Type(0); _24 = _Type(0);
			_31 = _Type(0); _32 = _Type(0); _33 = sz;       _34 = _Type(0);
			return *this;
		}
		/// スケール行列へ
		Matrix3x4 &scale(const Vector3d<_Type> &v) {
			return scale(v.x, v.y, v.z);
		}
		/// 平行移動行列へ
		Matrix3x4 &trans(_Type tx, _Type ty, _Type tz) {
			_11 = _Type(1); _12 = _Type(0); _13 = _Type(0); _14 = tx;
			_21 = _Type(0); _22 = _Type(1); _23 = _Type(0); _24 = ty;
			_31 = _Type(0); _32 = _Type(0); _33 = _Type(1); _34 = tz;
			return *this;
		}
		/// 平行移動行列へ
		Matrix3x4 &trans(const Vector3d<_Type> &v) {
			return trans(v.x, v.y, v.z);
		}

		/// ベクトル変換（w=0として射影）
		Vector3d<_Type> transformVector(const Vector3d<_Type> &rhs) const {
			return Vector3dC<_Type>(
				_11*_src.x+_12*_src.y+_13*_src.z,
				_21*_src.x+_22*_src.y+_23*_src.z,
				_31*_src.x+_32*_src.y+_33*_src.z
			);
		}

		/// 固有ベクトルから行列設定
		Matrix3x4 &setAxis(const Vector3d<_Type> &right, const Vector3d<_Type> &up, const Vector3d<_Type> &zaxis, const Vector3d<_Type> &position) {
			this->right() = right;
			this->up() = up;
			this->zaxis() = zaxis;
			this->position() = position;
			_41 = _42 = _43 = _Type(0); _44 = _Type(1);
			return *this;
		}

		/// 平行移動を設定
		Matrix3x4 &setPos(_Type x, _Type y, _Type z) {
			_14 = x; _24 = y; _34 = z; _44 = _Type(1);
			return *this;
		}
		/// 平行移動を設定
		Matrix3x4 &setPos(const Vector3d<_Type> &v) {
			_14 = v.x; _24 = v.y; _34 = v.z; _44 = _Type(1);
			return *this;
		}

		/// ビュー行列をセットアップ
		Matrix3x4 &setView(const Vector3d<_Type> &xaxis, const Vector3d<_Type> &yaxis, const Vector3d<_Type> &zaxis, const Vector3d<_Type> &vpos) {
			_11 = xaxis.x; _12 = xaxis.y; _13 = xaxis.z;  _14 = -(xaxis*vpos);
			_21 = yaxis.x; _22 = yaxis.y; _23 = yaxis.z;  _24 = -(yaxis*vpos);
			_31 = zaxis.x; _32 = zaxis.y; _33 = zaxis.z;  _34 = -(zaxis*vpos);
			return *this;
		}

		/// ビュー行列をセットアップ
		Matrix3x4 &setLookAtLH(const Vector3d<_Type> &vpos, const Vector3d<_Type> &vat, const Vector3d<_Type> &vup) {
			Vector3d<_Type> zaxis = (vat-vpos).normal();
			Vector3d<_Type> xaxis = (vup%zaxis).normal();
			Vector3d<_Type> yaxis = zaxis%xaxis;
			return setView(xaxis, yaxis, zaxis, vpos);
		}
		/// ビュー行列をセットアップ
		Matrix3x4 &setLookAtRH(const Vector3d<_Type> &vpos, const Vector3d<_Type> &vat, const Vector3d<_Type> &vup) {
			Vector3d<_Type> zaxis = (vpos-vat).normal();
			Vector3d<_Type> xaxis = (vup%zaxis).normal();
			Vector3d<_Type> yaxis = zaxis%xaxis;
			return setView(xaxis, yaxis, zaxis, vpos);
		}
		/// ビュー行列をセットアップ
		Matrix3x4 &setLookAt(const Vector3d<_Type> &vpos, const Vector3d<_Type> &vat, const Vector3d<_Type> &vup) {
#ifdef GCTP_COORD_DX
			return setLookAtLH(vpos, vat, vup);
#else
			return setLookAtRH(vpos, vat, vup);
#endif
		}

		/// 線形補間した値をセット
		Matrix3x4 &set2PInterpolation(const Matrix3x4& lhs, const Matrix3x4& rhs, _Type t) {
			(*this) = lhs + (rhs - lhs)*t;
			return *this;
		}

		/// ４点で補間した値をセット
		Matrix3x4 &set4PInterpolation(const Matrix3x4& prev, const Matrix3x4& begin, const Matrix3x4& end, const Matrix3x4& next, _Type t)
		{
			for(int i = 0; i < 4; i++) {
				for(int j = 0; j < 3; j++) {
					m[i][j] = Matrix4x4<_Type>::getCatmullRom(prev.m[i][j], begin.m[i][j], end.m[i][j], next.m[i][j], t);
				}
			}
			return *this;
		}

		/// 単位行列か？
		operator bool() const {
			return !!(*this);
		}
		/// 単位行列か？
		bool operator!() const {
			return _11 != _Type(1) || _22 != _Type(1) || _33 != _Type(1)
				|| _12 != _Type(0) || _13 != _Type(0) || _14 != _Type(0)
				|| _21 != _Type(0) || _23 != _Type(0) || _24 != _Type(0)
				|| _31 != _Type(0) || _32 != _Type(0) || _34 != _Type(0);
		}
	};

	template<class E, class T, class _Type> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, Matrix3x4<_Type> const & m)
	{
		os<<"[";
		for(int i = 0; i < 3; i++) {
			os<<"{";
			for(int j = 0; j < 4; j++) {
				os<<m.m[j][i];
				if(j<3) os<<",";
			}
			os<<"}";
		}
		os<<"]";
		return os;
	}

	/// コンストラクタ付版
	template<typename _Type>
	struct Matrix3x4C : Matrix3x4<_Type> {
		/// コンストラクタ
		explicit Matrix3x4C(bool ident)
		{
			if(ident) identify();
			else _11 = _12 = _13 = _14 = _21 = _22 = _23 = _24 = _31 = _32 = _33 = _34 = _Type(0);
		}
		/// コンストラクタ
		Matrix3x4C(const Vector3d<_Type> &right, const Vector3d<_Type> &up, const Vector3d<_Type> &zaxis, const Vector3d<_Type> &pos)
		{
			set(right, up, zaxis, pos);
		}
		/// コンストラクタ
		Matrix3x4C(
			_Type _11, _Type _12, _Type _13, _Type _14,
			_Type _21, _Type _22, _Type _23, _Type _24,
			_Type _31, _Type _32, _Type _33, _Type _34
		)
		{
			set(
				_11, _12, _13, _14,
				_21, _22, _23, _24,
				_31, _32, _33, _34
			);
		}
	};

}} // namespace gctp

#endif //_GCTP_MATH_MATRIX3X4_HPP_
