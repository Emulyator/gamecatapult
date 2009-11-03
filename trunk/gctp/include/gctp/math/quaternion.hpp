#ifndef _GCTP_MATH_QUATERNION_HPP_
#define _GCTP_MATH_QUATERNION_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult クォータニオンクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: quat.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include <gctp/def.hpp>
#include <gctp/math/vector3d.hpp>
#include <gctp/math/vector4d.hpp>
#include <gctp/math/matrix4x4.hpp>
#include <gctp/math/matrix3x4.hpp>
#include <gctp/math/matrix3x3.hpp>

namespace gctp { namespace math {

	/** 四元数・クォータニオン
	 *
	 * PODとなるよう、コンストラクタが無い
	 *
	 */
	template<typename _Type>
	struct Quaternion {
		_Type x, y, z, w;

		Quaternion &set(const Vector3d<_Type> &src)
		{
			w = 0; x = src.x; y = src.y; z = src.z;
			return *this;
		}
		Quaternion &set(_Type w, const Vector3d<_Type> &v)
		{
			w = w; x = v.x; y = v.y; z = v.z;
			return *this;
		}
		Quaternion &set(const _Type *pf)
		{
			w = pf[0]; x = pf[1]; y = pf[2]; z = pf[3];
			return *this;
		}
		Quaternion &set(_Type w, _Type x, _Type y, _Type z)
		{
			this->w = w; this->x = x; this->y = y; this->z = z;
			return *this;
		}
		/// 値セット
		template<typename _Type2>
		Quaternion &set(const Quaternion<_Type2> &src)
		{
			w = _Type(src.w); x = _Type(src.x); y = _Type(src.y); z = _Type(src.z);
			return *this;
		}

		/// 指定軸周りの回転を設定
		Quaternion &set(const Vector3d<_Type> &axis, _Type rad)
		{
			_Type r = rad/_Type(2);
			_Type sin_r = sin(r);
			w = cos(r);
			x = axis.x * sin_r;
			y = axis.y * sin_r;
			z = axis.z * sin_r;
			return *this;
		}
		/// 回転行列から回転を設定
		Quaternion &set(const Matrix4x4<_Type> &mat)
		{
			_Type s;
			_Type tr = mat._11 + mat._22 + mat._33;
			if(tr >= _Type(0)) {
				s = sqrt(tr + _Type(1));
				w = s*_Type(0.5);
				s = _Type(0.5)/s;
				x = (mat._32 - mat._23)*s;
				y = (mat._13 - mat._31)*s;
				z = (mat._21 - mat._12)*s;
			}
			else {
				_Type maxm = (std::max)((std::max)(mat._11, mat._22), mat._33);
				if(maxm == mat._11) {
					s = sqrt(mat._11 - (mat._22 + mat._33) + _Type(1));
					x = s*_Type(0.5);
					s = _Type(0.5)/s;
					y = (mat._12 + mat._21)*s;
					z = (mat._31 + mat._13)*s;
					w = (mat._32 - mat._23)*s;
				}
				else if(maxm == mat._22) {
					s = sqrt(mat._22 - (mat._33 + mat._11) + _Type(1));
					y = s*_Type(0.5);
					s = _Type(0.5)/s;
					z = (mat._23 + mat._32)*s;
					x = (mat._12 + mat._21)*s;
					w = (mat._13 - mat._31)*s;
				}
				else {
					s = sqrt(mat._33 - (mat._11 + mat._22) + _Type(1));
					z = s*_Type(0.5);
					s = _Type(0.5)/s;
					x = (mat._31 + mat._13)*s;
					y = (mat._23 + mat._32)*s;
					w = (mat._21 - mat._12)*s;
				}
			}
			return *this;
		}
		/// 回転行列から回転を設定
		Quaternion &set(const Matrix3x3<_Type> &mat)
		{
			_Type s;
			_Type tr = mat._11 + mat._22 + mat._33;
			if(tr >= _Type(0)) {
				s = sqrt(tr + _Type(1));
				w = s*_Type(0.5);
				s = _Type(0.5)/s;
				x = (mat._32 - mat._23)*s;
				y = (mat._13 - mat._31)*s;
				z = (mat._21 - mat._12)*s;
			}
			else {
				_Type maxm = max(max(mat._11, mat._22), mat._33);
				if(maxm == mat._11) {
					s = sqrt(mat._11 - (mat._22 + mat._33) + _Type(1));
					x = s*_Type(0.5);
					s = _Type(0.5)/s;
					y = (mat._12 + mat._21)*s;
					z = (mat._31 + mat._13)*s;
					w = (mat._32 - mat._23)*s;
				}
				else if(maxm == mat._22) {
					s = sqrt(mat._22 - (mat._33 + mat._11) + _Type(1));
					y = s*_Type(0.5);
					s = _Type(0.5)/s;
					z = (mat._23 + mat._32)*s;
					x = (mat._12 + mat._21)*s;
					w = (mat._13 - mat._31)*s;
				}
				else {
					s = sqrt(mat._33 - (mat._11 + mat._22) + _Type(1));
					z = s*_Type(0.5);
					s = _Type(0.5)/s;
					x = (mat._31 + mat._13)*s;
					y = (mat._23 + mat._32)*s;
					w = (mat._21 - mat._12)*s;
				}
			}
			return *this;
		}
		/// ヨー・ピッチ・ロールで回転を設定
		Quaternion &set(_Type yaw, _Type pitch, _Type roll)
		{
			yaw *= _Type(0.5);
			pitch *= _Type(0.5);
			roll *= _Type(0.5);
			_Type cos_yaw = cos(yaw);
			_Type sin_yaw = sin(yaw);
			_Type cos_pitch = cos(pitch);
			_Type sin_pitch = sin(pitch);
			_Type cos_roll = cos(roll);
			_Type sin_roll = sin(roll);
			w = cos_roll * cos_pitch * cos_yaw + sin_roll * sin_pitch * sin_yaw;
			x = cos_roll * sin_pitch * cos_yaw + sin_roll * cos_pitch * sin_yaw;
			y = cos_roll * cos_pitch * sin_yaw - sin_roll * sin_pitch * cos_yaw;
			z = sin_roll * cos_pitch * cos_yaw - cos_roll * sin_pitch * sin_yaw;
			return *this;
		}

		/// ヨー・ピッチ・ロールで回転を設定
		Quaternion &rot(_Type yaw, _Type pitch, _Type roll)
		{
			return set(yaw, pitch, roll);
		}

		/// X軸回転を設定
		Quaternion &rotX(_Type theta)
		{
			theta *= _Type(0.5);
			return set(cos(theta), sin(theta), 0, 0);
		}

		/// Y軸回転を設定
		Quaternion &rotY(_Type theta)
		{
			theta *= _Type(0.5);
			return set(cos(theta), 0, sin(theta), 0);
		}

		/// Z軸回転を設定
		Quaternion &rotZ(_Type theta)
		{
			theta *= _Type(0.5);
			return set(cos(theta), 0, 0, sin(theta));
		}

		/// 姿勢を定義する二つのベクトルから四元数を設定
		Quaternion &set(const Vector3d<_Type> &forward, const Vector3d<_Type> &up)
		{
			Vector3d<_Type> right = forward % up;
#ifdef GCTP_COORD_DX
			return set(Matrix3x3C<_Type>(right, right%forward, forward));
#else
			return set(Matrix3x3C<_Type>(right, right%forward, -forward));
#endif
		}

		// 代入演算子を外部にさらす
		Quaternion &operator+=(const Quaternion &src) {
			w += src.w;
			x += src.x;
			y += src.y;
			z += src.z;
			return *this;
		}
		Quaternion &operator-=(const Quaternion &src) {
			w -= src.w;
			x -= src.x;
			y -= src.y;
			z -= src.z;
			return *this;
		}
		Quaternion &operator*=(const Quaternion &src) {
			return *this = *this*src;
		}
		Quaternion &operator*=(_Type src) {
			w *= src;
			x *= src;
			y *= src;
			z *= src;
			return *this;
		}
		Quaternion &operator/=(_Type src) {
			return *this *= _Type(1)/src;
		}
		
		Quaternion operator + () const { return *this; }
		Quaternion operator - () const { return QuaternionC<_Type>(-w, -x, -y, -z); }
		Quaternion operator+(const Quaternion &rhs) const { return Quaternion(*this)+=rhs; }
		Quaternion operator-(const Quaternion &rhs) const { return Quaternion(*this)-=rhs; }
		Quaternion operator*(const Quaternion &rhs) const {
			return QuaternionC<_Type>(
				w*rhs.w - x*rhs.x - y*rhs.y - z*rhs.z,
				w*rhs.x + x*rhs.w + y*rhs.z - z*rhs.y,
				w*rhs.y + y*rhs.w + z*rhs.x - x*rhs.z,
				w*rhs.z + z*rhs.w + x*rhs.y - y*rhs.x
			);
		}
		Quaternion operator*(_Type rhs) const { return Quaternion(*this)*=rhs; }
		Quaternion operator/(_Type rhs) const { return Quaternion(*this)/=rhs; }
		friend Quaternion operator*(_Type lhs, const Quaternion &rhs) { return rhs*lhs; }
		friend Quaternion operator/(_Type lhs, const Quaternion &rhs) { return rhs/lhs; }

		/// ベクトル部を返す
		Vector3d<_Type> &vector() { return *reinterpret_cast<Vector3d<_Type> *>(&x); }
		/// ベクトル部を返す
		const Vector3d<_Type> &vector() const { return *reinterpret_cast<const Vector3d<_Type> *>(&x); }

#if 0
		// これ全然間違ってる

		/// ヨーを返す(正規化されてること)
		_Type yaw() const
		{
			return atan2(2*(y*z + w*x), w*w - x*x - y*y + z*z);
		}
		/// ピッチを返す(正規化されてること)
		_Type pitch() const
		{
			return asin(-2*(x*z - w*y));
		}
		/// ロールを返す(正規化されてること)
		_Type roll() const
		{
			return atan2(2*(x*y + w*z), w*w + x*x - y*y - z*z);
		}
#endif

		/// 回転軸と回転角を返す(正規化されてること)
		_Type toAxisAngle(Vector3d<_Type> &axis_out) const
		{
			_Type invscale = _Type(1) / vector().length();
			axis_out.x = x * invscale;
			axis_out.y = y * invscale;
			axis_out.z = z * invscale;
			return _Type(2) * acos(w);
		}
		/// 行列を返す(正規化されてること)
		Matrix4x4<_Type> toMatrix4x4() const
		{
			_Type d = norm2();
			assert(d != _Type(0));
			_Type s = _Type(2) / d;
			_Type xs = x * s,   ys = y * s,   zs = z * s;
			_Type wx = w * xs,  wy = w * ys,  wz = w * zs;
			_Type xx = x * xs,  xy = x * ys,  xz = x * zs;
			_Type yy = y * ys,  yz = y * zs,  zz = z * zs;
			return Matrix4x4C<_Type>(
				_Type(1) - (yy + zz),             xy - wz,              xz + wy,  _Type(0),
				            xy + wz,  _Type(1) - (xx + zz),             yz - wx,  _Type(0),
				            xz - wy,              yz + wx,  _Type(1) - (xx + yy), _Type(0),
				_Type(0),             _Type(0),             _Type(0),             _Type(1)
			);
		}
		/// 行列を返す(正規化されてること)
		Matrix3x4<_Type> toMatrix3x4() const
		{
			_Type d = norm2();
			assert(d != _Type(0));
			_Type s = _Type(2) / d;
			_Type xs = x * s,   ys = y * s,   zs = z * s;
			_Type wx = w * xs,  wy = w * ys,  wz = w * zs;
			_Type xx = x * xs,  xy = x * ys,  xz = x * zs;
			_Type yy = y * ys,  yz = y * zs,  zz = z * zs;
			return Matrix3x4C<_Type>(
				_Type(1) - (yy + zz),             xy - wz,              xz + wy,  _Type(0),
				            xy + wz,  _Type(1) - (xx + zz),             yz - wx,  _Type(0),
				            xz - wy,              yz + wx,  _Type(1) - (xx + yy), _Type(0)
			);
		}
		/// 行列を返す(正規化されてること)
		Matrix3x3<_Type> toMatrix3x3() const
		{
			_Type d = norm2();
			assert(d != _Type(0));
			_Type s = _Type(2) / d;
			_Type xs = x * s,   ys = y * s,   zs = z * s;
			_Type wx = w * xs,  wy = w * ys,  wz = w * zs;
			_Type xx = x * xs,  xy = x * ys,  xz = x * zs;
			_Type yy = y * ys,  yz = y * zs,  zz = z * zs;
			return Matrix3x4C<_Type>(
				_Type(1) - (yy + zz),             xy - wz,              xz + wy,
				            xy + wz,  _Type(1) - (xx + zz),             yz - wx,
				            xz - wy,              yz + wx,  _Type(1) - (xx + yy)
			);
		}
		/// 行列を返す(正規化されてること)
		Matrix4x4<_Type> toMatrix() const
		{
			return toMatrix4x4();
		}

		/// 単位クォータニオンに
		Quaternion &identify()
		{
			w = _Type(1); x = y = z = _Type(0);
			return *this;
		}
		/// 正規化
		Quaternion &normalize() {
			return *this /= norm();
		}
		/// 正規クォータニオンを返す
		Quaternion normal() const {
			return Quaternion(*this).normalize();
		}
		/// 共役化
		Quaternion &conjugate() {
			x = -x; y = -y; z = -z;
			return *this;
		}
		/// 共役を返す
		Quaternion conjugation() const {
			return Quaternion(*this).conjugate();
		}
		/// 共役を返す
		Quaternion operator~() const {
			return Quaternion(*this).conjugate();
		}
		/// 逆クォータニオン化
		Quaternion &invert() {
			return conjugate().normalize();
		}
		/// 逆クォータニオンを返す
		Quaternion inverse() const {
			return Quaternion(*this).invert();
		}

		/// 最小弧を設定
		Quaternion &setSmallestArc(const Vector3d<_Type> &from, const Vector3d<_Type> &to) {
			_Type s = sqrtf((1+from*to)*2);
			vector() = (from%to)/s; w = s/2.0f;
			return *this;
		}
		/// 最小弧を返す
		static Quaternion smallestArc(const Vector3d<_Type> &lhs, const Vector3d<_Type> &rhs) {
			return Quaternion().setSmallestArc(lhs, rhs);
		}

		/// 最小弧を設定(入力ベクトルが正規化済み)
		Quaternion &setSmallestArcFast(const Vector3d<_Type> &from, const Vector3d<_Type> &to) {
			w = from*to; vector() = from%to;
			return *this;
		}
		/// 最小弧を返す
		static Quaternion smallestArcFast(const Vector3d<_Type> &lhs, const Vector3d<_Type> &rhs) {
			return Quaternion().setSmallestArcFast(lhs, rhs);
		}

		/// 内積値を返す
		static _Type dot(const Quaternion &lhs, const Quaternion &rhs) {
			return lhs.w*rhs.w + lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z;
		}

		/// ノルムの二乗
		_Type norm2() const {
			return w*w+x*x+y*y+z*z;
		}
		/// ノルム
		_Type norm() const {
			return sqrt(norm2());
		}

		/// ベクトル変換
		Vector3d<_Type> transform(const Vector3d<_Type> &src) const {
			return ((*this)*QuaternionC<_Type>(src)*~(*this)).vector();
		}

		/// ベクトル変換
		const Quaternion &transform(Vector3d<_Type> &dst, const Vector3d<_Type> &src) const {
			dst = ((*this)*QuaternionC<_Type>(src)*~(*this)).vector();
			return *this;
		}

		/** @name 補間系
			@{ */
		/// 球状線形補間した値をセット
		Quaternion &setSlerp(const Quaternion &lhs, const Quaternion &rhs, _Type t) {
			_Type qr = dot(lhs, rhs);
			if(qr > _Type(1)) qr = _Type(1);
			else if(qr < _Type(-1)) qr = _Type(-1);
			_Type ss = _Type(1) - qr * qr;
			if(abs(ss) > _Type(g_epsilon)) {
				_Type ph = acos(qr);
				if(abs(ph) > _Type(g_epsilon)) {
					_Type sp = sqrt(ss);
					_Type pt = ph * t;
					_Type t1 = sin(pt) / sp;
					_Type t0 = sin(ph - pt) / sp;
					return *this = lhs * t0 + rhs * t1;
				}
			}
			return *this = lhs;
		}
		/// 球状平方補間した値をセット
		Quaternion &setSquad(const Quaternion &q, const Quaternion &a, const Quaternion &b, const Quaternion &c, _Type t) {
			setSlerp(Quaternion().setSlerp(q, c, t), Quaternion().setSlerp(a, b, t), _Type(2)*t*(_Type(1) - t));
			return *this;
		}
		/// Squadに渡すための制御点を求める
		static void getSquadQuaternions(Quaternion &a, Quaternion &b, Quaternion &c, const Quaternion &prev, const Quaternion &begin, const Quaternion &end, const Quaternion &next) {
			Quaternion q0 = (prev+begin).norm() < (prev-begin).norm() ? -prev : prev;
			c = (begin+end).norm() < (begin-end).norm() ? -end : end;
			Quaternion q3 = (end+next).norm() < (end-next).norm() ? -next : next;
			a = c * exp(-_Type(1)/_Type(4)*log(exp(begin)*c)+log(exp(begin)*q0));
			b = c * exp(-_Type(1)/_Type(4)*log(exp(c)*q3)+log(exp(c)*begin));
		}

		/// スプライン値を取得
		static Quaternion getSpline(const Quaternion& prev, const Quaternion& now, const Quaternion& next) {
			return now*exp(-0.25f*(log((~now)*next)+log((~now)*prev)));
		}

		Quaternion &set2PInterpolation(const Quaternion& lhs, const Quaternion& rhs, _Type t) {
			return setSlerp(lhs, rhs, t);
		}
		Quaternion &set4PInterpolation(const Quaternion& prev, const Quaternion& begin, const Quaternion& end, const Quaternion& next, _Type t) {
			Quaternion a, b, c;
			getSquadQuaternions(a, b, c, prev, begin, end, next);
			return setSquad(begin, a, b, c, t);
		}
		/** @} */
		
// D3DXライブラリサポート
#ifdef __D3DX9MATH_H__
		operator const D3DXQUATERNION &() const
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return *reinterpret_cast<const D3DXQUATERNION *>(this);
		}
		operator D3DXQUATERNION &()
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return *reinterpret_cast<D3DXQUATERNION *>(this);
		}
		operator const D3DXQUATERNION *() const
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return reinterpret_cast<const D3DXQUATERNION *>(this);
		}
		operator D3DXQUATERNION *()
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return reinterpret_cast<D3DXQUATERNION *>(this);
		}
#endif
	};

	/// 自然対数を返す
	template<class _Type>
	inline Quaternion<_Type> log(const Quaternion<_Type> &rhs) {
		return QuaternionC<_Type>(_Type(0), rhs.vector()*(_Type(2)*acos(rhs.w)));
	}

	/// 指数を返す
	template<class _Type>
	inline Quaternion<_Type> exp(const Quaternion<_Type> &rhs) {
		_Type theta = _Type(2)*acos(rhs.w);
		return QuaternionC<_Type>(cos(theta), rhs.vector()*sin(theta));
	}

	template<class E, class T, class _Type> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, Quaternion<_Type> const & q)
	{
		os<<"("<<q.w<<";"<<q.x<<","<<q.y<<","<<q.z<<")";
		return os;
	}

	/// コンストラクタ付版
	template<typename _Type>
	struct QuaternionC : Quaternion<_Type> {
		QuaternionC() {}
		explicit QuaternionC(bool ident) { if(ident) identify(); else { x = y = z = w = _Type(0); } }
		explicit QuaternionC(const Vector3d<_Type> &src) { set(src); }
		explicit QuaternionC(const Matrix4x4<_Type> &mat) { set(mat); }
		explicit QuaternionC(const Matrix3x3<_Type> &mat) { set(mat); }
		QuaternionC(_Type w, const Vector3d<_Type> &v) { set(w, v); }
		QuaternionC(const _Type *pf) { set(pf); }
		QuaternionC(_Type yaw, _Type pitch, _Type roll) { set(yaw, pitch, roll); }
		QuaternionC(_Type w, _Type x, _Type y, _Type z) { set( w, x, y, z ); }
		QuaternionC(const Vector3d<_Type> &axis, _Type rad) { set(axis, rad); }

// D3DXライブラリサポート
#ifdef __D3DX9MATH_H__
		QuaternionC(const D3DXQUATERNION &src)
		{
			set(src.w, src.x, src.y, src.z);
		}
		operator const D3DXQUATERNION &() const
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return *reinterpret_cast<const D3DXQUATERNION *>(this);
		}
		operator D3DXQUATERNION &()
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return *reinterpret_cast<D3DXQUATERNION *>(this);
		}
		operator const D3DXQUATERNION *() const
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return reinterpret_cast<const D3DXQUATERNION *>(this);
		}
		operator D3DXQUATERNION *()
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return reinterpret_cast<D3DXQUATERNION *>(this);
		}
#endif
	};

}} // namespace gctp::math

// D3DXライブラリを使用する
#ifdef GCTP_USE_D3DXMATH
#include <gctp/math/d3d/quaternion.inl>
#endif

#endif //_GCTP_MATH_QUATERNION_HPP_
