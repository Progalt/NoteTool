#pragma once

#include "Widget.h"
#include <functional>
#include "Utility.h"
#include "../Font.h"

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
			

				m_Clicked = false;
			}

			Colour borderCol = { 1.0f, 1.0f, 1.0f, 1.0f };


			col.a *= GetTransparency();

			Shape bg = gui::GenerateRoundedQuad({ m_GlobalBounds.position }, { m_GlobalBounds.position + m_GlobalBounds.size }, col, m_Rounding);

			if (m_ShadowColour.a != 0.0f)
			{
				Colour shadowCol = m_ShadowColour;
				shadowCol.a *= GetTransparency();
				Shape shadow = gui::GenerateRoundedQuad({ m_GlobalBounds.position - Vector2f(0.75f, 0.0f) }, { m_GlobalBounds.position + m_GlobalBounds.size + Vector2f(0.75f, 1.0f) }, shadowCol, m_Rounding);
				drawList.Add(shadow.vertices, shadow.indices);
			}

			if (m_Highlight.a != 0.0f && m_ShadowColour.a != 0.0f)
			{
				Colour highlightCol = m_Highlight;
				highlightCol.a *= GetTransparency();
				Shape highlight = gui::GenerateRoundedQuad({ m_GlobalBounds.position - Vector2f(0.75f, 1.0f) }, { m_GlobalBounds.position + m_GlobalBounds.size + Vector2f(0.75f, -1.0f) }, highlightCol, m_Rounding);
				drawList.Add(highlight.vertices, highlight.indices);
			}
			else if (m_Highlight.a != 0.0f && m_ShadowColour.a == 0.0f)
			{
				Colour highlightCol = m_Highlight;
				highlightCol.a *= GetTransparency();
				Shape highlight = gui::GenerateRoundedQuad({ m_GlobalBounds.position - Vector2f(0.75f, 1.0f) }, { m_GlobalBounds.position + m_GlobalBounds.size + Vector2f(0.75f, 1.0f) }, highlightCol, m_Rounding);
				drawList.Add(highlight.vertices, highlight.indices);
			}
			

			drawList.Add(bg.vertices, bg.indices, nullptr);

			if (!m_Text.str.empty())
			{
				float lineSpacing = (float)m_Text.font->GetLineSpacing();
				Vector2i position;
				position.y = m_GlobalBounds.y + lineSpacing;
				float textLength = gui::GetTextLength(m_Text.str, m_Text.font);

				switch (m_Text.alignment)
				{
				case Alignment::Centre:
					position.x = m_GlobalBounds.x + (m_GlobalBounds.w / 2.0f) - textLength / 2.0f ;
					break;
				case Alignment::Right:
					break;
				case Alignment::Left:
					position.x = m_GlobalBounds.x;
					break;
				}

				gui::RenderText(drawList, m_Text.str, m_Text.font, position, 0.0f, Colour{ 1.0f, 1.0f, 1.0f, 1.0f });
			}
		}

		void OnEvent() override
		{
			m_Hovered = false;
			if (m_GlobalBounds.Contains((float)EventHandler::x, (float)EventHandler::y))
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
		void SetShadowColour(Colour col) { m_ShadowColour = col; }


		void SetHoveredColour(Colour col) { m_HoveredColour = col; }

		void SetText(const std::string& str, Font* font, Alignment alignment = Alignment::Centre)
		{
			m_Text.str = str;
			m_Text.font = font;
			m_Text.alignment = alignment;
		}

	private:

		std::function<void()> m_Callback;

		bool m_Clicked = false;

		Colour m_Base;
		Colour m_Highlight = Colour(1.0f, 1.0f, 1.0f, 0.0f);
		Colour m_HoveredColour;
		Colour m_ShadowColour = Colour(1.0f, 1.0f, 1.0f, 0.0f);

		struct
		{
			std::string str = "";
			Font* font;
			Alignment alignment;
		} m_Text;

	};
}