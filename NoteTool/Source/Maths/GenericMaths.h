#pragma once
#include "../Colour.h"


template<typename _Ty>
inline _Ty Lerp(_Ty a, _Ty b, _Ty t)
{
	return a + t * (b - a);
}

template<typename _Ty>
inline Colour Lerp(Colour a, Colour b, _Ty t)
{
	Colour out;
	out.r = Lerp(a.r, b.r, t);
	out.g = Lerp(a.g, b.g, t);
	out.b = Lerp(a.b, b.b, t);
	out.a = Lerp(a.a, b.a, t);
	return out;
}