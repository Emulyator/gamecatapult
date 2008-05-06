#ifndef _GCTP_MATH_MATRIX4X4_HPP_
#define _GCTP_MATH_MATRIX4X4_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 行列クラスヘッダファイル
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: matrix.hpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include <iosfwd>               // for std::basic_ostream
#include <gctp/def.hpp>

#include <gctp/math/vector3d.hpp>
#include <gctp/math/vector4d.hpp>
#include <gctp/math/plane3d.hpp>

namespace gctp { namespace math {

	/** 4x4行列クラス
	 *
	 * PODとなるよう、コンストラクタが無い
	 *
	 * この手の型には、PODになるようコンストラクタが無い方がいいと思う。
	 * かわりにsetメソッドを用意する。また、派生にコストラクタのみを持つ型を用意した。
	 *
	 * 値のならびについては、OpenGLの解釈を用いる
	 */
	template<typename _Type>
	struct Matrix4x4 {
		union {
			struct {
				_Type _11, _21, _31, _41;
				_Type _12, _22, _32, _42;
				_Type _13, _23, _33, _43;
				_Type _14, _24, _34, _44;
			};
			_Type m[4][4];
		};
		/// 値セット
		Matrix4x4 &set(const Vector3d<_Type> &right, const Vector3d<_Type> &up, const Vector3d<_Type> &at) {
			this->right() = right;
			this->up() = up;
			this->at() = at;
			_14 = _24 = _34 = _41 = _42 = _43 = _Type(0);
			_44 = _Type(1);
		}
		/// 値セット
		Matrix4x4 &set(const Vector3d<_Type> &right, const Vector3d<_Type> &up, const Vector3d<_Type> &at, const Vector3d<_Type> &pos) {
			this->right() = right;
			this->up() = up;
			this->at() = at;
			position() = pos;
			_41 = _42 = _43 = _Type(0);
			_44 = _Type(1);
			return *this;
		}
		/// 値セット
		Matrix4x4 &set(
			_Type _11, _Type _12, _Type _13, _Type _14,
			_Type _21, _Type _22, _Type _23, _Type _24,
			_Type _31, _Type _32, _Type _33, _Type _34,
			_Type _41, _Type _42, _Type _43, _Type _44
		)
		{
			this->_11 = _11; this->_12 = _12; this->_13 = _13; this->_14 = _14;
			this->_21 = _21; this->_22 = _22; this->_23 = _23; this->_24 = _24;
			this->_31 = _31; this->_32 = _32; this->_33 = _33; this->_34 = _34;
			this->_41 = _41; this->_42 = _42; this->_43 = _43; this->_44 = _44;
			return *this;
		}

		/// 演算子
		Matrix4x4 &operator+=(const Matrix4x4 &src) {
			right4d()+=src.right4d();
			up4d()+=src.up4d();
			at4d()+=src.at4d();
			position4d()+=src.position4d();
			return *this;
		}
		/// 演算子
		Matrix4x4 &operator-=(const Matrix4x4 &src) {
			right4d()-=src.right4d();
			up4d()-=src.up4d();
			at4d()-=src.at4d();
			position4d()-=src.position4d();
			return *this;
		}
		/// 演算子
		Matrix4x4 &operator*=(const Matrix4x4 &src) {
			/* // Strassenのアルゴリズム
			Matrix2x2<_Type> A11;*/
/*			return *this = (Matrix4x4){
_11*rhs._11+_12*rhs._21+_13*rhs._31+_14*rhs._41, _21*rhs._11+_22*rhs._21+_23*rhs._31+_24*rhs._41, _21*rhs._11+_22*rhs._21+_23*rhs._31+_24*rhs._41, _21*rhs._11+_22*rhs._21+_23*rhs._31+_24*rhs._41,

_11*rhs._12+_12*rhs._22, _21*rhs._12+_22*rhs._22
			};*/
			Matrix4x4 tmp = src;
			for (int row = 0; row < 4; row++) {
				for (int col = 0; col < 4; col++) {
					tmp.m[row][col] = _Type(0);
					for(int mid = 0; mid < 4; mid++) {
						tmp.m[row][col] += m[row][mid]*src.m[mid][col];
					}
				}
			}
			*this = tmp;
			return *this;
		}
		/// 演算子
		Matrix4x4 &operator*=(_Type src) {
			right4d()*=src; up4d()*=src; at4d()*=src; position4d()*=src;
			return *this;
		}
		/// 演算子
		Matrix4x4 &operator/=(_Type src) {
			right4d()/=src; up4d()/=src; at4d()/=src; position4d()/=src;
			return *this;
		}

		/// 演算子
		Matrix4x4 operator+(const Matrix4x4 &rhs) const {
			return Matrix4x4(*this)+=rhs;
		}
		/// 演算子
		Matrix4x4 operator-(const Matrix4x4 &rhs) const {
			return Matrix4x4(*this)-=rhs;
		}
		/// 演算子
		Matrix4x4 operator*(_Type rhs) const {
			return Matrix4x4(*this)*=rhs;
		}
		/// 演算子
		Matrix4x4 operator*(const Matrix4x4 &rhs) const {
			return Matrix4x4(*this)*=rhs;
		}
		/// 演算子
		Matrix4x4 operator/(_Type rhs) const {
			return Matrix4x4(*this)/=rhs;
		}
		/// 演算子
		Matrix4x4 operator/(const Matrix4x4 &rhs) const {
			return Matrix4x4(*this)/=rhs;
		}

		/// 演算子
		Vector4d<_Type> operator*(const Vector4d<_Type> &rhs) const {
			return Vector4dC<_Type>(
				_11*rhs.x+_12*rhs.y+_13*rhs.z+_14*rhs.w,
				_21*rhs.x+_22*rhs.y+_23*rhs.z+_24*rhs.w,
				_31*rhs.x+_32*rhs.y+_33*rhs.z+_34*rhs.w,
				_41*rhs.x+_42*rhs.y+_43*rhs.z+_44*rhs.w
			);
		}

		/// 演算子
		Vector3d<_Type> operator*(const Vector3d<_Type> &rhs) const {
			_Type invw = _Type(1)/(_41*rhs.x+_42*rhs.y+_43*rhs.z+_44);
			return Vector3dC<_Type>(
				(_11*rhs.x+_12*rhs.y+_13*rhs.z+_14)*invw,
				(_21*rhs.x+_22*rhs.y+_23*rhs.z+_24)*invw,
				(_31*rhs.x+_32*rhs.y+_33*rhs.z+_34)*invw
			);
		}

		/// 平面の変換
		Plane3d<_Type> operator*(const Plane3d<_Type> &rhs) const {
			return Plane3dC<_Type>(
				_11*rhs.a+_12*rhs.b+_13*rhs.c+_14*rhs.d,
				_21*rhs.a+_22*rhs.b+_23*rhs.c+_24*rhs.d,
				_31*rhs.a+_32*rhs.b+_33*rhs.c+_34*rhs.d,
				_41*rhs.a+_42*rhs.b+_43*rhs.c+_44*rhs.d
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
		Vector3d<_Type> &at() { return *reinterpret_cast<Vector3d<_Type> *>(m[2]); }
		/// Ｚ軸
		const Vector3d<_Type> &at() const { return *reinterpret_cast<const Vector3d<_Type> *>(m[2]); }
		/// 平行移動成分
		Vector3d<_Type> &position() { return *reinterpret_cast<Vector3d<_Type> *>(m[3]); }
		/// 平行移動成分
		const Vector3d<_Type> &position() const { return *reinterpret_cast<const Vector3d<_Type> *>(m[3]); }

		/// Ｘ軸
		Vector4d<_Type> &right4d() { return *reinterpret_cast<Vector4d<_Type> *>(m[0]); }
		/// Ｘ軸
		const Vector4d<_Type> &right4d() const { return *reinterpret_cast<const Vector4d<_Type> *>(m[0]); }
		/// Ｙ軸
		Vector4d<_Type> &up4d() { return *reinterpret_cast<Vector4d<_Type> *>(m[1]); }
		/// Ｙ軸
		const Vector4d<_Type> &up4d() const { return *reinterpret_cast<const Vector4d<_Type> *>(m[1]); }
		/// Ｚ軸
		Vector4d<_Type> &at4d() { return *reinterpret_cast<Vector4d<_Type> *>(m[2]); }
		/// Ｚ軸
		const Vector4d<_Type> &at4d() const { return *reinterpret_cast<const Vector4d<_Type> *>(m[2]); }
		/// 平行移動成分
		Vector4d<_Type> &position4d() { return *reinterpret_cast<Vector4d<_Type> *>(m[3]); }
		/// 平行移動成分
		const Vector4d<_Type> &position4d() const { return *reinterpret_cast<const Vector4d<_Type> *>(m[3]); }

		/// スケールを抽出
		Vector3d<_Type> getScale() const {
			return Vector3d<_Type>().set(right().length(),up().length(),at().length());
		}

		/// 単位行列に
		Matrix4x4 &identify() {
			_11 = _22 = _33 = _44 = _Type(1);
			_21 = _31 = _41 = _12 = _32 = _42 = _13 = _23 = _43 = _14 = _24 = _34 = _Type(0);
			return *this;
		}
		/// 逆行列化
		Matrix4x4 &invert(_Type &det) {
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
			if( abs(det) == 0 ) return *this;

			_Type invdet = _Type(1)/det;
			Matrix4x4 tmp(*this);
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
		Matrix4x4 &invert() {
#if 0
			Matrix4x4 tmp(*this);
			/* LU分解 */
			for(int i = 0; i < 4; i++) {
				for(int j = i + 1; j < 4; j++){
					tmp.m[j][i] /= tmp.m[i][i];
					for(int k = i + 1; k < 4; k++){
						tmp.m[j][k] -= tmp.m[i][k] * tmp.m[j][i];
					}
				}
			}
			/* 逆行列を求める */
			for(int k = 0; k < 4; k++) {
				/* 初期化 */
				for(int i = 0; i < 4; i++) {
					if(i == k) m[i][k] = _Type(1);
					else m[i][k] = _Type(0);
				}
				/* 解を求める */
				for(int i = 0; i < 4; i++) {
					for(int j = i + 1; j < 4; j++) {
						m[j][k] -= m[i][k] * tmp.m[j][i];
					}
				}
				for(int i = 3; i >= 0; i--) {
					for(int j = i + 1; j < 4; j++ ) {
						m[i][k] -= tmp.m[i][j] * m[j][k];
					}
					m[i][k] /= tmp.m[i][i];
				}
			}
			return *this;
#else
			// 結局こっちのほうが早いんじゃ？
			_Type det;
			return invert(det);
#endif
		}
		/// 逆行列
		Matrix4x4 inverse(_Type &det) const {
			return Matrix4x4(*this).invert(det);
		}
		/// 逆行列
		Matrix4x4 inverse() const {
			return Matrix4x4(*this).invert();
		}
		/// 転置行列化
		Matrix4x4 &transpose() {
			for(int i = 0; i < 3; i++) {
				for(int j = i + 1; j < 4; j++) {
					swap(m[i][j], m[j][i]);
				}
			}
			return *this;
		}
		/// 転置行列
		Matrix4x4 transposition() const {
			return Matrix4x4(*this).transpose();
		}
		/// 正規化
		Matrix4x4 &normalize() {
			right().normalize();
			up().normalize();
			at().normalize();
			return *this;
		}
		/// 正規化した行列
		Matrix4x4 normal() const {
			return Matrix4x4(*this).normalize();
		}
		
		/// 正規直交化した行列をセット
		Matrix4x4 &setOrthoNormal(const Matrix4x4 &src) {
			*this = src;
			orthoNormalize();
			return *this;
		}
		/// 正規直交化(atvを基準にする)
		Matrix4x4 &orthoNormalize() {
			normalize();
			right() = at()%up();
			up() = right()%at();
			right() *= -1;
			return *this;
		}
		/// 正規直交化した行列
		Matrix4x4 orthoNormal() const {
			return Matrix4x4().setOrthoNormal(*this);
		}
		
		/// X軸の回転行列へ
		Matrix4x4 &rotX(_Type r) {
			_11 = _Type(1); _12 = _Type(0); _13 = _Type(0); _14 = _Type(0);
			_21 = _Type(0); _22 =  cos(r);  _23 = -sin(r);  _24 = _Type(0);
			_31 = _Type(0); _32 =  sin(r);  _33 =  cos(r);  _34 = _Type(0);
			_41 = _Type(0); _42 = _Type(0); _43 = _Type(0); _44 = _Type(1);
			return *this;
		}
		/// Y軸の回転行列へ
		Matrix4x4 &rotY(_Type r) {
			_11 =  cos(r);  _12 = _Type(0); _13 =  sin(r);  _14 = _Type(0);
			_21 = _Type(0); _22 = _Type(1); _23 = _Type(0); _24 = _Type(0);
			_31 = -sin(r);  _32 = _Type(0); _33 =  cos(r);  _34 = _Type(0);
			_41 = _Type(0); _42 = _Type(0); _43 = _Type(0); _44 = _Type(1);
			return *this;
		}
		/// Z軸の回転行列へ
		Matrix4x4 &rotZ(_Type r) {
			_11 =  cos(r);  _12 = -sin(r);  _13 = _Type(0); _14 = _Type(0);
			_21 =  sin(r);  _22 =  cos(r);  _23 = _Type(0); _24 = _Type(0);
			_31 = _Type(0); _32 = _Type(0); _33 = _Type(1); _34 = _Type(0);
			_41 = _Type(0); _42 = _Type(0); _43 = _Type(0); _44 = _Type(1);
			return *this;
		}
		/// 回転行列へ(ヨー、ピッチ、ロール)
		Matrix4x4 &rot(_Type yaw, _Type pitch, _Type roll) {
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
		Matrix4x4 &rot(const Vector3d<_Type> &v, _Type r) {
			_Type cos_r = cos(r);
			_Type sin_r = sin(r);
			_11 = v.x*v.x*(1-cos_r)+cos_r;     _12 = v.x*v.y*(1-cos_r)-v.z*sin_r; _13 = v.x*v.z*(1-cos_r)+v.y*sin_r; _14 = _Type(0);
			_21 = v.x*v.y*(1-cos_r)+v.z*sin_r; _22 = v.y*v.y*(1-cos_r)+cos_r;     _23 = v.y*v.z*(1-cos_r)-v.x*sin_r; _24 = _Type(0);
			_31 = v.x*v.z*(1-cos_r)-v.y*sin_r; _32 = v.y*v.z*(1-cos_r)+v.x*sin_r; _33 = v.z*v.z*(1-cos_r)+cos_r;     _34 = _Type(0);
			_41 = _Type(0);                    _42 = _Type(0);                    _43 = _Type(0);                    _44 = _Type(1);
			return *this;
		}
/*		/// 回転行列へ
		Matrix4x4 &rot(const Quaternion<_Type> &q) {
			//D3DXMatrix4x4RotationQuaternion(this, &q);
			return *this;
		}*/
		/// スケール行列へ
		Matrix4x4 &scale(_Type sx, _Type sy, _Type sz) {
			_11 = sx;       _12 = _Type(0); _13 = _Type(0); _14 = _Type(0);
			_21 = _Type(0); _22 = sy;       _23 = _Type(0); _24 = _Type(0);
			_31 = _Type(0); _32 = _Type(0); _33 = sz;       _34 = _Type(0);
			_41 = _Type(0); _42 = _Type(0); _43 = _Type(0); _44 = _Type(1);
			return *this;
		}
		/// スケール行列へ
		Matrix4x4 &scale(const Vector3d<_Type> &v) {
			return scale(v.x, v.y, v.z);
		}
		/// 平行移動行列へ
		Matrix4x4 &trans(_Type tx, _Type ty, _Type tz) {
			_11 = _Type(1); _12 = _Type(0); _13 = _Type(0); _14 = tx;
			_21 = _Type(0); _22 = _Type(1); _23 = _Type(0); _24 = ty;
			_31 = _Type(0); _32 = _Type(0); _33 = _Type(1); _34 = tz;
			_41 = _Type(0); _42 = _Type(0); _43 = _Type(0); _44 = _Type(1);
			return *this;
		}
		/// 平行移動行列へ
		Matrix4x4 &trans(const Vector3d<_Type> &v) {
			return trans(v.x, v.y, v.z);
		}

		/// ベクトル変換
		const Matrix4x4 &transform(Vector4d<_Type> &dst, const Vector4d<_Type> &src) const {
			dst.x = _11*_src.x+_12*_src.y+_13*_src.z+_14*_src.w;
			dst.y = _21*_src.x+_22*_src.y+_23*_src.z+_24*_src.w;
			dst.z = _31*_src.x+_32*_src.y+_33*_src.z+_34*_src.w;
			dst.w = _41*_src.x+_42*_src.y+_43*_src.z+_44*_src.w;
			return *this;
		}

		/// ベクトル変換（w=1として射影）
		const Matrix4x4 &transform(Vector3d<_Type> &dst, const Vector3d<_Type> &src) const {
			_Type invw = _Type(1)/(_41*src.x+_42*_src.y+_43*src.z+_44);
			dst.x = (_11*src.x+_12*_src.y+_13*src.z+_14)*invw;
			dst.x = (_21*src.x+_22*_src.y+_23*src.z+_24)*invw;
			dst.x = (_31*src.x+_32*_src.y+_33*src.z+_34)*invw;
			return *this;
		}

		/// ベクトル変換（w=0として変換）
		Vector3d<_Type> transformVector(const Vector3d<_Type> &rhs) const {
			return Vector3dC<_Type>(
				_11*rhs.x+_12*rhs.y+_13*rhs.z,
				_21*rhs.x+_22*rhs.y+_23*rhs.z,
				_31*rhs.x+_32*rhs.y+_33*rhs.z
			);
		}

		/// ベクトル変換（w=0として変換）
		const Matrix4x4 &transformVector(Vector3d<_Type> &dst, const Vector3d<_Type> &src) const {
			dst.x = _11*src.x+_12*src.y+_13*src.z;
			dst.y = _21*src.x+_22*src.y+_23*src.z;
			dst.z = _31*src.x+_32*src.y+_33*src.z;
			return *this;
		}

		/// 固有ベクトルから行列設定
		Matrix4x4 &setAxis(const Vector3d<_Type> &right, const Vector3d<_Type> &up, const Vector3d<_Type> &at, const Vector3d<_Type> &pos) {
			this->right() = right;
			this->up() = up;
			this->at() = at;
			position() = pos;
			_41 = _42 = _43 = _Type(0); _44 = _Type(1);
			return *this;
		}

		/// 平行移動を設定
		Matrix4x4 &setPos(_Type x, _Type y, _Type z) {
			_14 = x; _24 = y; _34 = z; _44 = _Type(1);
			return *this;
		}
		/// 平行移動を設定
		Matrix4x4 &setPos(const Vector3d<_Type> &v) {
			_14 = v.x; _24 = v.y; _34 = v.z; _44 = _Type(1);
			return *this;
		}

		/// ビュー行列をセットアップ
		Matrix4x4 &setViewLH(const Vector3d<_Type> &right, const Vector3d<_Type> &up, const Vector3d<_Type> &at, const Vector3d<_Type> &pos) {
			_11 = right.x; _12 = right.y; _13 = right.z; _14 = -(right*pos);
			_21 = up.x;    _22 = up.y;    _23 = up.z;    _24 = -(up*pos);
			_31 = at.x;    _32 = at.y;    _33 = at.z;    _34 = -(at*pos);
			_41 = _42 = _43 = _Type(0); _44 = _Type(1);
			return *this;
		}

		/// ビュー行列をセットアップ
		Matrix4x4 &setViewRH(const Vector3d<_Type> &right, const Vector3d<_Type> &up, const Vector3d<_Type> &at, const Vector3d<_Type> &pos) {
			_11 = right.x; _12 = right.y; _13 = right.z; _14 = right*pos;
			_21 = up.x;    _22 = up.y;    _23 = up.z;    _24 = up*pos;
			_31 = at.x;    _32 = at.y;    _33 = at.z;    _34 = at*pos;
			_14 = _24 = _34 = _Type(0); _44 = _Type(1);
			return *this;
		}

		/// ビュー行列をセットアップ
		Matrix4x4 &setView(const Vector3d<_Type> &right, const Vector3d<_Type> &up, const Vector3d<_Type> &at, const Vector3d<_Type> &pos) {
#ifdef GCTP_COORD_RH
			return setViewRH(right, up, at, pos);
#else
			return setViewLH(right, up, at, pos);
#endif
		}

		/// ビュー行列をセットアップ
		Matrix4x4 &setLookAtLH(const Vector3d<_Type> &vpos, const Vector3d<_Type> &vat, const Vector3d<_Type> &vup) {
			Vector3d<_Type> zaxis = (vat-vpos).normal();
			Vector3d<_Type> xaxis = (vup%zaxis).normal();
			Vector3d<_Type> yaxis = zaxis%xaxis;

			_11 = xaxis.x; _12 = xaxis.y; _13 = xaxis.z; _14 = -(xaxis*vpos);
			_21 = yaxis.x; _22 = yaxis.y; _23 = yaxis.z; _24 = -(yaxis*vpos);
			_31 = zaxis.x; _32 = zaxis.y; _33 = zaxis.z; _34 = -(zaxis*vpos);
			_41 = xaxis.x; _42 = xaxis.y; _43 = xaxis.z; _44 = _Type(1);
			return *this;
		}
		/// ビュー行列をセットアップ
		Matrix4x4 &setLookAtRH(const Vector3d<_Type> &vpos, const Vector3d<_Type> &vat, const Vector3d<_Type> &vup) {
			Vector3d<_Type> zaxis = (vpos-vat).normal();
			Vector3d<_Type> xaxis = (vup%zaxis).normal();
			Vector3d<_Type> yaxis = zaxis%xaxis;

			_11 = xaxis.x; _12 = xaxis.y; _13 = xaxis.z; _14 = -(xaxis*vpos);
			_21 = yaxis.x; _22 = yaxis.y; _23 = yaxis.z; _24 = -(yaxis*vpos);
			_31 = zaxis.x; _32 = zaxis.y; _33 = zaxis.z; _34 = -(zaxis*vpos);
			_41 = xaxis.x; _42 = xaxis.y; _43 = xaxis.z; _44 = _Type(1);
			return *this;
		}
		/// ビュー行列をセットアップ
		Matrix4x4 &setLookAt(const Vector3d<_Type> &vpos, const Vector3d<_Type> &vat, const Vector3d<_Type> &vup) {
#ifdef GCTP_COORD_RH
			return setLookAtRH(vpos, vat, vup);
#else
			return setLookAtLH(vpos, vat, vup);
#endif
		}
		/// 透視投影行列をセットアップ
		Matrix4x4 &setFOVLH(_Type fov, _Type aspect_ratio, _Type zn, _Type zf) {
			_22 = cot(fov/_Type(2));
			_11 = aspect_ratio/_22; _12 = _Type(0); _13 = _Type(0);   _14 = _Type(0);
			_21 = _Type(0);                         _23 = _Type(0);   _24 = _Type(0);
			_31 = _Type(0);         _32 = _Type(0); _33 = zf/(zf-zn); _34 = -zn*zf/(zf-zn);
			_41 = _Type(0);         _42 = _Type(0); _43 = _Type(1);   _44 = _Type(0);
			return *this;
		}
		/// 透視投影行列をセットアップ
		Matrix4x4 &setFOVRH(_Type fov, _Type aspect, _Type zn, _Type zf) {
			_22 = cot(fov/_Type(2));
			_11 = aspect_ratio/_22; _12 = _Type(0); _13 = _Type(0);   _14 = _Type(0);
			_21 = _Type(0);                         _23 = _Type(0);   _24 = _Type(0);
			_31 = _Type(0);         _32 = _Type(0); _33 = zf/(zn-zf); _34 = zn*zf/(zn-zf);
			_41 = _Type(0);         _42 = _Type(0); _43 = _Type(-1);  _44 = _Type(0);
			return *this;
		}
		/// 透視投影行列をセットアップ
		Matrix4x4 &setFOV(_Type fov, _Type aspect, _Type zn, _Type zf) {
#ifdef GCTP_COORD_RH
			return setFOVRH(fov, aspect, zn, zf);
#else
			return setFOVLH(fov, aspect, zn, zf);
#endif
		}
		/// 透視投影行列をセットアップ
		Matrix4x4 &setParsLH(_Type w, _Type h, _Type zn, _Type zf) {
			_11 = _Type(2)*zn/w; _12 = _Type(0);      _13 = _Type(0);   _14 = _Type(0);
			_21 = _Type(0);      _22 = _Type(2)*zn/h; _23 = _Type(0);   _24 = _Type(0);
			_31 = _Type(0);      _32 = _Type(0);      _33 = zf/(zf-zn); _34 = zn*zf/(zn-zf);
			_41 = _Type(0);      _42 = _Type(0);      _43 = _Type(1);   _44 = _Type(0);
			return *this;
		}
		/// 透視投影行列をセットアップ
		Matrix4x4 &setParsRH(_Type w, _Type h, _Type zn, _Type zf) {
			_11 = _Type(2)*zn/w; _12 = _Type(0);      _13 = _Type(0);   _14 = _Type(0);
			_21 = _Type(0);      _22 = _Type(2)*zn/h; _23 = _Type(0);   _24 = _Type(0);
			_31 = _Type(0);      _32 = _Type(0);      _33 = zf/(zn-zf); _34 = zn*zf/(zn-zf);
			_41 = _Type(0);      _42 = _Type(0);      _43 = _Type(-1);  _44 = _Type(0);
			return *this;
		}
		/// 透視投影行列をセットアップ
		Matrix4x4 &setPars(_Type w, _Type h, _Type zn, _Type zf) {
#ifdef GCTP_COORD_RH
			return setParsRH(w, h, zn, zf);
#else
			return setParsLH(w, h, zn, zf);
#endif
		}
		/** 透視投影行列をセットアップ
		 *
		 * l, t, r, bはスクリーンを
		 * (0, 0)-(1, 0)
		 *   |       |
		 * (0, 1)-(1, 1)
		 * と見立てたときのサブウィンドウのレクト
		 */
		Matrix4x4 &setFOVLH(_Type fov, _Type aspect, _Type l, _Type t, _Type r, _Type b, _Type zn, _Type zf) {
			_Type coef = zn*tanf(fov/2.0f);
			return setParsLH(coef*(2*l-1), coef*(2*r-1), coef/aspect*(2*t-1), coef/aspect*(2*b-1), zn, zf);
		}
		/** 透視投影行列をセットアップ
		 *
		 * l, t, r, bはスクリーンを
		 * (0, 0)-(1, 0)
		 *   |       |
		 * (0, 1)-(1, 1)
		 * と見立てたときのサブウィンドウのレクト
		 */
		Matrix4x4 &setFOVRH(_Type fov, _Type aspect, _Type l, _Type t, _Type r, _Type b, _Type zn, _Type zf) {
			_Type coef = zn*tanf(fov/2.0f);
			return setParsRH(coef*(2*l-1), coef*(2*r-1), coef/aspect*(2*t-1), coef/aspect*(2*b-1), zn, zf);
		}
		/** 透視投影行列をセットアップ
		 *
		 * l, t, r, bはスクリーンを
		 * (0, 0)-(1, 0)
		 *   |       |
		 * (0, 1)-(1, 1)
		 * と見立てたときのサブウィンドウのレクト
		 */
		Matrix4x4 &setFOV(_Type fov, _Type aspect, _Type l, _Type t, _Type r, _Type b, _Type zn, _Type zf) {
#ifdef GCTP_COORD_RH
			return setFOVRH(fov, aspect, l, t, r, b, zn, zf);
#else
			return setFOVLH(fov, aspect, l, t, r, b, zn, zf);
#endif
		}
		/// 透視投影行列をセットアップ
		Matrix4x4 &setParsLH(_Type l, _Type r, _Type b, _Type t, _Type zn, _Type zf) {
			_11 = _Type(2)*zn/(r-l); _12 = _Type(0);          _13 = (l+r)/(l-r); _14 = _Type(0);
			_21 = _Type(0);          _22 = _Type(2)*zn/(t-b); _23 = (t+b)/(b-t); _24 = _Type(0);
			_31 = _Type(0);          _32 = _Type(0);          _33 = zf/(zf-zn);  _34 = zn*zf/(zn-zf);
			_41 = _Type(0);          _42 = _Type(0);          _43 = _Type(1);    _44 = _Type(0);
			return *this;
		}
		/// 透視投影行列をセットアップ
		Matrix4x4 &setParsRH(_Type minx, _Type maxx, _Type miny, _Type maxy, _Type zn, _Type zf) {
			_11 = _Type(2)*zn/(r-l); _12 = _Type(0);          _13 = (l+r)/(r-l); _14 = _Type(0);
			_21 = _Type(0);          _22 = _Type(2)*zn/(t-b); _23 = (t+b)/(t-b); _24 = _Type(0);
			_31 = _Type(0);          _32 = _Type(0);          _33 = zf/(zn-zf);  _34 = zn*zf/(zn-zf);
			_41 = _Type(0);          _42 = _Type(0);          _43 = _Type(-1);   _44 = _Type(0);
			return *this;
		}
		/// 透視投影行列をセットアップ
		Matrix4x4 &setPars(_Type minx, _Type maxx, _Type miny, _Type maxy, _Type zn, _Type zf) {
#ifdef GCTP_COORD_RH
			return setParsRH(minx, maxx, miny, maxy, zn, zf);
#else
			return setParsLH(minx, maxx, miny, maxy, zn, zf);
#endif
		}
		/// 平行投影行列をセットアップ
		Matrix4x4 &setOrthoRH(_Type w, _Type h, _Type zn, _Type zf) {
			_11 = _Type(2)/w; _12 = _Type(0);   _13 = _Type(0);         _14 = _Type(0);
			_21 = _Type(0);   _22 = _Type(2)/h; _23 = _Type(0);         _24 = _Type(0);
			_31 = _Type(0);   _32 = _Type(0);   _33 = _Type(1)/(zf-zn); _34 = zn/(zn-zf);
			_41 = _Type(0);   _42 = _Type(0);   _43 = _Type(0);         _44 = _Type(1);
			return *this;
		}
		/// 平行投影行列をセットアップ
		Matrix4x4 &setOrthoLH(_Type w, _Type h, _Type zn, _Type zf) {
			_11 = _Type(2)/w; _12 = _Type(0);   _13 = _Type(0);         _14 = _Type(0);
			_21 = _Type(0);   _22 = _Type(2)/h; _23 = _Type(0);         _24 = _Type(0);
			_31 = _Type(0);   _32 = _Type(0);   _33 = _Type(1)/(zn-zf); _34 = zn/(zn-zf);
			_41 = _Type(0);   _42 = _Type(0);   _43 = _Type(0);         _44 = _Type(1);
			return *this;
		}
		/// 平行投影行列をセットアップ
		Matrix4x4 &setOrtho(_Type w, _Type h, _Type zn, _Type zf) {
#ifdef GCTP_COORD_RH
			return setOrthoRH(w, h, zn, zf);
#else
			return setOrthoLH(w, h, zn, zf);
#endif
		}
		/// 平行投影行列をセットアップ
		Matrix4x4 &setOrthoRH(_Type l, _Type r, _Type b, _Type t, _Type zn, _Type zf) {
			_11 = _Type(2)/(r-l); _12 = _Type(0);       _13 = _Type(0);         _14 = (l+r)/(l-r);
			_21 = _Type(0);       _22 = _Type(2)/(t-b); _23 = _Type(0);         _24 = (t+b)/(b-t);
			_31 = _Type(0);       _32 = _Type(0);       _33 = _Type(1)/(zf-zn); _34 = zn/(zn-zf);
			_41 = _Type(0);       _42 = _Type(0);       _43 = _Type(0);         _44 = _Type(1);
			return *this;
		}
		/// 平行投影行列をセットアップ
		Matrix4x4 &setOrthoLH(_Type l, _Type r, _Type b, _Type t, _Type zn, _Type zf) {
			_11 = _Type(2)/(r-l); _12 = _Type(0);       _13 = _Type(0);         _14 = (l+r)/(l-r);
			_21 = _Type(0);       _22 = _Type(2)/(t-b); _23 = _Type(0);         _24 = (t+b)/(b-t);
			_31 = _Type(0);       _32 = _Type(0);       _33 = _Type(1)/(zn-zf); _34 = zn/(zn-zf);
			_41 = _Type(0);       _42 = _Type(0);       _43 = _Type(0);         _44 = _Type(1);
			return *this;
		}
		/// 平行投影行列をセットアップ
		Matrix4x4 &setOrtho(_Type minx, _Type maxx, _Type miny, _Type maxy, _Type zn, _Type zf) {
#ifdef GCTP_COORD_RH
			return setOrthoRH(minx, maxx, miny, maxy, zn, zf);
#else
			return setOrthoLH(minx, maxx, miny, maxy, zn, zf);
#endif
		}
		/// 線形補間した値をセット
		Matrix4x4 &set2PInterpolation(const Matrix4x4& lhs, const Matrix4x4& rhs, _Type t) {
			(*this) = lhs + (rhs - lhs)*t;
			return *this;
		}

		/// Catmull-Rom
		static _Type getCatmullRom(_Type x0, _Type x1, _Type x2, _Type x3, _Type t)
		{
			static const Matrix4x4 m = {
				-_Type(1)/_Type(2),  _Type(3)/_Type(2), -_Type(3)/_Type(2),  _Type(1)/_Type(2),
				 _Type(2)/_Type(2), -_Type(5)/_Type(2),  _Type(4)/_Type(2), -_Type(1)/_Type(2),
				-_Type(1)/_Type(2),  _Type(0),           _Type(1)/_Type(2),  _Type(0),
				 _Type(0),           _Type(1),           _Type(0),           _Type(0)
			};
			_Type t2 = t*t;
			Vector4d<_Type> vt = {t2*t, t2, t, _Type(1)};
			vt = m * vt;
			return x0 * vt.x + x1 * vt.y + x2 * vt.z + x3 * vt.w;
		}

		/// ４点で補間した値をセット
		Matrix4x4 &set4PInterpolation(const Matrix4x4& prev, const Matrix4x4& begin, const Matrix4x4& end, const Matrix4x4& next, _Type t)
		{
			for(int i = 0; i < 4; i++) {
				for(int j = 0; j < 4; j++) {
					m[i][j] = getCatmullRom(prev.m[i][j], begin.m[i][j], end.m[i][j], next.m[i][j], t);
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
			return _11 != _Type(1) || _22 != _Type(1) || _33 != _Type(1) || _44 != _Type(1)
				|| _12 != _Type(0) || _13 != _Type(0) || _14 != _Type(0)
				|| _21 != _Type(0) || _23 != _Type(0) || _24 != _Type(0)
				|| _31 != _Type(0) || _32 != _Type(0) || _34 != _Type(0)
				|| _41 != _Type(0) || _42 != _Type(0) || _43 != _Type(0);
		}
		
// D3DXライブラリサポート
#ifdef __D3DX9MATH_H__
		operator const D3DXMATRIX &() const
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return *reinterpret_cast<const D3DXMATRIX *>(this);
		}
		operator D3DXMATRIX &()
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return *reinterpret_cast<D3DXMATRIX *>(this);
		}
		operator const D3DXMATRIX *() const
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return reinterpret_cast<const D3DXMATRIX *>(this);
		}
		operator D3DXMATRIX *()
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return reinterpret_cast<D3DXMATRIX *>(this);
		}
#endif
#ifdef D3DMATRIX_DEFINED
		operator const D3DMATRIX &() const
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return *reinterpret_cast<const D3DMATRIX *>(this);
		}
		operator D3DMATRIX &()
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return *reinterpret_cast<D3DMATRIX *>(this);
		}
		operator const D3DMATRIX *() const
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return reinterpret_cast<const D3DMATRIX *>(this);
		}
		operator D3DMATRIX *()
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return reinterpret_cast<D3DMATRIX *>(this);
		}
#endif
	};

	template<class E, class T, class _Type> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, Matrix4x4<_Type> const & m)
	{
		os<<"[";
		for(int i = 0; i < 4; i++) {
			os<<"{";
			for(int j = 0; j < 4; j++) {
				os<<m.m[i][j];
				if(j<3) os<<",";
			}
			os<<"}";
		}
		os<<"]";
		return os;
	}

	/// コンストラクタ付版
	template<typename _Type>
	struct Matrix4x4C : Matrix4x4<_Type> {
		/// コンストラクタ
		explicit Matrix4x4C(bool ident)
		{
			if(ident) identify();
			else {
				_11 = _12 = _13 = _14 =
				_21 = _22 = _23 = _24 =
				_31 = _32 = _33 = _34 =
				_41 = _42 = _43 = _44 = _Type(0);
			}
		}
		/// コンストラクタ
		Matrix4x4C(const Vector3d<_Type> &right, const Vector3d<_Type> &up, const Vector3d<_Type> &at, const Vector3d<_Type> &pos)
		{
			set(right, up, at, pos);
		}
		/// コンストラクタ
		Matrix4x4C(
			_Type _11, _Type _12, _Type _13, _Type _14,
			_Type _21, _Type _22, _Type _23, _Type _24,
			_Type _31, _Type _32, _Type _33, _Type _34,
			_Type _41, _Type _42, _Type _43, _Type _44
		)
		{
			set(
				_11, _12, _13, _14,
				_21, _22, _23, _24,
				_31, _32, _33, _34,
				_41, _42, _43, _44
			);
		}

// D3DXライブラリサポート
#ifdef __D3DX9MATH_H__
		Matrix4x4C(const D3DXMATRIX &src)
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			memcpy(this, &src, sizeof(*this));
		}
#endif
#ifdef D3DMATRIX_DEFINED
		Matrix4x4C(const D3DMATRIX &src)
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			memcpy(this, &src, sizeof(*this));
		}
#endif
	};

}} // namespace gctp

// D3DXライブラリサポート
#ifdef GCTP_USE_D3DXMATH
#include <gctp/math/d3d/matrix4x4.inl>
#endif

#endif //_GCTP_MATH_MATRIX4X4_HPP_
