#ifndef _GCTP_MATH_D3D_VECTOR2D_INL_
#define _GCTP_MATH_D3D_VECTOR2D_INL_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE

namespace gctp { namespace math {

	template<>
	Vector2d<float> &Vector2d<float>::setHermite(const Vector2d<float> &ps, const Vector2d<float> &ts, const Vector2d<float> &pe, const Vector2d<float> &te, float s) {
		D3DXVec2Hermite(*this, ps, ts, pe, te, s);
		return *this;
	}

}} // namespace gctp

#endif //_GCTP_MATH_D3D_VECTOR2D_INL_
