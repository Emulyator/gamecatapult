#ifndef _GCTP_MATH_D3D_VECTOR3D_INL_
#define _GCTP_MATH_D3D_VECTOR3D_INL_

namespace gctp { namespace math {

	template<>
	Vector3d<float> &Vector3d<float>::normalize()
	{
		D3DXVec3Normalize(*this, *this);
		return *this;
	}

	template<>
	Vector3d<float> &Vector3d<float>::setHermite(const Vector3d<float> &begin, const Vector3d<float> &begintan, const Vector3d<float> &end, const Vector3d<float> &endtan, float t)
	{
		D3DXVec3Hermite(*this, begin, begintan, end, endtan, t);
		return *this;
	}

	template<>
	Vector3d<float> &Vector3d<float>::setCatmullRom(const Vector3d<float> &prev, const Vector3d<float> &begin, const Vector3d<float> &end, const Vector3d<float> &next, float t)
	{
		D3DXVec3CatmullRom(*this, prev, begin, end, next, t);
		return *this;
	}

}} // namespace gctp

#endif //_GCTP_MATH_D3D_VECTOR3D_INL_