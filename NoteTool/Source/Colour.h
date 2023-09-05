#pragma once

#include <cmath>

// Really simple colour class
// Colours are internally stored as 0 - 1 floats 
class Colour
{
public:

	Colour() { }
	Colour(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) { }
	Colour(int r, int g, int b, int a = 255)
	{
		this->r = static_cast<float>(r) / 255.0f;
		this->g = static_cast<float>(g) / 255.0f;
		this->b = static_cast<float>(b) / 255.0f;
		this->a = static_cast<float>(a) / 255.0f;
	}

	unsigned char AsByte(unsigned int idx)
	{
		return static_cast<unsigned char>(c[idx] * 255.0f);
	}

	// Convert the colour to an srgb colour space
	void ToSRGB()
	{
		r = powf(r, 1.0f / 2.0f);
		g = powf(g, 1.0f / 2.0f);
		b = powf(b, 1.0f / 2.0f);
	}

	union
	{
		struct
		{
			float r, g, b, a;
		};
		float c[4];
	};
};


inline Colour operator+(Colour col, Colour col2)
{
	Colour out;
	out.r = col.r + col2.r;
	out.g = col.g + col2.g;
	out.b = col.b + col2.b;
	out.a = col.a + col2.a;
	return out;
}

inline bool operator==(Colour c, Colour c2)
{
	if (c.r == c2.r && c.g == c2.g && c.b == c2.b && c.a == c2.a)
		return true;

	return false;
}

inline bool operator!=(Colour c, Colour c2)
{
	if (c.r != c2.r && c.g != c2.g && c.b != c2.b && c.a != c2.a)
		return true;

	return false;
}