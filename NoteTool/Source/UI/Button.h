#pragma once

#include "Widget.h"
#include <functional>
#include "Utility.h"

namespace gui
{
	class Button : public Widget
	{
	public:

		void GenerateVertexList(DrawList& drawList)
		{

			Colour col = m_Base;

			if (m_Hovered && !m_Clicked)
			{
				col = m_HoveredColour;
			}

			if (m_Clicked)
			{
				col.r += m_Theme->clickModifier.r;
				col.g += m_Theme->clickModifier.g;
				col.b += m_Theme->clickModifier.b;

				m_Clicked = false;
			}

			Colour borderCol = { 1.0f, 1.0f, 1.0f, 1.0f };

			Shape highlight = gui::GenerateRoundedQuad({ m_GlobalBounds.position - Vector2f(0.75f, 0.75f) }, { m_GlobalBounds.position + m_GlobalBounds.size + Vector2f(0.75f, 0.75f) }, m_Highlight, m_Rounding);
	
			Shape bg = gui::GenerateRoundedQuad({ m_GlobalBounds.position }, { m_GlobalBounds.position + m_GlobalBounds.size }, col, m_Rounding);

			drawList.Add(highlight.vertices, highlight.indices);
			drawList.Add(bg.vertices, bg.indices, nullptr);
		}

		void OnEvent() override
		{
			m_Hovered = false;
			if (m_Bounds.Contains((float)EventHandler::x, (float)EventHandler::y))
			{
				m_Hovered = true;
			}

			if (m_Hovered && EventHandler::mouseButton[MouseButton::MOUSE_LEFT].clicks > 0)
			{
				m_Callback();
				m_Clicked = true;
			}

		}
		
		void SetOnClick(std::function<void()> callback) { m_Callback = callback; }

		void SetButtonColour(Colour col) { m_Base = col; }

		void SetHighlightColour(Colour col) { m_Highlight = col; }

		void SetButtonRounding(float rounding) { m_Rounding = rounding; }

		void SetHoveredColour(Colour col) { m_HoveredColour = col; }

	private:

		std::function<void()> m_Callback;

		bool m_Clicked = false;

		Colour m_Base;
		Colour m_Highlight;
		Colour m_HoveredColour;
		float m_Rounding = 6.0f;

	};
}