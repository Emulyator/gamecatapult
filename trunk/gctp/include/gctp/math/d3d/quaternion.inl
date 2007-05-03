#ifndef _GCTP_MATH_D3D_QUATERNION_INL_
#define _GCTP_MATH_D3D_QUATERNION_INL_

namespace gctp { namespace math {

	template<>
	Quaternion<float> &Quaternion<float>::set(const Vector3d<float> &axis, float rad)
	{
		D3DXQuaternionRotationAxis(*this, axis, rad);
		return *this;
	}

	template<>
	Quaternion<float> &Quaternion<float>::set(const Matrix4x4<float> &mat)
	{
		D3DXQuaternionRotationMatrix(*this, mat);
		return *this;
	}

	template<>
	Quaternion<float> &Quaternion<float>::set(float yaw, float pitch, float roll)
	{
		D3DXQuaternionRotationYawPitchRoll(*this, yaw, pitch, roll);
		return *this;
	}

	template<>
	float Quaternion<float>::toAxisAngle(Vector3d<float> &axis_out) const
	{
		float ret;
		D3DXQuaternionToAxisAngle(*this, axis_out, &ret);
		return ret;
	}

	template<>
	Matrix4x4<float> Quaternion<float>::toMatrix() const
	{
		Matrix4x4<float> ret;
		D3DXMatrixRotationQuaternion(ret, *this);
		return ret;
	}

	template<>
	Quaternion<float> &Quaternion<float>::normalize() {
		D3DXQuaternionNormalize(*this, *this);
		return *this;
	}

	template<>
	Quaternion<float> &Quaternion<float>::invert() {
		D3DXQuaternionInverse(*this, *this);
		return *this;
	}

	template<>
	float Quaternion<float>::dot(const Quaternion<float> &lhs, const Quaternion<float> &rhs) {
		return D3DXQuaternionDot(lhs, rhs);
	}

	template<>
	float Quaternion<float>::norm() const {
		return D3DXQuaternionLength(*this);
	}

	template<>
	float Quaternion<float>::norm2() const {
		return D3DXQuaternionLengthSq(*this);
	}

	template<>
	inline Quaternion<float> log(const Quaternion<float> &rhs) {
		Quaternion<float> ret;
		D3DXQuaternionLn(ret, rhs);
		return ret;
	}

	template<>
	inline Quaternion<float> exp(const Quaternion<float> &rhs) {
		Quaternion<float> ret;
		D3DXQuaternionExp(ret, rhs);
		return ret;
	}

	template<>
	Quaternion<float> &Quaternion<float>::setSlerp(const Quaternion<float> &lhs, const Quaternion<float> &rhs, float t) {
		D3DXQuaternionSlerp(*this, lhs, rhs, t);
		return *this;
	}

	template<>
	Quaternion<float> &Quaternion<float>::setSquad(const Quaternion<float> &q, const Quaternion<float> &a, const Quaternion<float> &b, const Quaternion<float> &c, float t) {
		D3DXQuaternionSquad(*this, q, a, b, c, t);
		return *this;
	}

	template<>
	void Quaternion<float>::getSquadQuaternions(Quaternion<float> &a, Quaternion<float> &b, Quaternion<float> &c, const Quaternion<float> &prev, const Quaternion<float> &begin, const Quaternion<float> &end, const Quaternion<float> &next) {
		D3DXQuaternionSquadSetup(a, b, c, prev, begin, end, next);
	}

}} // namespace gctp

#endif //_GCTP_MATH_D3D_QUATERNION_INL_