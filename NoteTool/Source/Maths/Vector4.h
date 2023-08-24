#pragma once

#include "Vector3.h"

template<typename _Ty>
class Vector4
{
public:
	Vector4() : x(0), y(0), z(0), w(0) { }
	Vector4(_Ty _x, _Ty _y, _Ty _z, _Ty _w) : x(_x), y(_y), z(_z), w(_w) { }

	Vector4(const Vector2<_Ty>& v)
	{
		x = v.x;
		y = v.y;
		z = 0.0;
		w = 1.0;
	}

	Vector4(const Vector3<_Ty>& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = 1.0;
	}


	const size_t GetSize() const { return 4; }

	_Ty& operator[](size_t idx)
	{
		return v[idx];
	}

	union
	{
		struct
		{
			_Ty x, y, z, w;
		};

		struct
		{
			Vector3<_Ty> xyz;
			_Ty w;
		};

		struct
		{
			Vector2<_Ty> xy;
			Vector2<_Ty> zw;
		};

		_Ty v[4];
	};

private:
};

typedef Vector4<float> Vector4f;
typedef Vector4<int> Vector4i;
typedef Vector4<unsigned int> Vector4u;