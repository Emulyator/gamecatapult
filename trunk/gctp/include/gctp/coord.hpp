#ifndef _GCTP_COORD_HPP_
#define _GCTP_COORD_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult ���W�n���N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 21:17:27
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/stance.hpp>

namespace gctp {

	/** ���W�n���N���X
	 *
	 * �������W�n������킷�N���X�BStance�ɃX�P�[���������������́B
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 21:17:04
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	struct Coord : Stance {
		Coord() {}
		Coord(const Vector &position, const Quat &posture = QuatC(true), const Vector &scale = VectorC(1.0f, 1.0f, 1.0f))
			: Stance(position, posture), scale(scale) {}
		Coord(const Matrix &src) : Stance(src), scale(src.getScaling()) {}

		void setDefault() {
			Coord::setDefault();
			scale.x = scale.y = scale.z = 1.0f;
		}

		Matrix toMatrix() const
		{
			return Matrix().identify()*Matrix().scale(scale)*posture.inverse().toMatrix()*Matrix().trans(position);
		}

		/// �X�P�[��
		Vector	scale;

		// ������Z�q���O���ɂ��炷
		Coord &operator+=(const Coord &src) {
			Stance::operator+=(src);
			scale += src.scale;
			return *this;
		}
		Coord &operator-=(const Coord &src) {
			Stance::operator-=(src);
			scale -= src.scale;
			return *this;
		}
		Coord &operator*=(const Coord &src) {
			Stance::operator*=(src);
			scale *= src.scale;
			return *this;
		}
		Coord &operator*=(float src) {
			Stance::operator*=(src);
			scale *= src;
			return *this;
		}
		Coord &operator/=(float src) {
			Stance::operator/=(src);
			scale /= src;
			return *this;
		}
		
		Coord operator+(const Coord &rhs) const { return Coord(*this)+=rhs; }
		Coord operator-(const Coord &rhs) const { return Coord(*this)-=rhs; }
		Coord operator*(const Coord &rhs) const { return Coord(*this)*=rhs; }
		Coord operator*(float rhs) const { return Coord(*this)*=rhs; }
		Coord operator/(float rhs) const { return Coord(*this)/=rhs; }
		friend Coord operator*(float lhs, const Coord &rhs) { return rhs*lhs; }
		friend Coord operator/(float lhs, const Coord &rhs) { return rhs/lhs; }
		
		/// ���`��Ԃ����l���Z�b�g
		Coord &set2PInterpolation(const Coord &lhs, const Coord &rhs, float t)
		{
			Stance::set2PInterpolation(lhs, rhs, t);
			scale.set2PInterpolation(lhs.scale, rhs.scale, t);
			return *this;
		}

		/// �S�_�ŕ�Ԃ����l���Z�b�g
		Coord &set4PInterpolation(const Coord &prev, const Coord &begin, const Coord &end, const Coord &next, float t)
		{
			Stance::set4PInterpolation(prev, begin, end, next, t);
			scale.set4PInterpolation(prev.scale, begin.scale, end.scale, next.scale, t);
			return *this;
		}
	};

} // namespace gctp

#endif //_GCTP_COORD_HPP_