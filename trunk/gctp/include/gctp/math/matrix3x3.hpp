#ifndef _GCTP_MATH_MATRIX3X3_HPP_
#define _GCTP_MATH_MATRIX3X3_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 3x3�s��N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 2:42:16
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/vector.hpp>

namespace gctp { namespace math {

	/** 3x3�s��N���X
	 *
	 * ���s�ړ������̖����A�R�����̕ύX�p�ƁA�Q�����̕ύX�p�r�A�Q��ނ̗p�r������
	 *
	 * POD�ƂȂ�悤�A�R���X�g���N�^������
	 *
	 * �l�̂Ȃ�тɂ��ẮAOpenGL�̉��߂�p����
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/16 5:02:29
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<typename _Type>
	struct Matrix3x3 {
		union {
			struct {
				_Type _11, _21, _31;
				_Type _12, _22, _32;
				_Type _13, _23, _33;
			};
			_Type m[3][3];
		};

		/// �l�Z�b�g
		Matrix3x3 &set(bool ident)
		{
			if(ident) { _11 = _22 = _33 = 1; } else { _11 = _22 = _33 = 0; }
			_21 = _31 = _12 = _32 = _13 = _23 = 0;
			return *this;
		}
		/// �l�Z�b�g
		Matrix3x3 &set(
			_Type _11, _Type _12, _Type _13,
			_Type _21, _Type _22, _Type _23,
			_Type _31, _Type _32, _Type _33
		)
		{
			this->_11 = _11; this->_12 = _12; this->_13 = _13;
			this->_21 = _21; this->_22 = _22; this->_23 = _23;
			this->_31 = _31; this->_32 = _32; this->_33 = _33;
			return *this;
		}
		/// �l�Z�b�g
		Matrix3x3 &set(const _Type *_m)
		{
			memcpy(m, _m, sizeof(_Type)*9);
			return *this;
		}
		/// �l�Z�b�g
		Matrix3x3 &set(const Vector2d<_Type> &right, const Vector2d<_Type> &up, const Vector2d<_Type> &pos)
		{
			this->right() = right; this->up() = up; position() = pos;
			return *this;
		}
		/// �l�Z�b�g
		Matrix3x3 &set(const Vector3d<_Type> &right, const Vector3d<_Type> &up, const Vector3d<_Type> &at)
		{
			this->right3d() = right; this->up3d() = up; this->at3d() = at;
			return *this;
		}

		/// �A�N�Z�X�֐�
		_Type &operator () ( uint row, uint col ) {
			return m[col][row];
		}
		/// �A�N�Z�X�֐�
		_Type operator () ( uint row, uint col ) const {
			return m[col][row];
		}
		/// X��
		Vector2d<_Type> &right() {
			return *(Vector2d<_Type> *)(&m[0][0]);
		}
		/// X��
		const Vector2d<_Type> &right() const {
			return *(Vector2d<_Type> *)(&m[0][0]);
		}
		/// X��
		Vector3d<_Type> &right3d() {
			return *(Vector3d<_Type> *)(&m[0][0]);
		}
		/// X��
		const Vector3d<_Type> &right3d() const {
			return *(Vector3d<_Type> *)(&m[0][0]);
		}
		/// Y��
		Vector2d<_Type> &up() {
			return *(Vector2 *)(&m[1][0]);
		}
		/// Y��
		const Vector2d<_Type> &up() const {
			return *(Vector2 *)(&m[1][0]);
		}
		/// Y��
		Vector3d<_Type> &up3d() {
			return *(Vector3d<_Type> *)(&m[1][0]);
		}
		/// Y��
		const Vector3d<_Type> &up3d() const {
			return *(Vector3d<_Type> *)(&m[1][0]);
		}
		/// Z��
		Vector3d<_Type> &at() {
			return *(Vector3d<_Type> *)(&m[2][0]);
		}
		/// Z��
		const Vector3d<_Type> &at() const {
			return *(Vector3d<_Type> *)(&m[2][0]);
		}
		/// ���s�ړ���
		Vector2d<_Type> &position() {
			return *(Vector2d<_Type> *)(&m[2][0]);
		}
		/// ���s�ړ���
		const Vector2d<_Type> &position() const {
			return *(Vector2d<_Type> *)(&m[2][0]);
		}

		/// �X�P�[���𒊏o
		Vector2d<_Type> getScale() const { return Vector2d<_Type>(right().length(),up().length()); }

		/// �L���X�g���Z�ǂ�
		operator _Type* () { return &_11; }
		/// �L���X�g���Z�ǂ�
		operator const _Type* () const { return &_11; }

		/// ���
		Matrix3x3& operator *= ( const Matrix3x3 &rhs ) {
			Matrix3x3 w(*this);
			_11 = w._11*rhs._11 + w._12*rhs._21 + w._13*rhs._31;
			_12 = w._11*rhs._12 + w._12*rhs._22 + w._13*rhs._32;
			_13 = w._11*rhs._13 + w._12*rhs._23 + w._13*rhs._33;
			_21 = w._21*rhs._11 + w._22*rhs._21 + w._23*rhs._31;
			_22 = w._21*rhs._12 + w._22*rhs._22 + w._23*rhs._32;
			_23 = w._21*rhs._13 + w._22*rhs._23 + w._23*rhs._33;
			_31 = w._31*rhs._11 + w._32*rhs._21 + w._33*rhs._31;
			_32 = w._31*rhs._12 + w._32*rhs._22 + w._33*rhs._32;
			_33 = w._31*rhs._13 + w._32*rhs._23 + w._33*rhs._33;
			return *this;
		}
		/// ���
		Matrix3x3& operator += ( const Matrix3x3 &rhs ) {
			_11 += rhs._11; _21 += rhs._21; _31 += rhs._31;
			_12 += rhs._12; _22 += rhs._22; _32 += rhs._32;
			_13 += rhs._13; _23 += rhs._23; _33 += rhs._33;
		}
		/// ���
		Matrix3x3& operator -= ( const Matrix3x3 &rhs ) {
			_11 -= rhs._11; _21 -= rhs._21; _31 -= rhs._31;
			_12 -= rhs._12; _22 -= rhs._22; _32 -= rhs._32;
			_13 -= rhs._13; _23 -= rhs._23; _33 -= rhs._33;
		}
		/// ���
		Matrix3x3& operator *= ( _Type rhs ) {
			_11 *= rhs; _21 *= rhs; _31 *= rhs;
			_12 *= rhs; _22 *= rhs; _32 *= rhs;
			_13 *= rhs; _23 *= rhs; _33 *= rhs;
		}
		/// ���
		Matrix3x3& operator /= ( _Type rhs ) {
			_11 /= rhs; _21 /= rhs; _31 /= rhs;
			_12 /= rhs; _22 /= rhs; _32 /= rhs;
			_13 /= rhs; _23 /= rhs; _33 /= rhs;
		}

		/// �P�����Z
		Matrix3x3 operator + () const { return *this; }
		/// �P�����Z
		Matrix3x3 operator - () const { return (Matrix3x3){{-_11, -_21, -_31}, {-_12, -_22, -_32}, {-_13, -_23, -_33}}; }

		/// �Q�����Z
		Matrix3x3 operator * ( const Matrix3x3 &rhs ) const { return Matrix3x3(*this)*=rhs; }
		/// �Q�����Z
		Matrix3x3 operator + ( const Matrix3x3 &rhs ) const { return Matrix3x3(*this)+=rhs; }
		/// �Q�����Z
		Matrix3x3 operator - ( const Matrix3x3 &rhs ) const { return Matrix3x3(*this)-=rhs; }
		/// �Q�����Z
		Matrix3x3 operator * ( _Type rhs ) const { return Matrix3x3(*this)*=rhs; }
		/// �Q�����Z
		Matrix3x3 operator / ( _Type rhs ) const { return Matrix3x3(*this)/=rhs; }
		/// �Q�����Z
		friend Matrix3x3 operator * ( _Type lhs, const Matrix3x3 &rhs ) { return rhs*lhs; }
		/// �Q�����Z
		friend Matrix3x3 operator / ( _Type lhs, const Matrix3x3 &rhs ) { return rhs/lhs; }
		/// ���W�ϊ�
		Vector3d<_Type> operator*(const Vector3d<_Type> &rhs) const {
			return Vector3dC<_Type>(
				_11*rhs.x+_12*rhs.y+_13*rhs.z,
				_21*rhs.x+_22*rhs.y+_23*rhs.z,
				_31*rhs.x+_32*rhs.y+_33*rhs.z
			);
		}
		/// ���W�ϊ�
		Vector2d<_Type> operator*(const Vector2d<_Type> &rhs) const {
			_Type invw = _Type(1)/(_31*rhs.x+_32*rhs.y+_33);
			return Vector2dC<_Type>(
				(_11*rhs.x+_12*rhs.y+_13)*invw,
				(_21*rhs.x+_22*rhs.y+_23)*invw
			);
		}

		/// �Q�����Z
		bool operator == ( const Matrix3x3 &rhs ) const { return _11==rhs._11 && _21==rhs._21 && _31==rhs._31 && _12==rhs._12 && _22==rhs._22 && _32==rhs._32 && _13==rhs._13 && _23==rhs._23 && _33==rhs._33; }
		/// �Q�����Z
		bool operator != ( const Matrix3x3 &rhs ) const { return _11!=rhs._11 || _21!=rhs._21 || _31!=rhs._31 || _12!=rhs._12 || _22!=rhs._22 || _32!=rhs._32 || _13!=rhs._13 || _23!=rhs._23 || _33!=rhs._33; }

		/// ��]���Z�b�g
		Matrix3x3 &rot(_Type theta) { _11 = _22 = cos(theta); _12 = sin(theta); _21 = -_12; _33=1; _13=_23=_31=_32=0; return *this; }
		/// �X�P�[�����Z�b�g
		Matrix3x3 &scale(_Type x, _Type y) { _11=x; _22=y; _33=1; _13=_23=_31=_32=0; return *this; }
		/// �X�P�[�����Z�b�g
		Matrix3x3 &scale(const Vector2d<_Type> &scl) { return scale(scl.x, scl.y); }

		/// �P�ʍs��
		Matrix3x3 &identify()
		{
			_11 = _22 = _33 = _Type(1);
			_21 = _31 = _12 = _32 = _13 = _23 = _Type(0);
			return *this;
		}

		/// ���K��
		Matrix3x3 &normalize()
		{
			right().normalize();
			up().normalize();
			return *this;
		}
		/// ���K�������s��
		Matrix3x3 normal() const
		{
			return Matrix3x3(*this).normalize();
		}
		
		/// �ϊ�
		Vector2d<_Type> transformVector(const Vector2d<_Type> &src) const
		{
			return Vector2dC<_Type>(
				_11*src.x+_12*src.y,
				_21*src.x+_22*src.y
			);
		}
	};

	template<class E, class T, class _Type> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, Matrix3x3<_Type> const & m)
	{
		os<<"{["<<m._11<<","<<m._12<<"],["<<m._21<<","<<m._22<<"]}";
		return os;
	}

	/** �R���X�g���N�^�t3x3�s��N���X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/16 5:02:29
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<typename _Type>
	struct Matrix3x3C : Matrix3x3<_Type> {
		/// �R���X�g���N�^
		Matrix3x3C() {}
		/// �R���X�g���N�^
		explicit Matrix3x3C(bool ident)
		{
			if(ident) identify();
			else _11 = _22 = _33 = _21 = _31 = _12 = _32 = _13 = _23 = _Type(0);
		}
		/// �R���X�g���N�^
		Matrix3x3C(
			_Type _11, _Type _12, _Type _13,
			_Type _21, _Type _22, _Type _23,
			_Type _31, _Type _32, _Type _33
		)
		{
			set(
				_11, _12, _13,
				_21, _22, _23,
				_31, _32, _33,
			);
		}
		/// �R���X�g���N�^
		Matrix3x3C(const _Type *_m) { memcpy(m, _m, sizeof(_Type)*9); }
		/// �R���X�g���N�^
		Matrix3x3C(const Vector2d<_Type> &right, const Vector2d<_Type> &up, const Vector2d<_Type> &pos)
		{
			this->right() = right; this->up() = up; position() = pos;
			m[0][2]=m[1][2]=0;m[2][2]=1;
		}
		/// �R���X�g���N�^
		Matrix3x3C(const Vector3d<_Type> &right, const Vector3d<_Type> &up, const Vector3d<_Type> &at)
		{
			right3d() = right; up3d() = up; this->at() = at;
			m[0][2]=m[1][2]=0;m[2][2]=1;
		}
	};

}} //namespace gctp

#endif //_GCTP_MATH_MATRIX3X3_HPP_