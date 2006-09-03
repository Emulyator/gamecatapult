#ifndef _GCTP_MATH_D3D_PLANE3D_INL_
#define _GCTP_MATH_D3D_PLANE3D_INL_

namespace gctp { namespace math {

	template<>
	Plane3d<float> &Plane3d<float>::set(const Vector3d<float> &pos, const Vector3d<float> &vec)
	{
		D3DXPlaneFromPointNormal(*this, pos, vec);
		return *this;
	}

	template<>
	Plane3d<float> &Plane3d<float>::set(const Vector3d<float> &p1, const Vector3d<float> &p2, const Vector3d<float> &p3)
	{
		D3DXPlaneFromPoints(*this, p1, p2, p3);
		return *this;
	}

	template<>
	Plane3d<float> &Plane3d<float>::normalize()
	{
		D3DXPlaneNormalize(*this, *this);
		return *this;
	}

/*	template<>
	float Plane3d<float>::operator*(const Vector4d<float> &rhs) const
	{
		return D3DXPlaneDot(*this, rhs);
	}

	template<>
	inline float dot(const Plane3d<float> &lhs, const Vector4d<float> &rhs)
	{
		return D3DXPlaneDot(lhs, rhs);
	}

	template<>
	inline float dot(const Vector4d<float> &lhs, const Plane3d<float> &rhs)
	{
		return D3DXPlaneDot(rhs, lhs);
	}

	template<>
	float Plane3d<float>::operator*(const Vector3d<float> &rhs) const
	{
		return D3DXPlaneDotCoord(*this, rhs);
	}

	template<>
	inline float operator*(const Vector3d<float> &lhs, const Plane3d<float> &rhs)
	{
		return D3DXPlaneDotCoord(rhs, lhs);
	}

	template<>
	inline float dot(const Plane3d<float> &lhs, const Vector3d<float> &rhs)
	{
		return D3DXPlaneDotCoord(lhs, rhs);
	}

	template<>
	inline float dot(const Vector3d<float> &lhs, const Plane3d<float> &rhs)
	{
		return D3DXPlaneDotCoord(rhs, lhs);
	}

	template<>
	inline float dotVector(const Plane3d<float> &lhs, const Vector3d<float> &rhs)
	{
		return D3DXPlaneDotNormal(lhs, rhs);
	}

	template<>
	inline float dotVector(const Vector3d<float> &lhs, const Plane3d<float> &rhs)
	{
		return D3DXPlaneDotNormal(rhs, lhs);
	}*/

}} // namespace gctp

#endif //_GCTP_MATH_D3D_PLANE3D_INL_