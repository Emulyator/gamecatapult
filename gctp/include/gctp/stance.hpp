#ifndef _GCTP_STANCE_HPP_
#define _GCTP_STANCE_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 位置情報クラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 21:17:27
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/vector.hpp>
#include <gctp/matrix.hpp>
#include <gctp/quat.hpp>

namespace gctp {

	/** 位置情報クラス
	 *
	 * 正規直交座標系をあらわす。
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 21:17:04
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	struct Stance {
		Stance() {}
		Stance(const Vector &position, const Quat &posture = QuatC(true)) : position(position), posture(posture) {}
		Stance(const Matrix &src) : position(src.position()), posture(QuatC(src)) {}

		void setDefault() {
			position.x = position.y = position.z = 0.0f;
			posture.identify();
		}
	
		Matrix toMatrix() const
		{
			return posture.toMatrix()*Matrix().trans(position);
		}

		/// 位置
		Vector	position;
		/// 姿勢
		Quat	posture;

		// 代入演算子を外部にさらす
		Stance &operator+=(const Stance &src) {
			position += src.position;
			posture += src.posture;
			return *this;
		}
		Stance &operator-=(const Stance &src) {
			position -= src.position;
			posture -= src.posture;
			return *this;
		}
		Stance &operator*=(const Stance &src) {
			position *= src.position;
			posture *= src.posture;
			return *this;
		}
		Stance &operator*=(float src) {
			position *= src;
			posture *= src;
			return *this;
		}
		Stance &operator/=(float src) {
			position /= src;
			posture /= src;
			return *this;
		}
		
		Stance operator+(const Stance &rhs) const { return Stance(*this)+=rhs; }
		Stance operator-(const Stance &rhs) const { return Stance(*this)-=rhs; }
		Stance operator*(const Stance &rhs) const { return Stance(*this)*=rhs; }
		Stance operator*(float rhs) const { return Stance(*this)*=rhs; }
		Stance operator/(float rhs) const { return Stance(*this)/=rhs; }
		friend Stance operator*(float lhs, const Stance &rhs) { return rhs*lhs; }
		friend Stance operator/(float lhs, const Stance &rhs) { return rhs/lhs; }
		
		/// 線形補間した値をセット
		Stance &set2PInterpolation(const Stance &lhs, const Stance &rhs, float t)
		{
			position.set2PInterpolation(lhs.position, rhs.position, t);
			posture.set2PInterpolation(lhs.posture, rhs.posture, t);
			return *this;
		}

		/// ４点で補間した値をセット
		Stance &set4PInterpolation(const Stance &prev, const Stance &begin, const Stance &end, const Stance &next, float t)
		{
			position.set4PInterpolation(prev.position, begin.position, end.position, next.position, t);
			posture.set4PInterpolation(prev.posture, begin.posture, end.posture, next.posture, t);
			return *this;
		}
	};

} // namespace gctp

#endif //_GCTP_STANCE_HPP_