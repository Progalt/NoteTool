#pragma once

#include <cmath>
#include "GenericMaths.h"

template<typename _Ty>
struct Vector2
{
	Vector2() : x(0), y(0) { }
	Vector2(_Ty _x, _Ty _y) : x(_x), y(_y) { }

	template<typename _Ty2>
	Vector2(Vector2<_Ty2> v) :
		x(static_cast<_Ty>(v.x)),
		y(static_cast<_Ty>(v.y))
	{

	}


	size_t GetSize() const { return 2; }

	_Ty Length()
	{
		return static_cast<_Ty>(std::sqrt(static_cast<double>(x * x + y * y)));
	}

	Vector2<_Ty> Normalize()
	{
		_Ty len = Length();

		if (len != 0)
		{
			x /= len;
			y /= len;

		}


		return *this;
	}

	_Ty Distance(const Vector2<_Ty> vec)
	{
		_Ty xdif = x - vec.x;
		_Ty ydif = y - vec.y;

		_Ty x2 = xdif * xdif;
		_Ty y2 = ydif * ydif;

		return static_cast<_Ty>(std::sqrt(static_cast<double>(x2 + y2)));
	}

	_Ty& operator[](size_t idx)
	{
		return v[idx];
	}

	union
	{
		struct
		{
			_Ty x, y;
		};

		_Ty v[2];
	};


	// Operator overloads

	Vector2<_Ty> operator+=(Vector2<_Ty> rh)
	{
		x += rh.x;
		y += rh.y;

		return *this;
	}

	Vector2<_Ty> operator-=(Vector2<_Ty> rh)
	{
		x -= rh.x;
		y -= rh.y;

		return *this;
	}

	Vector2<_Ty> operator*=(Vector2<_Ty> rh)
	{
		x *= rh.x;
		y *= rh.y;

		return *this;
	}

	Vector2<_Ty> operator/=(Vector2<_Ty> rh)
	{
		x /= rh.x;
		y /= rh.y;

		return *this;
	}




	bool operator==(Vector2<_Ty> v)
	{
		if (x == v.x && y == v.y)
		{
			return true;
		}

		return false;
	}

	bool operator!=(Vector2<_Ty> v)
	{
		if (x != v.x && y != v.y)
		{
			return true;
		}

		return false;
	}
};

template<typename _Ty, typename _Ty2>
Vector2<_Ty> operator+(const Vector2<_Ty>& lh, const Vector2<_Ty2>& rh)
{
	Vector2<_Ty> out;
	out.x = lh.x + static_cast<_Ty>(rh.x);
	out.y = lh.y + static_cast<_Ty>(rh.y);
	return out;
}

template<typename _Ty, typename _Ty2>
Vector2<_Ty> operator-(const Vector2<_Ty>& lh, const Vector2<_Ty2>& rh)
{
	Vector2<_Ty> out;
	out.x = lh.x - static_cast<_Ty>(rh.x);
	out.y = lh.y - static_cast<_Ty>(rh.y);
	return out;
}

template<typename _Ty, typename _Ty2>
Vector2<_Ty> operator*(const Vector2<_Ty>& lh, const Vector2<_Ty2>& rh)
{
	Vector2<_Ty> out;
	out.x = lh.x * static_cast<_Ty>(rh.x);
	out.y = lh.y * static_cast<_Ty>(rh.y);
	return out;
}

template<typename _Ty, typename _Ty2>
Vector2<_Ty> operator/(const Vector2<_Ty>& lh, const Vector2<_Ty2>& rh)
{
	Vector2<_Ty> out;
	out.x = lh.x / static_cast<_Ty>(rh.x);
	out.y = lh.y / static_cast<_Ty>(rh.y);
	return out;
}

template<typename _Ty>
Vector2<_Ty> operator*(const Vector2<_Ty>& lh, _Ty v)
{
	Vector2<_Ty> out;
	out.x = lh.x * v;
	out.y = lh.y * v;
	return out;
}

template<typename _Ty>
Vector2<_Ty> operator/(const Vector2<_Ty>& lh, _Ty v)
{
	Vector2<_Ty> out;
	out.x = lh.x / v;
	out.y = lh.y / v;
	return out;
}

typedef Vector2<float> Vector2f;
typedef Vector2<int> Vector2i;
typedef Vector2<unsigned int> Vector2u;