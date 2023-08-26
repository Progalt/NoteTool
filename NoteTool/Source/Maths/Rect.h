#pragma once
#include "Vector2.h"

template<typename _Ty>
class Rect
{
public:
	Rect() : x(0), y(0), w(0), h(0) { }
	Rect(_Ty x, _Ty y, _Ty w, _Ty h) : x(x), y(y), w(w), h(h) { }

	template<typename _Ty2>
	Rect(Rect<_Ty2> rt2)
	{
		x = static_cast<_Ty>(rt2.x);
		y = static_cast<_Ty>(rt2.y);
		w = static_cast<_Ty>(rt2.w);
		h = static_cast<_Ty>(rt2.h);
	}

	const bool IsNull() const
	{
		if (x == 0 && y == 0 && w == 0 && h == 0)
			return true;

		return false;
	}

	bool Contains(_Ty px, _Ty py)
	{
		if (px > x && px < x + w && py > y && py < y + h)
			return true;

		return false;
	}

	bool Contains(Rect<_Ty> r2)
	{
		if (Contains(r2.x, r2.y) && Contains(r2.x + r2.w, r2.y + r2.h))
			return true;

		return false;
	}

	bool Intersects(Rect<_Ty> r2)
	{
		auto valueInRange = [](_Ty value, _Ty min, _Ty max)
			{
				return (value >= min) && (value <= max);
			};

		bool xOverlap = valueInRange(x, r2.x, r2.x + r2.w) ||
			valueInRange(r2.x, x, x + w);

		bool yOverlap = valueInRange(y, r2.y, r2.y + r2.h) ||
			valueInRange(r2.y, y, y + h);

		return xOverlap && yOverlap;
	}

	union
	{
		struct
		{
			_Ty x, y, w, h;
		};

		struct
		{
			Vector2<_Ty> position;
			Vector2<_Ty> size;
		};
	};



};

typedef Rect<float> FloatRect;
typedef Rect<int> IntRect;


template<typename _Ty>
bool operator==(Rect<_Ty> lh, Rect<_Ty> rh)
{
	return (lh.x == rh.x && lh.y == rh.y && lh.w == rh.w && lh.h == rh.h);
}

template<typename _Ty>
bool operator !=(Rect<_Ty> lh, Rect<_Ty> rh)
{
	return !(lh == rh);
}