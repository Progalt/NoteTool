#pragma once
#include "Widget.h"
#include "Utility.h"
#include "EventHandler.h"
#include "Formatter.h"
#include <functional>

namespace gui
{
	class TextBox : public Widget
	{
	public:

		void GenerateVertexList(DrawList& drawList) override
		{
			if (!m_Visible || m_GlobalBounds.w <= 0.0f || m_GlobalBounds.h <= 0.0f)
				return;

			float cursorMod = 4.0f;

			if (EventHandler::selectionStart > string.size())
				EventHandler::selectionStart = EventHandler::cursorOffset;

			// Draw selection box
			if (EventHandler::selectionStart != EventHandler::cursorOffset && m_Editing)
			{
				uint32_t firstLine = gui::GetLineOfChar(EventHandler::selectionStart, text.formattedString, m_Font, m_GlobalBounds.w);
				uint32_t lastLine = gui::GetLineOfChar(EventHandler::cursorOffset, text.formattedString, m_Font, m_GlobalBounds.w);

				uint32_t first, last;
				first = std::min(firstLine, lastLine);
				last = std::max(firstLine, lastLine);

				uint32_t startSelection = std::min(EventHandler::selectionStart, EventHandler::cursorOffset);
				uint32_t endSelection = std::max(EventHandler::selectionStart, EventHandler::cursorOffset);

				if (first == last)
				{
					Vector2f startPos = gui::GetPositionOfCharFormatted(startSelection, text.formattedString, m_FontManager, m_FontSize, m_DefaultWeight, m_GlobalBounds.w, m_Formats);
					Vector2f endPos = gui::GetPositionOfCharFormatted(endSelection, text.formattedString, m_FontManager, m_FontSize, m_DefaultWeight, m_GlobalBounds.w, m_Formats);

					endPos.y += (float)m_Font->GetPixelSize();

					Shape selection = gui::GenerateQuad(m_GlobalBounds.position + startPos, m_GlobalBounds.position + endPos + Vector2f{ 0.0f, cursorMod }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.1f, 0.1f, 0.1f, 1.0f });

					drawList.Add(selection.vertices, selection.indices);
				}
				else
				{
					for (uint32_t i = first; i <= last; i++)
					{
						uint32_t firstOfLine = gui::GetLastIdxOfLine(i - 1, text.formattedString, m_Font, m_GlobalBounds.w) + 2;
						uint32_t lastOfLine = gui::GetLastIdxOfLine(i, text.formattedString, m_Font, m_GlobalBounds.w) + 1;

						if (i == first)
						{
							Vector2f startPos = gui::GetPositionOfCharFormatted(startSelection, text.formattedString, m_FontManager, m_FontSize, m_DefaultWeight, m_GlobalBounds.w, m_Formats);
							Vector2f endPos = gui::GetPositionOfCharFormatted(lastOfLine, text.formattedString, m_FontManager, m_FontSize, m_DefaultWeight, m_GlobalBounds.w, m_Formats);

							endPos.y += (float)m_Font->GetPixelSize();

							Shape selection = gui::GenerateQuad(m_GlobalBounds.position + startPos, m_GlobalBounds.position + endPos + Vector2f{ 0.0f, cursorMod }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.1f, 0.1f, 0.1f, 1.0f });

							drawList.Add(selection.vertices, selection.indices);
						}
						else if (i == last)
						{
							Vector2f startPos = gui::GetPositionOfCharFormatted(firstOfLine, text.formattedString, m_FontManager, m_FontSize, m_DefaultWeight, m_GlobalBounds.w, m_Formats);
							Vector2f endPos = gui::GetPositionOfCharFormatted(endSelection, text.formattedString, m_FontManager, m_FontSize, m_DefaultWeight, m_GlobalBounds.w, m_Formats);

							endPos.y += (float)m_Font->GetPixelSize();

							Shape selection = gui::GenerateQuad(m_GlobalBounds.position + startPos, m_GlobalBounds.position + endPos + Vector2f{ 0.0f, cursorMod }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.1f, 0.1f, 0.1f, 1.0f });

							drawList.Add(selection.vertices, selection.indices);

						}
						else
						{
							Vector2f startPos = gui::GetPositionOfCharFormatted(firstOfLine, text.formattedString, m_FontManager, m_FontSize, m_DefaultWeight, m_GlobalBounds.w, m_Formats);
							Vector2f endPos = gui::GetPositionOfCharFormatted(lastOfLine, text.formattedString, m_FontManager, m_FontSize, m_DefaultWeight, m_GlobalBounds.w, m_Formats);

							endPos.y += (float)m_Font->GetPixelSize();

							Shape selection = gui::GenerateQuad(m_GlobalBounds.position + startPos, m_GlobalBounds.position + endPos + Vector2f{ 0.0f, cursorMod }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.1f, 0.1f, 0.1f, 1.0f });

							drawList.Add(selection.vertices, selection.indices);
						}

					}
				}
			}



			/*if (!string.empty() && m_Font)
			{
				Colour col = m_Colour;
				col.a *= GetTransparency();

				Vector2f pos = m_GlobalBounds.position;
				pos.y += m_Font->GetPixelSize();
				gui::RenderText(drawList, string, m_Font, pos, m_GlobalBounds.w, col, m_GlobalBounds);
			}*/

			if (m_FullRerender && !string.empty() && m_FontManager)
			{


				text.RasterizeTextFormatted(string, m_FontManager, m_FontSize, m_Bounds.w, m_Formats, m_DefaultWeight);
				//text.RasterizeText(string, m_FontManager->Get(gui::FontWeight::Light, m_FontSize), m_Bounds.w);

				m_FullRerender = false;
			}

			if (!string.empty() && m_Font)
			{
				Colour col = m_Colour;
				col.a *= GetTransparency();

				Vector2f position = m_GlobalBounds.position;
				//position.y += m_Font->GetPixelSize() + m_Font->GetAscent();
				Shape quad = gui::GenerateQuad(position, position + text.textBounds.size, { 0.0f, 0.0f }, { 1.0f, 1.0f }, col);

				drawList.Add(quad.vertices, quad.indices, &text.texture);
			}

			// Render cursor

			if (m_Editing)
			{
				if (m_CursorTime < m_CursorBlinkTime)
				{
					Vector2f cursorPos = gui::GetPositionOfCharFormatted(gui::EventHandler::cursorOffset, text.formattedString, m_FontManager, m_FontSize, m_DefaultWeight, m_GlobalBounds.w, m_Formats);

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
			
			static bool dragging = false;
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

					EventHandler::selecting = false;
				}
				

				// double click so select word
				if (EventHandler::mouseButton[MouseButton::MOUSE_LEFT].clicks == 2)
				{

				}

				if (EventHandler::mouseButton[MouseButton::MOUSE_LEFT].down)
				{
					dragging = true;
				}
				else
				{
					dragging = false;
				}

			}
			else
			{
				if (EventHandler::mouseButton[MouseButton::MOUSE_LEFT].clicks == 1)
				{
					// We have lost focus
					m_Editing = false;

					if (m_OnLoseFocus)
						m_OnLoseFocus();
				}
			}
			

			if (EventHandler::mouseButton[MouseButton::MOUSE_LEFT].down && dragging && m_Editing)
			{

				Vector2f mpoint = Vector2f((float)EventHandler::x, (float)EventHandler::y);
				mpoint = mpoint - m_GlobalBounds.position;

				// if the mouse is above the bounds of the text box lock the position to just within the 
				// textbox on the y axis
				if (mpoint.y < 0.0f)
					mpoint.y = 4.0f;

				if (mpoint.y > m_GlobalBounds.h)
					mpoint.y = m_GlobalBounds.h - 5.0f;

				
				EventHandler::selectionStart = gui::GetNearestCharFromPoint(mpoint, string, m_Font, m_GlobalBounds.w);

				EventHandler::selecting = true;

				if (EventHandler::selectionStart > EventHandler::textInput->size())
					EventHandler::selectionStart = EventHandler::textInput->size();
			}

			if (string != m_CachedString || m_Bounds != m_CachedBounds)//|| m_Formats != m_CachedFormat)
				TriggerRerender();
		}

		std::string string;

		void TriggerRerender()
		{
			m_FullRerender = true;

			m_CachedString = string;
			m_CachedBounds = m_Bounds;
			m_CachedFormat = m_Formats;

			if (m_OnEdit)
				m_OnEdit();
		}

		void SetFontManager(FontManager* font)
		{
			m_FontManager = font;
		} 

		void SetFontSize(uint32_t size, FontWeight defaultWeight = FontWeight::ExtraLight)
		{
			m_Font = m_FontManager->Get(defaultWeight, size);
			m_FontSize = size;
			m_DefaultWeight = defaultWeight;
			m_FullRerender = true;

		}

		void SetFormatting(std::vector< TextFormat> f)
		{
			m_Formats = f;
			TriggerRerender();
		}

		void SetOnEditCallback(std::function<void()> func)
		{
			m_OnEdit = func;
		}

		void SetOnOnLoseCallback(std::function<void()> func)
		{
			m_OnLoseFocus = func;
		}

	private:

		std::function<void()> m_OnEdit;
		std::function<void()> m_OnLoseFocus;

		std::string m_CachedString;
		FloatRect m_CachedBounds;
		std::vector< TextFormat> m_CachedFormat;

		FontManager* m_FontManager;
		Font* m_Font;
		uint32_t m_FontSize;
		FontWeight m_DefaultWeight = FontWeight::ExtraLight;
		bool m_Editing = false;

		float m_CursorTime = 0.0f;
		float m_CursorBlinkTime = 0.5f;

		bool m_FullRerender = true;

		std::vector< TextFormat> m_Formats;

		struct TextEntry
		{
			GPUTexture texture;
			Image image;
			bool rerender = true;
			FloatRect textBounds;
			std::vector<TextFormat> formatting;

			std::string formattedString;


			void RasterizeText(const std::string& str, Font* font, float textWrap)
			{
				textBounds.w = textWrap + gui::TextPadding;
				textBounds.h = gui::TextPadding + ((gui::GetLineCount(str, font, textWrap) + 1) * font->GetLineSpacing());

				image.New((int)textBounds.w, textBounds.h, 4);
				image.Fill({ 0.0f, 0.0f, 0.0f, 0.0f });

				float baseLine = (float)font->GetAscent();

				printf("Rerasterising Textbox...\n");

				gui::RenderTextSoftware(image, str, font, {}, textWrap, { 1.0f, 1.0f, 1.0f, 1.0f }, textBounds, baseLine);


				texture.CreateFromImage(image);

				
			}

			void RasterizeTextFormatted(const std::string& str, FontManager* fontManager, uint32_t fontSize, float textWrap, std::vector<TextFormat> formatting, FontWeight defaultWeight)
			{
				Font* font = fontManager->Get(gui::FontWeight::Bold, fontSize);

				//gui::Formatter formatter(str);
				//formattedString = formatter.GetStringWithFormatting();

				formattedString = str;

				textBounds.w =   textWrap + gui::TextPadding;
				float height = gui::TextPadding + ((gui::GetLineCount(formattedString, font, textWrap) + 1) * font->GetLineSpacing());

				float baseLine = (float)font->GetAscent();

				textBounds.h = gui::GetTextBoxSizeFormatted(formattedString, fontManager, fontSize,
					defaultWeight, {}, textWrap, { 1.0f, 1.0f, 1.0f, 1.0f }, textBounds, baseLine, formatting).y;

				textBounds.size += { gui::TextPadding, gui::TextPadding };

				image.New((int)textBounds.w, textBounds.h, 4);
				image.Fill({ 0.0f, 0.0f, 0.0f, 0.0f });

				

				printf("Rerasterising Textbox...\n");


				gui::RenderTextSoftwareFormatted(image, formattedString, fontManager, fontSize, 
					defaultWeight, {}, textWrap, { 1.0f, 1.0f, 1.0f, 1.0f }, textBounds, baseLine, formatting);


				texture.CreateFromImage(image);


			}
			
		};

		TextEntry text;
	};
}