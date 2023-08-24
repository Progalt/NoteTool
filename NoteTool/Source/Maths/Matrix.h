#pragma once

#include "Vector4.h"
#include <cmath>

template<typename _Ty>
struct Matrix4x4
{

	Matrix4x4()
	{
		Identity();
	}

	Matrix4x4(
		_Ty m00, _Ty m10, _Ty m20, _Ty m30,
		_Ty m01, _Ty m11, _Ty m21, _Ty m31,
		_Ty m02, _Ty m12, _Ty m22, _Ty m32,
		_Ty m03, _Ty m13, _Ty m23, _Ty m33)
	{
		m[0][0] = m00, m[1][0] = m10, m[2][0] = m20, m[3][0] = m30,
			m[0][1] = m01, m[1][1] = m11, m[2][1] = m21, m[3][1] = m31,
			m[0][2] = m02, m[1][2] = m12, m[2][2] = m22, m[3][2] = m32,
			m[0][3] = m03, m[1][3] = m13, m[2][3] = m23, m[3][3] = m33;
	}

	void Identity()
	{

		m[0][0] = 1, m[1][0] = 0, m[2][0] = 0, m[3][0] = 0,
			m[0][1] = 0, m[1][1] = 1, m[2][1] = 0, m[3][1] = 0,
			m[0][2] = 0, m[1][2] = 0, m[2][2] = 1, m[3][2] = 0,
			m[0][3] = 0, m[1][3] = 0, m[2][3] = 0, m[3][3] = 1;
	}

	Matrix4x4& Mul(Matrix4x4 b) {
		Matrix4x4<_Ty> result;

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				_Ty sum = 0;
				for (int k = 0; k < 4; k++) {
					sum += m[k][j] * b.m[i][k];
				}
				result.m[i][j] = sum;
			}
		}

		*this = result;

		return *this;
	}



	Matrix4x4<_Ty> Inverse()
	{
		Matrix4x4<_Ty> T;

		_Ty s[6];
		_Ty c[6];
		s[0] = m[0][0] * m[1][1] - m[1][0] * m[0][1];
		s[1] = m[0][0] * m[1][2] - m[1][0] * m[0][2];
		s[2] = m[0][0] * m[1][3] - m[1][0] * m[0][3];
		s[3] = m[0][1] * m[1][2] - m[1][1] * m[0][2];
		s[4] = m[0][1] * m[1][3] - m[1][1] * m[0][3];
		s[5] = m[0][2] * m[1][3] - m[1][2] * m[0][3];

		c[0] = m[2][0] * m[3][1] - m[3][0] * m[2][1];
		c[1] = m[2][0] * m[3][2] - m[3][0] * m[2][2];
		c[2] = m[2][0] * m[3][3] - m[3][0] * m[2][3];
		c[3] = m[2][1] * m[3][2] - m[3][1] * m[2][2];
		c[4] = m[2][1] * m[3][3] - m[3][1] * m[2][3];
		c[5] = m[2][2] * m[3][3] - m[3][2] * m[2][3];

		_Ty idet = 1.0f / (s[0] * c[5] - s[1] * c[4] + s[2] * c[3] + s[3] * c[2] - s[4] * c[1] + s[5] * c[0]);

		T[0][0] = (m[1][1] * c[5] - m[1][2] * c[4] + m[1][3] * c[3]) * idet;
		T[0][1] = (-m[0][1] * c[5] + m[0][2] * c[4] - m[0][3] * c[3]) * idet;
		T[0][2] = (m[3][1] * s[5] - m[3][2] * s[4] + m[3][3] * s[3]) * idet;
		T[0][3] = (-m[2][1] * s[5] + m[2][2] * s[4] - m[2][3] * s[3]) * idet;

		T[1][0] = (-m[1][0] * c[5] + m[1][2] * c[2] - m[1][3] * c[1]) * idet;
		T[1][1] = (m[0][0] * c[5] - m[0][2] * c[2] + m[0][3] * c[1]) * idet;
		T[1][2] = (-m[3][0] * s[5] + m[3][2] * s[2] - m[3][3] * s[1]) * idet;
		T[1][3] = (m[2][0] * s[5] - m[2][2] * s[2] + m[2][3] * s[1]) * idet;

		T[2][0] = (m[1][0] * c[4] - m[1][1] * c[2] + m[1][3] * c[0]) * idet;
		T[2][1] = (-m[0][0] * c[4] + m[0][1] * c[2] - m[0][3] * c[0]) * idet;
		T[2][2] = (m[3][0] * s[4] - m[3][1] * s[2] + m[3][3] * s[0]) * idet;
		T[2][3] = (-m[2][0] * s[4] + m[2][1] * s[2] - m[2][3] * s[0]) * idet;

		T[3][0] = (-m[1][0] * c[3] + m[1][1] * c[1] - m[1][2] * c[0]) * idet;
		T[3][1] = (m[0][0] * c[3] - m[0][1] * c[1] + m[0][2] * c[0]) * idet;
		T[3][2] = (-m[3][0] * s[3] + m[3][1] * s[1] - m[3][2] * s[0]) * idet;
		T[3][3] = (m[2][0] * s[3] - m[2][1] * s[1] + m[2][2] * s[0]) * idet;

		return T;
	}

	Vector4<_Ty> Mul(Vector4<_Ty> v)
	{
		Vector4<_Ty> output;
		output.x = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w;
		output.y = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w;
		output.z = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w;
		output.w = m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * v.w;

		return output;
	}

	void SetTranslation(const Vector2<_Ty>& vec)
	{
		m[3][0] = vec.x;
		m[3][1] = vec.y;
	}

	void SetTranslation(const Vector3<_Ty>& vec)
	{
		m[3][0] = vec.x;
		m[3][1] = vec.y;
		m[3][2] = vec.z;
	}

	void SetScale(const Vector2<_Ty>& vec)
	{
		m[0][0] = vec.x;
		m[1][1] = vec.y;
	}

	void SetScale(const Vector3<_Ty>& vec)
	{
		m[0][0] = vec.x;
		m[1][1] = vec.y;
		m[2][2] = vec.z;
	}


	void Ortho(_Ty left, _Ty right, _Ty bottom, _Ty top, _Ty back, _Ty front)
	{
		_Ty l = left, r = right, b = bottom, t = top, n = front, f = back;
		_Ty tx = -(r + l) / (r - l);
		_Ty ty = -(t + b) / (t - b);
		_Ty tz = -(f + n) / (f - n);
		*this = Matrix4x4(
			2 / (r - l), 0, 0, tx,
			0, 2 / (t - b), 0, ty,
			0, 0, 2 / (f - n), tz,
			0, 0, 0, 1
		);
	}

	void Perspective(_Ty fov, _Ty aspect, _Ty nearPlane, _Ty farPlane)
	{
		_Ty a = static_cast<_Ty>(1.0 / tan(static_cast<double>(fov) / 2.0));
		_Ty n = nearPlane,
			f = farPlane;

		m[0][0] = a / aspect;
		m[0][1] = 0.0;
		m[0][2] = 0.0;
		m[0][3] = 0.0;

		m[1][0] = 0.0;
		m[1][1] = a;
		m[1][2] = 0.0;
		m[1][3] = 0.0;

		m[2][0] = 0.0;
		m[2][1] = 0.f;
		m[2][2] = -((f + n) / (f - n));
		m[2][3] = -1.0;

		m[3][0] = 0.0;
		m[3][1] = 0.0;
		m[3][2] = -((2.0 * f * n) / (f - n));
		m[3][3] = 0.0;
	}

	union
	{
		_Ty m[4][4];
		_Ty m_flat[16];
	};
};


template<typename _Ty>
Vector4<_Ty> operator*(Matrix4x4<_Ty> m, Vector4<_Ty> v)
{
	return m.Mul(v);
}

template<typename _Ty>
Matrix4x4<_Ty> operator*(Matrix4x4<_Ty> lh, Matrix4x4<_Ty> rh)
{
	return lh.Mul(rh);
}

typedef Matrix4x4<float> Matrix4x4f;