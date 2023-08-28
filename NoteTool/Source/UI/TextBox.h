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

			float cursorMod = 4.0f;

			if (EventHandler::selectionStart > string.size())
				EventHandler::selectionStart = EventHandler::cursorOffset;

			// Draw selection box
			if (EventHandler::selectionStart != EventHandler::cursorOffset)
			{
				uint32_t firstLine = gui::GetLineOfChar(EventHandler::selectionStart, string, m_Font, m_GlobalBounds.w);
				uint32_t lastLine = gui::GetLineOfChar(EventHandler::cursorOffset, string, m_Font, m_GlobalBounds.w);

				uint32_t first, last;
				first = std::min(firstLine, lastLine);
				last = std::max(firstLine, lastLine);

				uint32_t startSelection = std::min(EventHandler::selectionStart, EventHandler::cursorOffset);
				uint32_t endSelection = std::max(EventHandler::selectionStart, EventHandler::cursorOffset);

				if (first == last)
				{
					Vector2f startPos = gui::GetPositionOfChar(startSelection, string, m_Font, m_GlobalBounds.w);
					Vector2f endPos = gui::GetPositionOfChar(endSelection, string, m_Font, m_GlobalBounds.w);

					endPos.y += (float)m_Font->GetPixelSize();

					Shape selection = gui::GenerateQuad(m_GlobalBounds.position + startPos, m_GlobalBounds.position + endPos + Vector2f{ 0.0f, cursorMod }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.1f, 0.1f, 0.1f, 1.0f });

					drawList.Add(selection.vertices, selection.indices);
				}
				else
				{
					for (uint32_t i = first; i <= last; i++)
					{
						uint32_t firstOfLine = gui::GetLastIdxOfLine(i - 1, string, m_Font, m_GlobalBounds.w) + 2;
						uint32_t lastOfLine = gui::GetLastIdxOfLine(i, string, m_Font, m_GlobalBounds.w) + 1;

						if (i == first)
						{
							Vector2f startPos = gui::GetPositionOfChar(startSelection, string, m_Font, m_GlobalBounds.w);
							Vector2f endPos = gui::GetPositionOfChar(lastOfLine, string, m_Font, m_GlobalBounds.w);

							endPos.y += (float)m_Font->GetPixelSize();

							Shape selection = gui::GenerateQuad(m_GlobalBounds.position + startPos, m_GlobalBounds.position + endPos + Vector2f{ 0.0f, cursorMod }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.1f, 0.1f, 0.1f, 1.0f });

							drawList.Add(selection.vertices, selection.indices);
						}
						else if (i == last)
						{
							Vector2f startPos = gui::GetPositionOfChar(firstOfLine, string, m_Font, m_GlobalBounds.w);
							Vector2f endPos = gui::GetPositionOfChar(endSelection, string, m_Font, m_GlobalBounds.w);

							endPos.y += (float)m_Font->GetPixelSize();

							Shape selection = gui::GenerateQuad(m_GlobalBounds.position + startPos, m_GlobalBounds.position + endPos + Vector2f{ 0.0f, cursorMod }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.1f, 0.1f, 0.1f, 1.0f });

							drawList.Add(selection.vertices, selection.indices);

						}
						else
						{
							Vector2f startPos = gui::GetPositionOfChar(firstOfLine, string, m_Font, m_GlobalBounds.w);
							Vector2f endPos = gui::GetPositionOfChar(lastOfLine, string, m_Font, m_GlobalBounds.w);

							endPos.y += (float)m_Font->GetPixelSize();

							Shape selection = gui::GenerateQuad(m_GlobalBounds.position + startPos, m_GlobalBounds.position + endPos + Vector2f{ 0.0f, cursorMod }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.1f, 0.1f, 0.1f, 1.0f });

							drawList.Add(selection.vertices, selection.indices);
						}

					}
				}
			}

			

			if (!string.empty() && m_Font)
			{
				Colour col = m_Colour;
				col.a *= GetTransparency();

				Vector2f pos = m_GlobalBounds.position;
				pos.y += m_Font->GetPixelSize();
				gui::RenderText(drawList, string, m_Font, pos, m_GlobalBounds.w, col);
			}

		
			// Render cursor

			if (m_Editing)
			{
				if (m_CursorTime < m_CursorBlinkTime)
				{
					Vector2f cursorPos = gui::GetPositionOfChar(gui::EventHandler::cursorOffset, string, m_Font, m_GlobalBounds.w);

					//cursorPos.x += 1.0f;

					Shape cursor = gui::GenerateQuad(m_GlobalBounds.position + cursorPos,
						{ m_GlobalBounds.position.x + cursorPos.x + 1.0f, m_GlobalBounds.position.y + cursorPos.y + (float)m_Font->GetPixelSize() + cursorMod },
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
			

			m_Hovered = false;
			if (m_GlobalBounds.Contains((float)EventHandler::x, (float)EventHandler::y))
			{
				m_Hovered = true;
			}

			if (m_Hovered)
			{
				
				
				if (EventHandler::mouseButton[MouseButton::MOUSE_LEFT].clicks == 1)
				{
					m_Editing = true;
					Vector2f mpoint = Vector2f((float)EventHandler::x, (float)EventHandler::y);
					mpoint = mpoint - m_GlobalBounds.position;
					EventHandler::cursorOffset = gui::GetNearestCharFromPoint(mpoint, string, m_Font, m_GlobalBounds.w) + 1;
					EventHandler::textInput = &string;

					if (EventHandler::cursorOffset > EventHandler::textInput->size())
						EventHandler::cursorOffset = EventHandler::textInput->size();

					EventHandler::selectionStart = EventHandler::cursorOffset;
				}

				// double click so select word
				if (EventHandler::mouseButton[MouseButton::MOUSE_LEFT].clicks == 2)
				{

				}

				if (EventHandler::mouseButton[MouseButton::MOUSE_LEFT].down)
				{
					Vector2f mpoint = Vector2f((float)EventHandler::x, (float)EventHandler::y);
					mpoint = mpoint - m_GlobalBounds.position;

					EventHandler::selectionStart = gui::GetNearestCharFromPoint(mpoint, string, m_Font, m_GlobalBounds.w) + 1;

					EventHandler::selecting = true;

					if (EventHandler::selectionStart > EventHandler::textInput->size())
						EventHandler::selectionStart = EventHandler::textInput->size();
				}

			}

		
		}

		std::string string;

		void SetFontManager(FontManager* font)
		{
			m_FontManager = font;
		} 

		void SetFontSize(uint32_t size)
		{
			m_Font = m_FontManager->Get(gui::FontWeight::ExtraLight, size);
		}

	private:

		FontManager* m_FontManager;
		Font* m_Font;
		bool m_Editing = false;

		float m_CursorTime = 0.0f;
		float m_CursorBlinkTime = 0.5f;
	};
}