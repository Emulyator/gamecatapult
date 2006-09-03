#ifndef _GCTP_MATH_D3D_VECTOR4D_INL_
#define _GCTP_MATH_D3D_VECTOR4D_INL_

namespace gctp { namespace math {

	template<>
	Vector4d<float> &Vector4d<float>::normalize()
	{
		D3DXVec4Normalize(*this, *this);
		return *this;
	}
	template<>
	Vector4d<float> &Vector4d<float>::setCross(const Vector4d<float>& u, const Vector4d<float>& v, const Vector4d<float>& w)
	{
		D3DXVec4Cross(*this, u, v, w);
		return *this;
	}
	template<>
	Vector4d<float> &Vector4d<float>::setHermite(const Vector4d<float> &begin, const Vector4d<float> &begintan, const Vector4d<float> &end, const Vector4d<float> &endtan, float t)
	{
		D3DXVec4Hermite(*this, begin, begintan, end, endtan, t);
		return *this;
	}
	template<>
	Vector4d<float> &Vector4d<float>::setCatmullRom(const Vector4d<float> &prev, const Vector4d<float> &begin, const Vector4d<float> &end, const Vector4d<float> &next, float t)
	{
		D3DXVec4CatmullRom(*this, prev, begin, end, next, t);
		return *this;
	}

}} // namespace gctp

#endif //_GCTP_MATH_D3D_VECTOR4D_INL_