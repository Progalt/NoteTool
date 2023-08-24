#pragma once

#include "Vector2.h"

template<typename _Ty>
class Vector3
{
public:
	Vector3() : x(0), y(0), z(0) { }
	Vector3(_Ty _x, _Ty _y, _Ty _z) : x(_x), y(_y), z(_z) { }

	_Ty& operator[](size_t idx)
	{
		return v[idx];
	}

	union
	{
		struct
		{
			_Ty x, y, z;
		};

		// This allows different swizzling so you can access it like vec3.xy to get it as a vector2
		struct
		{
			Vector2<_Ty> xy;
			_Ty z;
		};

		struct
		{
			_Ty x;
			Vector2<_Ty> yz;
		};

		_Ty v[3];
	};
};

template<typename _Ty>
Vector3<_Ty> operator*(const Vector3<_Ty>& lh, _Ty v)
{
	Vector3<_Ty> out;
	out.x = lh.x * v;
	out.y = lh.y * v;
	out.z = lh.z * v;
	return out;
}

template<typename _Ty>
Vector3<_Ty> operator/(const Vector3<_Ty>& lh, _Ty v)
{
	Vector3<_Ty> out;
	out.x = lh.x / v;
	out.y = lh.y / v;
	out.z = lh.z / v;
	return out;
}

typedef Vector3<float> Vector3f;
typedef Vector3<int> Vector3i;
typedef Vector3<unsigned int> Vector3u;