#ifndef _GCTP_MATH_VECTOR3D_HPP_
#define _GCTP_MATH_VECTOR3D_HPP_
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
#ifdef GCTP_USE_D3DXMATH
# include <d3dx9math.h>
#else
# include <cmath>
# include <limits>
#endif

namespace gctp { namespace math {

	/** 3�����x�N�g��
	 *
	 * POD�ƂȂ�悤�A�R���X�g���N�^������
	 *
	 * ���̎�̌^�ɂ́APOD�ɂȂ�悤�R���X�g���N�^���������������Ǝv���B
	 * ������set���\�b�h��p�ӂ���B
	 */
	template<typename _Type>
	struct Vector3d {
		_Type x, y, z;
		
		/// �l�Z�b�g
		Vector3d &set(const _Type *p)
		{
			x = p[0]; y = p[1]; z = p[2];
			return *this;
		}
		/// �l�Z�b�g
		Vector3d &set(_Type x, _Type y, _Type z)
		{
			this->x = x; this->y = y; this->z = z;
			return *this;
		}
		/// �l�Z�b�g
		template<typename _Type2>
		Vector3d &set(const Vector3d<_Type2> &src)
		{
			x = _Type(src.x); _Type(y = src.y); _Type(z = src.z);
			return *this;
		}

		// ������Z�q
		Vector3d &operator+=(const Vector3d &src) {
			x += src.x; y += src.y; z += src.z;
			return *this;
		}
		// ������Z�q
		Vector3d &operator-=(const Vector3d &src) {
			x -= src.x; y -= src.y; z -= src.z;
			return *this;
		}
		// ������Z�q
		Vector3d &operator*=(const Vector3d &src) {
			x *= src.x; y *= src.y; z *= src.z;
			return *this;
		}
		// ������Z�q
		Vector3d &operator/=(const Vector3d &src) {
			x /= src.x; y /= src.y; z /= src.z;
			return *this;
		}
		// ������Z�q
		Vector3d &operator*=(Real src) {
			x = _Type(x*src); y = _Type(y*src); z = _Type(z*src);
			return *this;
		}
		// ������Z�q
		Vector3d &operator/=(Real src) {
			x = _Type(x/src); y = _Type(y/src); z = _Type(z/src);
			return *this;
		}
		/// �O�ϒl���Z�b�g
		Vector3d &operator%=(const Vector3d &rhs) {
			return *this = Vector3d().set(y*rhs.z - z*rhs.y, z*rhs.x - x*rhs.z, x*rhs.y - y*rhs.x);
		}
		/// ���
		Vector3d &operator|=(const Vector3d &src) {
			if(x < src.x) x = src.x;
			if(y < src.y) y = src.y;
			if(z < src.z) z = src.z;
			return *this;
		}
		/// ���
		Vector3d &operator&=(const Vector3d &src) {
			if(x > src.x) x = src.x;
			if(y > src.y) y = src.y;
			if(z > src.z) z = src.z;
			return *this;
		}

		Vector3d operator + () const { return *this; }
		Vector3d operator - () const { return Vector3d().set(-x, -y, -z); }
		Vector3d operator+(const Vector3d &rhs) const { return Vector3d(*this)+=rhs; }
		Vector3d operator-(const Vector3d &rhs) const { return Vector3d(*this)-=rhs; }
		friend Vector3d mult(const Vector3d &lhs, const Vector3d &rhs) { return Vector3d(lhs)*=rhs; }
		Vector3d operator/(const Vector3d &rhs) const { return Vector3d(*this)/=rhs; }
		Vector3d operator*(Real rhs) const { return Vector3d(*this)*=rhs; }
		friend Vector3d operator*(Real lhs, const Vector3d &rhs) { return Vector3d(rhs)*=lhs; }
		Vector3d operator/(Real rhs) const { return Vector3d(*this)/=rhs; }
		friend Vector3d operator/(Real lhs, const Vector3d &rhs) { return Vector3d(rhs)/=lhs; }
		/// �ő剻���Z�q
		Vector3d operator|(const Vector3d &rhs) const { return Vector3d(*this)|=rhs; }
		/// �ŏ������Z�q
		Vector3d operator&(const Vector3d &rhs) const { return Vector3d(*this)&=rhs; }
		
		// ��r���Z�q
		bool operator==(const Vector3d &rhs)
		{
			return (x == rhs.x && y == rhs.y && z == rhs.z);
		}
		bool operator!=(const Vector3d &rhs)
		{
			return (x != rhs.x || y != rhs.y || z != rhs.z);
		}
		
		/// ���K��
		Vector3d &normalize() {
			_Type l = length(); x /= l; y /= l; z /= l;
			return *this;
		}
		/// ���K�����ꂽ�x�N�g����Ԃ�
		Vector3d normal() const {
			return Vector3d(*this).normalize();
		}

		// @{
		/// �O�ς�Ԃ�
		Vector3d operator%(const Vector3d &rhs) const {
			return Vector3d(*this) %= rhs;
		}
		friend Vector3d cross(const Vector3d &lhs, const Vector3d &rhs) {
			return lhs % rhs;
		}
		// @}

		// @{
		/// ���ϒl��Ԃ�
		_Type operator *(const Vector3d &rhs) const {
			return x*rhs.x+y*rhs.y+z*rhs.z;
		}
		friend _Type dot(const Vector3d &lhs, const Vector3d &rhs) {
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

		/// @defgroup Vector3dInterporations ��Ԍn
		// @{
		/// ���`��Ԃ����l���Z�b�g
		Vector3d &setLerp(const Vector3d &lhs, const Vector3d &rhs, _Type t) {
			*this = lhs + (rhs-lhs)*t;
			return *this;
		}
		/// �G���~�[�g�X�v���C��
		Vector3d &setHermite(const Vector3d &begin, const Vector3d &begintan, const Vector3d &end, const Vector3d &endtan, _Type t) {
			//BOOST_STATIC_ASSERT(0); // This is not implemented.
			assert("not implemented");
			return *this;
		}
		/// Catmull-Rom�X�v���C��
		Vector3d &setCatmullRom(const Vector3d &prev, const Vector3d &begin, const Vector3d &end, const Vector3d &next, _Type t) {
			//BOOST_STATIC_ASSERT(0); // This is not implemented.
			assert("not implemented");
			return *this;
		}

		/// 2�_�ł̕�ԁi���`��ԁj
		Vector3d &set2PInterpolation(const Vector3d& lhs, const Vector3d& rhs, _Type t) {
			return setLerp(lhs, rhs, t);
		}
		/// 4�_�ł̕�ԁi�G���~�[�g�A�܂���Catmull-Rom�j
		Vector3d &set4PInterpolation(const Vector3d& prev, const Vector3d& begin, const Vector3d& end, const Vector3d& next, _Type t) {
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
		operator const D3DXVECTOR3 &() const
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return *reinterpret_cast<const D3DXVECTOR3 *>(this);
		}
		operator D3DXVECTOR3 &()
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return *reinterpret_cast<D3DXVECTOR3 *>(this);
		}
		operator const D3DXVECTOR3 *() const
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return reinterpret_cast<const D3DXVECTOR3 *>(this);
		}
		operator D3DXVECTOR3 *()
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return reinterpret_cast<D3DXVECTOR3 *>(this);
		}
#endif
#ifdef D3DVECTOR_DEFINED
		operator const D3DVECTOR &() const
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return *reinterpret_cast<const D3DVECTOR *>(this);
		}
		operator D3DVECTOR &()
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return *reinterpret_cast<D3DVECTOR *>(this);
		}
		operator const D3DVECTOR *() const
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return reinterpret_cast<const D3DVECTOR *>(this);
		}
		operator D3DVECTOR *()
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			return reinterpret_cast<D3DVECTOR *>(this);
		}
#endif
	};

	// @{
	/// �Q�_�Ԃ̋���
	template<typename _Type>
	inline _Type distance2(const Vector3d<_Type> &lhs, const Vector3d<_Type> &rhs) { return (rhs-lhs).length2(); }
	template<typename _Type>
	inline _Type distance(const Vector3d<_Type> &lhs, const Vector3d<_Type> &rhs) { return (rhs-lhs).length(); }
	// @}

	template<class E, class T, class _Type> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, Vector3d<_Type> const & v)
	{
		os<<"{"<<v.x<<","<<v.y<<","<<v.z<<"}";
		return os;
	}

	/// �R���X�g���N�^�t�x�N�g���N���X
	template<typename _Type>
	struct Vector3dC : Vector3d<_Type> {
		Vector3dC() {}
		Vector3dC(const _Type *p)
		{
			set(p);
		}
		Vector3dC(_Type x, _Type y, _Type z)
		{
			set(x, y, z);
		}
		template<typename _Type2>
		Vector3dC(const Vector3d<_Type2> &src)
		{
			set(src);
		}
// D3DX���C�u�����T�|�[�g
#ifdef GCTP_USE_D3DXMATH
		Vector3dC(const D3DXVECTOR3 & src)
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			set(src.x, src.y, src.z);
		}
#endif
#ifdef D3DVECTOR_DEFINED
		Vector3dC(const D3DVECTOR & src)
		{
			BOOST_STATIC_ASSERT((boost::is_same<_Type, float>::value));
			set(src.x, src.y, src.z);
		}
#endif
	};

}} // namespace gctp

// D3DX���C�u�����T�|�[�g
#ifdef GCTP_USE_D3DXMATH
#include <gctp/math/d3d/vector3d.inl>
#endif

#endif //_GCTP_MATH_VECTOR3D_HPP_