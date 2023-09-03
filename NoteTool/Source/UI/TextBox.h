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

			float cursorMod = 2.0f;

			if (m_Format)
			{
				text.formattedString = string;
			}

			Colour col = { 1.0f, 1.0f, 1.0f, 1.0f };
			std::string str = m_Format ? text.formattedString : string;
			bool dim = false;
			if (str.empty() && !m_BlankText.empty())
			{
				str = m_BlankText;
				dim = true;
			}

			if (m_FullRerender)
			{
				
				if (m_Format)
				{
					gui::Formatter formatter(string);

					m_Formats = formatter.GetFormattingForBaseString();
					text.formattedString = string;
				}

				uint32_t min = 0;
				uint32_t max = string.size();

				if (!m_Editing)
					max = 0;

				

				Colour col = { 1.0f, 1.0f, 1.0f, 1.0f };
				if (dim)
				{
					col = { 0.25f, 0.25f, 0.25f, 1.0f };
				}

				text.RasterizeTextFormatted(str, m_FontManager, m_CodeFontManager, m_FontSize, m_Bounds.w, m_Formats, m_DefaultWeight, min, max, col);
				 
					//text.RasterizeText(string, m_FontManager->Get(gui::FontWeight::Light, m_FontSize), m_Bounds.w);

				m_FullRerender = false;
			}

			// Render formatted text elements that have "extra bits"

			for (auto& format : m_Formats)
			{
				if (format.option == TextFormatOption::InlineCode)
				{
					uint32_t offset = 1;

					if (m_Editing)
						offset = 0;

					float padding = 4.0f;

					Vector2f start = m_GlobalBounds.position + gui::GetPositionOfCharFormatted(format.start - offset, string, m_FontManager, m_CodeFontManager, m_FontSize, m_DefaultWeight, m_GlobalBounds.w, m_Formats);
					Vector2f end = m_GlobalBounds.position + gui::GetPositionOfCharFormatted(format.end - offset + 1, string, m_FontManager, m_CodeFontManager, m_FontSize, m_DefaultWeight, m_GlobalBounds.w, m_Formats);
					start.y -= m_FontManager->Get(m_DefaultWeight, m_FontSize)->GetAscent();
					end.y -= m_FontManager->Get(m_DefaultWeight, m_FontSize)->GetAscent();

					start.x -= 4.0f;
					end.y += m_FontManager->Get(m_DefaultWeight, m_FontSize)->GetMaxHeight();

					Shape bg = gui::GenerateRoundedQuad(start, end, { 0.04f, 0.04f, 0.04f, 1.0f}, 2.0f);

					drawList.Add(bg.vertices, bg.indices);
				}

				if (format.option == TextFormatOption::CodeBlock)
				{
					Vector2f start = m_GlobalBounds.position + gui::GetPositionOfCharFormatted(format.start - 2, string, m_FontManager, m_CodeFontManager, m_FontSize, m_DefaultWeight, m_GlobalBounds.w, m_Formats);
					Vector2f end = m_GlobalBounds.position + gui::GetPositionOfCharFormatted(format.end + 2, string, m_FontManager, m_CodeFontManager, m_FontSize, m_DefaultWeight, m_GlobalBounds.w, m_Formats);

					start.y -= m_FontManager->Get(m_DefaultWeight, m_FontSize)->GetAscent();
					end.y -= m_FontManager->Get(m_DefaultWeight, m_FontSize)->GetAscent();

					start.x -= 5.0f;
					end.y += m_FontManager->Get(m_DefaultWeight, m_FontSize)->GetMaxHeight();
					end.x = m_GlobalBounds.x + m_GlobalBounds.w;

					Shape bg = gui::GenerateRoundedQuad(start, end, { 0.03f, 0.03f, 0.03f, 1.0f }, 4.0f);
					//Shape bgBorder = gui::GenerateRoundedQuad(start - Vector2f{1.0f, 1.0f}, end + Vector2f{ 1.0f, 1.0f }, { 0.06f, 0.06f, 0.06f, 1.0f }, 4.0f);

					//drawList.Add(bgBorder.vertices, bgBorder.indices);
					drawList.Add(bg.vertices, bg.indices);
				}

				if (format.option == TextFormatOption::HorizontalRule && !m_Editing)
				{
					Vector2f start = m_GlobalBounds.position + gui::GetPositionOfCharFormatted(format.start - 2, string, m_FontManager, m_CodeFontManager, m_FontSize, m_DefaultWeight, m_GlobalBounds.w, m_Formats);
					start.y -= (float)m_FontManager->Get(m_DefaultWeight, m_FontSize)->GetAscent() / 2.0f;

					Vector2f end = start;
					end.y += 1.0f;
					end.x = m_GlobalBounds.x + m_GlobalBounds.w;

					Shape bg = gui::GenerateQuad(start, end, {}, {}, { 0.04f, 0.04f, 0.04f, 1.0f });

					drawList.Add(bg.vertices, bg.indices);
				}

				if (format.option == TextFormatOption::Highlight)
				{

					uint32_t offset = 2;

					if (m_Editing)
						offset = 0;

					float padding = 4.0f;

					Vector2f start = m_GlobalBounds.position + gui::GetPositionOfCharFormatted(format.start - offset, string, m_FontManager, m_CodeFontManager, m_FontSize, m_DefaultWeight, m_GlobalBounds.w, m_Formats);
					Vector2f end = m_GlobalBounds.position + gui::GetPositionOfCharFormatted(format.end - offset + 1, string, m_FontManager, m_CodeFontManager, m_FontSize, m_DefaultWeight, m_GlobalBounds.w, m_Formats);
					start.y -= m_FontManager->Get(m_DefaultWeight, m_FontSize)->GetAscent();
					end.y -= m_FontManager->Get(m_DefaultWeight, m_FontSize)->GetAscent();

					start.x -= padding;
					end.x += padding;
					end.y += m_FontManager->Get(m_DefaultWeight, m_FontSize)->GetMaxHeight() + padding / 2.0f;

					Shape bg = gui::GenerateRoundedQuad(start, end, { 0.6f, 0.04f, 0.04f, 1.0f }, 2.0f);

					drawList.Add(bg.vertices, bg.indices);
				}
			}

			if (!str.empty() && m_Font)
			{
				Colour col = m_Colour;
				col.a *= GetTransparency();

				Vector2f position = m_GlobalBounds.position;
				Shape quad = gui::GenerateQuad(position, position + text.textBounds.size, { 0.0f, 0.0f }, { 1.0f, 1.0f }, col);

				drawList.Add(quad.vertices, quad.indices, &text.texture);
			}

			// Render cursor

			if (m_Editing)
			{
				if (m_CursorTime < m_CursorBlinkTime)
				{
					Vector2f cursorPos = gui::GetPositionOfCharFormatted(gui::EventHandler::cursorOffset, string, m_FontManager,m_CodeFontManager, m_FontSize, m_DefaultWeight, m_GlobalBounds.w, m_Formats);

					//cursorPos.x += 1.0f;

					TextFormatOption formatOption = GetFormattingAtPoint(gui::EventHandler::cursorOffset);

					Font* font = GetFontForFormat(formatOption, m_FontManager, m_CodeFontManager, m_DefaultWeight, m_FontSize);

					Shape cursor = gui::GenerateQuad(m_GlobalBounds.position + cursorPos + Vector2f{ 0.0f, cursorMod },
						{ m_GlobalBounds.position.x + cursorPos.x + 2.0f, m_GlobalBounds.position.y + cursorPos.y - (float)font->GetAscent() },
						{ 0.0f, 0.0f }, { 0.0f, 0.0f }, m_Colour);

					drawList.Add(cursor.vertices, cursor.indices);
				}

				m_CursorTime += EventHandler::deltaTime;

				if (m_CursorTime >= 1.0f)
					m_CursorTime = 0.0f;

				//m_Bounds.h = gui::GetTextBoxSizeFormatted(string, m_FontManager, m_CodeFontManager, m_FontSize, m_DefaultWeight, {}, m_Bounds.w, {}, {}, 0.0f, m_Formats).y;
			}

			GenerateChildVertexLists(drawList);
		}
	

		void OnEvent() override
		{

			static bool dragging = false;
			m_Hovered = false;
			FloatRect bounds = m_GlobalBounds;
			bounds.h = text.textBounds.h;
			if (bounds.Contains((float)EventHandler::x, (float)EventHandler::y))
			{
				m_Hovered = true;
			}

			if (m_Hovered)
			{
				
				
				if (EventHandler::mouseButton[MouseButton::MOUSE_LEFT].clicks == 1)
				{
					m_Editing = true;
					m_FullRerender = true;
					Vector2f mpoint = Vector2f((float)EventHandler::x, (float)EventHandler::y);
					mpoint = mpoint - m_GlobalBounds.position;
					EventHandler::cursorOffset = gui::GetNearestCharFromPointFormatted(mpoint, 0, string, m_FontManager, m_CodeFontManager, m_FontSize, m_DefaultWeight, m_GlobalBounds.w, m_Formats) + 1;
					EventHandler::textInput = &string;

					if (EventHandler::cursorOffset > EventHandler::textInput->size())
						EventHandler::cursorOffset = EventHandler::textInput->size();

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
					m_FullRerender = true;

					if (m_OnLoseFocus)
						m_OnLoseFocus();
				}
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

		void SetCodeFontManager(FontManager* codeFont)
		{
			m_CodeFontManager = codeFont;
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

		void SetOnLoseCallback(std::function<void()> func)
		{
			m_OnLoseFocus = func;
		}

		void SetShouldFormat(bool format)
		{
			m_Format = format;
		}

		float GetTextBoxHeight()
		{
			return gui::GetTextBoxSizeFormatted(string, m_FontManager, m_CodeFontManager, m_FontSize, m_DefaultWeight, {}, m_Bounds.w, {}, {}, 0.0f, m_Formats).y;
		}

		void ShowBlankText(const std::string& str)
		{
			m_BlankText = str;
		}

	private:

		std::function<void()> m_OnEdit;
		std::function<void()> m_OnLoseFocus;

		std::string m_CachedString;
		FloatRect m_CachedBounds;
		std::vector< TextFormat> m_CachedFormat;

		bool m_Format = false;

		FontManager* m_FontManager;
		FontManager* m_CodeFontManager;
		Font* m_Font;
		uint32_t m_FontSize;
		FontWeight m_DefaultWeight = FontWeight::ExtraLight;
		bool m_Editing = false;

		float m_CursorTime = 0.0f;
		float m_CursorBlinkTime = 0.5f;

		bool m_FullRerender = true;

		std::string m_BlankText = "";


		std::vector< TextFormat> m_Formats;

		TextFormatOption GetFormattingAtPoint(uint32_t pos)
		{
			if (m_Formats.size() == 0)
				return TextFormatOption::None;

			TextFormatOption formatOption = TextFormatOption::None;

			uint32_t currentFormatEnd = UINT32_MAX;
			uint32_t currentFormatStart = UINT32_MAX;
			uint32_t currentStartFormatterSize = 0;
			uint32_t currentEndFormatterSize = 0;

			for (auto& formats : m_Formats)
			{
				if (pos > formats.start - formats.formatterStartSize && pos < formats.end + formats.formatterEndSize)
				{
					//if (i > formats.start && i < formats.end)
					//{
					formatOption = formats.option;
					//}
					currentFormatEnd = formats.end;
					currentFormatStart = formats.start;
					currentStartFormatterSize = formats.formatterStartSize;
					currentEndFormatterSize = formats.formatterEndSize;
				}
			}

			return formatOption;

		}

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

			void RasterizeTextFormatted(const std::string& str, FontManager* fontManager, FontManager* codeManager, uint32_t fontSize, float textWrap, 
				std::vector<TextFormat> formatting, FontWeight defaultWeight, uint32_t minTextFormat, uint32_t maxTextFormat, Colour col)
			{
				Font* font = fontManager->Get(gui::FontWeight::Bold, fontSize);


				textBounds.w =   textWrap + gui::TextPadding;
				float height = gui::TextPadding + ((gui::GetLineCount(str, font, textWrap) + 1) * font->GetLineSpacing());

				float baseLine = (float)font->GetAscent();

				textBounds.h = gui::GetTextBoxSizeFormatted(str, fontManager, codeManager, fontSize,
					defaultWeight, {}, textWrap, { 1.0f, 1.0f, 1.0f, 1.0f }, textBounds, baseLine, formatting).y;

				textBounds.size += { gui::TextPadding, gui::TextPadding };

				image.New((int)textBounds.w, (int)textBounds.h, 4);
				image.Fill({ 0.0f, 0.0f, 0.0f, 0.0f });

				

				printf("Rerasterising Textbox...\n");



				gui::RenderTextSoftwareFormatted(image, str, fontManager, codeManager, fontSize,
					defaultWeight, {}, textWrap, { 1.0f, 1.0f, 1.0f, 1.0f }, textBounds, baseLine, formatting, minTextFormat, maxTextFormat, col);


				texture.CreateFromImage(image);


			}
			
		};

		TextEntry text;
	};
}