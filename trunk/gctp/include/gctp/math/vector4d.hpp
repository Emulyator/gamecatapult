#ifndef _GCTP_MATH_VECTOR4_HPP_
#define _GCTP_MATH_VECTOR4_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult �x�N�g���N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: vector.hpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include <iosfwd>               // for std::basic_ostream
#include <gctp/def.hpp>

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <assert.h>
#include <cmath>
#include <limits>
#ifdef GCTP_USE_D3DXMATH
# include <d3dx9math.h>
#endif

namespace gctp { namespace math {

	/** �S�����x�N�g��/�l�����N���X
	 *
	 */
	template<typename _Type>
	struct Vector4d {
		_Type x, y, z, w;
		
		/// �l�Z�b�g
		Vector4d &set(const _Type *p)
		{
			x = p[0]; y = p[1]; z = p[2]; w = p[3];
			return *this;
		}
		/// �l�Z�b�g
		Vector4d &set(_Type x, _Type y, _Type z, _Type w)
		{
			this->x = x; this->y = y; this->z = z; this->w = w;
			return *this;
		}
		/// �l�Z�b�g
		Vector4d &set(const Vector3d<_Type> &src, _Type w = 1)
		{
			x = src.x; y = src.y; z = src.z; this->w = w;
			return *this;
		}
		/// �l�Z�b�g
		template<typename _Type2>
		Vector4d &set(const Vector4d<_Type2> &src)
		{
			x = _Type(src.x); y = _Type(src.y); z = _Type(src.z); w = _Type(src.w);
			return *this;
		}

		// ������Z�q
		Vector4d &operator+=(const Vector4d &src) {
			x += src.x; y += src.y; z += src.z; w += src.w;
			return *this;
		}
		// ������Z�q
		Vector4d &operator-=(const Vector4d &src) {
			x -= src.x; y -= src.y; z -= src.z; w -= src.w;
			return *this;
		}
		// ������Z�q
		Vector4d &operator*=(const Vector4d &src) {
			x *= src.x; y *= src.y; z *= src.z; w *= src.w;
			return *this;
		}
		// ������Z�q
		Vector4d &operator/=(const Vector4d &src) {
			x /= src.x; y /= src.y; z /= src.z; w /= src.w;
			return *this;
		}
		// ������Z�q
		Vector4d &operator*=(_Type src) {
			x = _Type(x*src); y = _Type(y*src); z = _Type(z*src); w = _Type(w*src);
			return *this;
		}
		// ������Z�q
		Vector4d &operator/=(_Type src) {
			x = _Type(x/src); y = _Type(y/src); z = _Type(z/src); w = _Type(w/src);
			return *this;
		}
		/// ���
		Vector4d &operator|=(const Vector4d &src) {
			if(x < src.x) x = src.x;
			if(y < src.y) y = src.y;
			if(z < src.z) z = src.z;
			if(w < src.w) w = src.w;
			return *this;
		}
		/// ���
		Vector4d &operator&=(const Vector4d &src) {
			if(x > src.x) x = src.x;
			if(y > src.y) y = src.y;
			if(z > src.z) z = src.z;
			if(w > src.w) w = src.w;
			return *this;
		}

		Vector4d operator + () const { return *this; }
		Vector4d operator - () const { return (Vector4d){-x, -y, -z, -w}; }
		Vector4d operator+(const Vector4d &rhs) const { return Vector4dC<_Type>(*this)+=rhs; }
		Vector4d operator-(const Vector4d &rhs) const { return Vector4dC<_Type>(*this)-=rhs; }
		friend Vector4d mult(const Vector4d &lhs, const Vector4d &rhs) { return Vector4d(lhs)*=rhs; }
		Vector4d operator/(const Vector4d &rhs) const { return Vector4dC<_Type>(*this)/=rhs; }
		Vector4d operator*(_Type rhs) const { return Vector4dC<_Type>(*this)*=rhs; }
		friend Vector4d operator*(_Type lhs, const Vector4d &rhs) { return Vector4dC<_Type>(rhs)*=lhs; }
		Vector4d operator/(_Type rhs) const { return Vector4dC<_Type>(*this)/=rhs; }
		friend Vector4d operator/(_Type lhs, const Vector4d &rhs) { return Vector4dC<_Type>(rhs)/=lhs; }
		/// �ő剻���Z�q
		Vector4d operator|(const Vector4d &rhs) const { return Vector4dC<_Type>(*this)|=rhs; }
		/// �ŏ������Z�q
		Vector4d operator&(const Vector4d &rhs) const { return Vector4dC<_Type>(*this)&=rhs; }

		// ��r���Z�q
		bool operator==(const Vector4d &rhs)
		{
			return (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
		}
		bool operator!=(const Vector4d &rhs)
		{
			return (x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w);
		}

		/// �R�����x�N�g������Ԃ�
		Vector3d<_Type> &vector3d()
		{
			return *reinterpret_cast<Vector3d<_Type> *>(&x);
		}
		/// �R�����x�N�g������Ԃ�
		const Vector3d<_Type> &vector3d() const
		{
			return *reinterpret_cast<const Vector3d<_Type> *>(&x);
		}
		
		/// ���K��
		Vector4d &normalize() {
			_Type l = length(); x /= l; y /= l; z /= l; w /= l;
			return *this;
		}
		/// ���K�����ꂽ�x�N�g����Ԃ�
		Vector4d normal() const {
			return Vector4d().setNormal(*this);
		}

		// @{
		/// �O�ϒl���Z�b�g
		Vector4d &setCross(const Vector4d &u, const Vector4d &v, const Vector4d &w) {
			_Type a = v.x * w.y + v.y * w.x;
			_Type b = v.x * w.z + v.z * w.x;
			_Type c = v.x * w.w + v.w * w.x;
			_Type d = v.y * w.z + v.z * w.y;
			_Type e = v.y * w.w + v.w * w.y;
			_Type f = v.z * w.w + v.w * w.z;
			x = f * u.y + e * u.z + d * u.w
			y = f * u.x + c * u.z + b * u.w
			z = e * u.x + c * u.y + a * u.w
			w = d * u.x + b * u.y + a * u.z 
			return *this;
		}
		/// �O�ς�Ԃ�
		friend Vector4d cross(const Vector4d &u, const Vector4d &v, const Vector4d &w) {
			return Vector4dC<_Type>().setCross(u, v, w);
		}
		// @}

		// @{
		/// ���ϒl��Ԃ�
		_Type operator *(const Vector4d &rhs) const {
			return x*rhs.x+y*rhs.y+z*rhs.z+w*rhs.w;
		}
		friend _Type dot(const Vector4d &lhs, const Vector4d &rhs) {
			return lhs * rhs;
		}
		// @}

		/// �x�N�g���̒����̓��
		_Type length2() const {
			return (*this)*(*this);
		}
		/// �x�N�g���̒���
		Real length() const {
			return sqrt((Real)length2());
		}

		/// @defgroup Vector4dInterporations ��Ԍn
		// @{
		/// ���`��Ԃ����l���Z�b�g
		Vector4d &setLerp(const Vector4d &lhs, const Vector4d &rhs, _Type t) {
			*this = lhs + (rhs-lhs)*t;
			return *this;
		}
		/// �G���~�[�g�X�v���C��
		Vector4d &setHermite(const Vector4d &begin, const Vector4d &begintan, const Vector4d &end, const Vector4d &endtan, _Type t) {
			BOOST_STATIC_ASSERT(0); // This is not implemented.
			return *this;
		}
		/// Catmull-Rom�X�v���C��
		Vector4d &setCatmullRom(const Vector4d &prev, const Vector4d &begin, const Vector4d &end, const Vector4d &next, _Type t) {
			BOOST_STATIC_ASSERT(0); // This is not implemented.
			return *this;
		}

		/// 2�_�ł̕�ԁi���`��ԁj
		Vector4d &set2PInterpolation(const Vector4d& lhs, const Vector4d& rhs, _Type t) {
			return setLerp(lhs, rhs, t);
		}
		/// 4�_�ł̕�ԁi�G���~�[�g�A�܂���Catmull-Rom�j
		Vector4d &set4PInterpolation(const Vector4d& prev, const Vector4d& begin, const Vector4d& end, const Vector4d& next, _Type t) {
#ifdef GCTP_VECTOR_HERMITE
			return setHermite(begin, (begin-prev)/2, end, (next-end)/2, t);
#else
			return setCatmullRom(prev, begin, end, next, t);
#endif
		}
		// @}

		operator const _Type *() const
		{
			return &x;
		}

		operator _Type *()
		{
			return &x;
		}

// D3DX���C�u�����T�|�[�g
#ifdef GCTP_USE_D3DXMATH
		operator const D3DXVECTOR4 &() const
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return *reinterpret_cast<const D3DXVECTOR4 *>(this);
		}
		operator D3DXVECTOR4 &()
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return *reinterpret_cast<D3DXVECTOR4 *>(this);
		}
		operator const D3DXVECTOR4 *() const
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return reinterpret_cast<const D3DXVECTOR4 *>(this);
		}
		operator D3DXVECTOR4 *()
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return reinterpret_cast<D3DXVECTOR4 *>(this);
		}
#endif
	};

	// @{
	/// �Q�_�Ԃ̋���
	template<typename _Type>
	inline _Type distance2(const Vector4d<_Type> &lhs, const Vector4d<_Type> &rhs) { return (rhs-lhs).length2(); }
	template<typename _Type>
	inline _Type distance(const Vector4d<_Type> &lhs, const Vector4d<_Type> &rhs) { return (rhs-lhs).length(); }
	// @}

	template<class E, class T, class _Type> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, Vector4d<_Type> const & v)
	{
		os<<"{"<<v.x<<","<<v.y<<","<<v.z<<"}";
		return os;
	}

	/// �R���X�g���N�^�t�x�N�g���N���X
	template<typename _Type>
	struct Vector4dC : Vector4d<_Type> {
		Vector4dC() {}
		Vector4dC(const _Type *p)
		{
			set(p);
		}
		Vector4dC(_Type x, _Type y, _Type z, _Type w)
		{
			set(x, y, z, w);
		}
		Vector4dC(const Vector3d<_Type> &src, _Type w = 1)
		{
			set(src, w);
		}
		template<typename _Type2>
		Vector4dC(const Vector4d<_Type2> &src)
		{
			set(src);
		}
#ifdef GCTP_USE_D3DXMATH
		Vector4dC(const D3DXVECTOR4 &src)
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			set(src.x, src.y, src.z, src.w);
		}
#endif
	};

}} // namespace gctp

// D3DX���C�u�����T�|�[�g
#ifdef GCTP_USE_D3DXMATH
#include <gctp/math/d3d/vector4d.inl>
#endif

#endif //_GCTP_MATH_VECTOR4_HPP_