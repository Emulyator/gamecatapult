#ifndef _GCTP_MATH_D3D_MATRIX4X4_INL_
#define _GCTP_MATH_D3D_MATRIX4X4_INL_

namespace gctp { namespace math {

	template<>
	Matrix4x4<float> &Matrix4x4<float>::operator*=(const Matrix4x4<float> &src) {
		D3DXMatrixMultiply(*this, *this, src);
		return *this;
	}

	template<>
	Vector4d<float> Matrix4x4<float>::operator*(const Vector4d<float> &src) const {
		Vector4d<float> ret;
		D3DXVec4Transform(ret, src, *this);
		return ret;
	}
	
	template<>
	Vector3d<float> Matrix4x4<float>::operator*(const Vector3d<float> &src) const {
		Vector3d<float> ret;
		D3DXVec3TransformCoord(ret, src, *this);
		return ret;
	}

	template<>
	Plane3d<float> Matrix4x4<float>::operator*(const Plane3d<float> &src) const {
		Plane3d<float> ret;
		D3DXPlaneTransform(ret, src, *this);
		return ret;
	}

	template<>
	Matrix4x4<float>::operator bool() const {
		return (D3DXMatrixIsIdentity(*this))?true:false;
	}

	template<>
	bool Matrix4x4<float>::operator!() const {
		return (D3DXMatrixIsIdentity(*this))?false:true;
	}

	template<>
	Matrix4x4<float> &Matrix4x4<float>::invert(float &det) {
		D3DXMatrixInverse(*this, &det, *this);
		return *this;
	}

	template<>
	Matrix4x4<float> &Matrix4x4<float>::invert() {
		D3DXMatrixInverse(*this, NULL, *this);
		return *this;
	}

	template<>
	Matrix4x4<float> &Matrix4x4<float>::transpose() {
		D3DXMatrixTranspose(*this, *this);
		return *this;
	}

	template<>
	Matrix4x4<float> &Matrix4x4<float>::rotX(float r) {
		D3DXMatrixRotationX(*this, r);
		return *this;
	}
	
	template<>
	Matrix4x4<float> &Matrix4x4<float>::rotY(float r) {
		D3DXMatrixRotationY(*this, r);
		return *this;
	}
	
	template<>
	Matrix4x4<float> &Matrix4x4<float>::rotZ(float r) {
		D3DXMatrixRotationZ(*this, r);
		return *this;
	}
	
	template<>
	Matrix4x4<float> &Matrix4x4<float>::rot(float yaw, float pitch, float roll) {
		D3DXMatrixRotationYawPitchRoll(*this, yaw, pitch, roll);
		return *this;
	}
	
	template<>
	Matrix4x4<float> &Matrix4x4<float>::rot(const Vector3d<float> &v, float r) {
		D3DXMatrixRotationAxis(*this, v, r);
		return *this;
	}
	
	template<>
	Matrix4x4<float> &Matrix4x4<float>::scale(float sx, float sy, float sz) {
		D3DXMatrixScaling(*this, sx, sy, sz);
		return *this;
	}
	
	template<>
	Matrix4x4<float> &Matrix4x4<float>::scale(const Vector3d<float> &v) {
		D3DXMatrixScaling(*this, v.x, v.y, v.z);
		return *this;
	}
	
	template<>
	Matrix4x4<float> &Matrix4x4<float>::trans(float tx, float ty, float tz) {
		D3DXMatrixTranslation(*this, tx, ty, tz);
		return *this;
	}
	
	template<>
	Matrix4x4<float> &Matrix4x4<float>::trans(const Vector3d<float> &v) {
		D3DXMatrixTranslation(*this, v.x, v.y, v.z);
		return *this;
	}

	template<>
	const Matrix4x4<float> &Matrix4x4<float>::transform(Vector4d<float> &dst, const Vector4d<float> &src) const {
		D3DXVec4Transform(dst, src, *this);
		return *this;
	}

	template<>
	const Matrix4x4<float> &Matrix4x4<float>::transform(Vector3d<float> &dst, const Vector3d<float> &src) const {
		D3DXVec3TransformCoord(dst, src, *this);
		return *this;
	}

	template<>
	const Matrix4x4<float> &Matrix4x4<float>::transformVector(Vector3d<float> &dst, const Vector3d<float> &src) const {
		D3DXVec3TransformNormal(dst, src, *this);
		return *this;
	}

	template<>
	Vector3d<float> Matrix4x4<float>::transformVector(const Vector3d<float> &src) const {
		Vector3d<float> ret;
		D3DXVec3TransformNormal(ret, src, *this);
		return ret;
	}

	template<>
	Matrix4x4<float> &Matrix4x4<float>::setLookAtLH(const Vector3d<float> &vpos, const Vector3d<float> &vat, const Vector3d<float> &vup) {
		D3DXMatrixLookAtLH(*this, vpos, vat, vup);
		return *this;
	}

	template<>
	Matrix4x4<float> &Matrix4x4<float>::setLookAtRH(const Vector3d<float> &vpos, const Vector3d<float> &vat, const Vector3d<float> &vup) {
		D3DXMatrixLookAtRH(*this, vpos, vat, vup);
		return *this;
	}

	template<>
	Matrix4x4<float> &Matrix4x4<float>::setFOVLH(float fov, float aspect, float zn, float zf) {
		D3DXMatrixPerspectiveFovLH(*this, fov, aspect, zn, zf);
		return *this;
	}

	template<>
	Matrix4x4<float> &Matrix4x4<float>::setFOVRH(float fov, float aspect, float zn, float zf) {
		D3DXMatrixPerspectiveFovRH(*this, fov, aspect, zn, zf);
		return *this;
	}

	template<>
	Matrix4x4<float> &Matrix4x4<float>::setParsLH(float w, float h, float zn, float zf) {
		D3DXMatrixPerspectiveLH(*this, w, h, zn, zf);
		return *this;
	}

	template<>
	Matrix4x4<float> &Matrix4x4<float>::setParsRH(float w, float h, float zn, float zf) {
		D3DXMatrixPerspectiveRH(*this, w, h, zn, zf);
		return *this;
	}

	template<>
	Matrix4x4<float> &Matrix4x4<float>::setFOVLH(float fov, float aspect, float l, float t, float r, float b, float zn, float zf) {
		float coef = zn*tanf(fov/2.0f);
		D3DXMatrixPerspectiveOffCenterLH(*this, coef*(2*l-1), coef*(2*r-1), coef/aspect*(2*t-1), coef/aspect*(2*b-1), zn, zf);
		return *this;
	}

	template<>
	Matrix4x4<float> &Matrix4x4<float>::setFOVRH(float fov, float aspect, float l, float t, float r, float b, float zn, float zf) {
		float coef = zn*tanf(fov/2.0f);
		D3DXMatrixPerspectiveOffCenterRH(*this, coef*(2*l-1), coef*(2*r-1), coef/aspect*(2*t-1), coef/aspect*(2*b-1), zn, zf);
		return *this;
	}

	template<>
	Matrix4x4<float> &Matrix4x4<float>::setParsLH(float l, float r, float b, float t, float zn, float zf) {
		D3DXMatrixPerspectiveOffCenterLH(*this, l, r, b, t, zn, zf);
		return *this;
	}

	template<>
	Matrix4x4<float> &Matrix4x4<float>::setParsRH(float l, float r, float b, float t, float zn, float zf) {
		D3DXMatrixPerspectiveOffCenterRH(*this, l, r, b, t, zn, zf);
		return *this;
	}

	template<>
	Matrix4x4<float> &Matrix4x4<float>::setOrthoLH(float w, float h, float zn, float zf) {
		D3DXMatrixOrthoLH(*this, w, h, zn, zf);
		return *this;
	}

	template<>
	Matrix4x4<float> &Matrix4x4<float>::setOrthoRH(float w, float h, float zn, float zf) {
		D3DXMatrixOrthoRH(*this, w, h, zn, zf);
		return *this;
	}

	template<>
	Matrix4x4<float> &Matrix4x4<float>::setOrthoLH(float l, float r, float b, float t, float zn, float zf) {
		D3DXMatrixOrthoOffCenterLH(*this, l, r, b, t, zn, zf);
		return *this;
	}

	template<>
	Matrix4x4<float> &Matrix4x4<float>::setOrthoRH(float l, float r, float b, float t, float zn, float zf) {
		D3DXMatrixOrthoOffCenterRH(*this, l, r, b, t, zn, zf);
		return *this;
	}

}} // namespace gctp

#endif //_GCTP_MATH_D3D_MATRIX4X4_INL_