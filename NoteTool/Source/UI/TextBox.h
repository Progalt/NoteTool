#pragma once
#include "Widget.h"
#include "Utility.h"
#include "EventHandler.h"

namespace gui
{
	class TextBox : public Widget
	{
	public:

		void GenerateVertexList(DrawList& drawList) override
		{
			if (!m_Visible)
				return;

			if (!string.empty() && m_Font)
			{
				Colour col = m_Colour;
				col.a *= GetTransparency();

				gui::RenderText(drawList, string, m_Font, m_GlobalBounds.position, m_GlobalBounds.w, col);
			}

			// Render cursor

			if (m_Editing)
			{
				if (m_CursorTime < m_CursorBlinkTime)
				{
					Vector2f cursorPos = gui::GetPositionOfChar(gui::EventHandler::cursorOffset, string, m_Font, m_GlobalBounds.w);

					cursorPos.x += 1.0f;

					Shape cursor = gui::GenerateQuad(m_GlobalBounds.position + cursorPos,
						{ m_GlobalBounds.position.x + cursorPos.x + 1.0f, m_GlobalBounds.position.y + cursorPos.y - (float)m_Font->GetPixelSize() },
						{ 0.0f, 0.0f }, { 0.0f, 0.0f }, m_Colour);

					drawList.Add(cursor.vertices, cursor.indices);
				}

				m_CursorTime += EventHandler::deltaTime;

				if (m_CursorTime >= 1.0f)
					m_CursorTime = 0.0f;
			}

			GenerateChildVertexLists(drawList);
		}

		void OnEvent() override
		{
			if (!m_Visible)
				return;

			m_Hovered = false;
			if (m_GlobalBounds.Contains((float)EventHandler::x, (float)EventHandler::y))
			{
				m_Hovered = true;
			}

			if (m_Hovered)
			{
				if (EventHandler::mouseButton[MouseButton::MOUSE_LEFT].clicks >= 1)
				{
					m_Editing = true;
					EventHandler::cursorOffset = string.size();
					EventHandler::textInput = &string;
				}
			}

		
		}

		std::string string;

		void SetFont(Font* font) 
		{
			m_Font = font;
		} 

	private:

		Font* m_Font;
		bool m_Editing = false;

		float m_CursorTime = 0.0f;
		float m_CursorBlinkTime = 0.5f;
	};
}