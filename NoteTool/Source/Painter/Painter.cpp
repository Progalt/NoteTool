
#include "Painter.h"


void Painter::Fill(const Colour& col)
{
	m_Target->Fill(col);
}

void Painter::DrawLine(const Vector2f& p1, const Vector2f& p2)
{
	Colour col = m_CurrentPen->GetColour();

	switch (m_CurrentPen->GetLineMode())
	{
	case LineMode::None:

		break;
	case LineMode::Antialiasing:

		break;
	}
}

void Painter::DrawRect(const FloatRect& rect)
{

}