
#include "Painter.h"


void Painter::Fill(const Colour& col)
{
	m_Target->Fill(col);
}

void Painter::DrawLine(const Vector2f& p1, const Vector2f& p2)
{
	
}

void Painter::DrawRect(const FloatRect& rect)
{
	bool renderPen = (m_CurrentPen != nullptr);
	bool fill = (m_CurrentBrush != nullptr && m_CurrentBrush->GetMode() == BrushMode::Solid);
}