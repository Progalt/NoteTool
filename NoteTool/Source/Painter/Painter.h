#pragma once

#include "../Image.h"
#include "Brush.h"
#include "../Maths/Rect.h"

enum class PointMode
{
	Pixel, 
	Normalised
};

// The painter is for software rasterisation 
// It provides some standard painting routines 
// Sort of inspired by Qt's QPainter
class Painter
{
public:


	// Sets the current target for the painter to draw to
	void SetTarget(Image* img) { m_Target = img; }

	void SetPen(Pen* pen) { m_CurrentPen = pen; }
	void SetBrush(Brush* brush) { m_CurrentBrush = brush; }


	// Any pixels outside of this rectangle will be clipped and not drawn
	void SetClippingRect(IntRect clippingRect) { m_ClippingRect = clippingRect; }

	void SetPointMode(PointMode mode) { m_PointMode = mode; }

	// --- DRAWING FUNCTIONS ---

	void Fill(const Colour& col);

	void DrawLine(const Vector2f& p1, const Vector2f& p2);

	void DrawRect(const FloatRect& rect);

private:

	Image* m_Target;


	Brush* m_CurrentBrush = nullptr;	// The internal filling of the shape
	Pen* m_CurrentPen = nullptr;		// Pen determines how the outline should be drawn if any

	IntRect m_ClippingRect = IntRect();

	PointMode m_PointMode;
};