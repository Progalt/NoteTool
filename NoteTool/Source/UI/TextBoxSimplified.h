#pragma once
#include "Widget.h"
#include "Utility.h"
#include "EventHandler.h"
#include "Formatter.h"
#include <functional>

namespace gui
{
	class TextBoxSimplified : public Widget
	{
	public:

		void GenerateVertexList(DrawList& drawList) override
		{
			if (!m_Visible || m_GlobalBounds.w <= 0.0f || m_GlobalBounds.h <= 0.0f)
				return;

			float cursorMod = 2.0f;

			if (m_Format)
			{
				formattedString = string;
			}

			Colour col = { 1.0f, 1.0f, 1.0f, 1.0f };
			std::string str = m_Format ? formattedString : string;
			bool dim = false;
			if (str.empty() && !m_BlankText.empty())
			{
				str = m_BlankText;
				dim = true;
			}


			if (m_FullRerender)
			{

				

				uint32_t min = 0;
				uint32_t max = string.size();

				if (!m_Editing)
					max = 0;



				Colour col = { 1.0f, 1.0f, 1.0f, 1.0f };
				if (dim)
				{
					col = { 0.25f, 0.25f, 0.25f, 1.0f };
				}

				RasterizeTextFormatted(str, m_FontManager, m_CodeFontManager, m_FontSize, m_Bounds.w, m_Formats, m_DefaultWeight, min, max, col, gui::EventHandler::cursorOffset);

				m_FullRerender = false;


			}


			/*for (auto& pos :  formattedPositions)
			{
				Vector2f min = m_GlobalBounds.position + pos.position;
				Vector2f max = min - pos.size;

				Shape shape = gui::GenerateQuad( min, max, {}, {}, {1.0f, 0.0f, 0.0f, 0.25f});

				drawList.Add(shape.vertices, shape.indices);
			} */




			if (!str.empty() && m_Font)
			{
				Colour col = m_Colour;
				col.a *= GetTransparency();

				Vector2f position = m_GlobalBounds.position;
				Shape quad = gui::GenerateQuad(position, position + textBounds.size, { 0.0f, 0.0f }, { 1.0f, 1.0f }, col);

				drawList.Add(quad.vertices, quad.indices, &texture);
			}


			// Render cursor

			if (m_Editing)
			{
				if (m_CursorTime < m_CursorBlinkTime)
				{
					Vector2f cursorPos = GetPosition(gui::EventHandler::cursorOffset); // gui::GetPositionOfCharFormatted(gui::EventHandler::cursorOffset, string, m_FontManager, m_CodeFontManager, m_FontSize, m_DefaultWeight, m_GlobalBounds.w, m_Formats);

					
					//cursorPos.x += 1.0f;

					TextFormatOption formatOption = GetFormattingAtPoint(gui::EventHandler::cursorOffset);

					Font* font = GetFontForFormat(formatOption, m_FontManager, m_CodeFontManager, m_DefaultWeight, m_FontSize);

					if (cursorPos == Vector2f(0.0f, 0.0f))
					{
						// no text so cursor defaults to 0, 0 
						// just manually offset it by font ascent
						cursorPos.y += font->GetAscent();
					}

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
			bounds.h = textBounds.h;
			if (bounds.Contains((float)EventHandler::x, (float)EventHandler::y))
			{
				m_Hovered = true;
			}

			if (m_Hovered)
			{


				if (EventHandler::mouseButton[MouseButton::MOUSE_LEFT].clicks == 1)
				{
					if (!m_Editing)
					{
						m_FullRerender = true;
					}
					m_Editing = true;

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
				if (EventHandler::mouseButton[MouseButton::MOUSE_LEFT].clicks == 1 && m_Editing)
				{
					if (m_Editing)
					{
						m_FullRerender = true;
					}
					// We have lost focus
					m_Editing = false;


					if (m_OnLoseFocus)
						m_OnLoseFocus();
				}
			}



			if (string != m_CachedString || m_Bounds != m_CachedBounds)
				TriggerRerender();
		}

		std::string string;

		void TriggerRerender()
		{
			m_FullRerender = true;

			m_CachedString = string;
			m_CachedBounds = m_Bounds;

			if (m_OnEdit)
				m_OnEdit(m_OnEditUserData);
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

		void SetOnEditCallback(std::function<void(void*)> func ,void* userData = nullptr)
		{
			m_OnEdit = func;
			m_OnEditUserData = userData;
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
			//if (formattedPositions.size() == 0)
			//	return m_GlobalBounds.h;
			//else
			//	return formattedPositions[formattedPositions.size() - 1].y + formattedPositions[formattedPositions.size() - 1].h + 24.0f;

			float fontHeight = (float)m_FontManager->Get(m_DefaultWeight, m_FontSize)->GetMaxHeight();
			float lineCount = (float)gui::GetLineCount(string, m_FontManager->Get(m_DefaultWeight, m_FontSize), m_Bounds.w) + 1;

			return fontHeight * lineCount;
		}

		void ShowBlankText(const std::string& str)
		{
			m_BlankText = str;
		}

		void RemoveFocus()
		{
			m_Editing = false;
		}

		void TakeFocus()
		{
			if (!m_Editing)
			{
				m_FullRerender = true;
			}
			m_Editing = true;

			EventHandler::textInput = &string;
			gui::EventHandler::cursorOffset = 0;
		}


	private:

		std::function<void(void*)> m_OnEdit;
		void* m_OnEditUserData;
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

		std::vector<TextFormat> m_Formats;


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


		GPUTexture texture;
		Image image;
		bool rerender = true;
		FloatRect textBounds;
		std::vector<TextFormat> formatting;

		std::vector <FloatRect > formattedPositions;

		std::string formattedString;

		Vector2f GetPosition(uint32_t idx)
		{
			if (formattedPositions.size() == 0)
				return Vector2f(0.0f, 0.0f);

			if (idx == 0)
			{
				Vector2f pos = formattedPositions[0].position;
				pos.x -= formattedPositions[0].size.x;
				return pos;
			}

			if (idx - 1 < formattedPositions.size())
				return formattedPositions[idx - 1].position;

			return formattedPositions[formattedPositions.size() - 1].position;
		}

		void RasterizeTextFormatted(const std::string& str, FontManager* fontManager, FontManager* codeManager, uint32_t fontSize, float textWrap,
			std::vector<TextFormat> formatting, FontWeight defaultWeight, uint32_t minTextFormat, uint32_t maxTextFormat, Colour col, uint32_t cursorPos)
		{
			PROFILE_BEGIN(rasterTime);
			Font* font = fontManager->Get(gui::FontWeight::Bold, fontSize);


			textBounds.w = textWrap + gui::TextPadding;

			float baseLine = (float)font->GetAscent();

			//textBounds.h = gui::GetTextBoxSizeFormatted(str, fontManager, codeManager, fontSize,
			//	defaultWeight, {}, textWrap, { 1.0f, 1.0f, 1.0f, 1.0f }, textBounds, baseLine, formatting).y;


			textBounds.h = GetTextBoxHeight() + gui::TextPadding;


			textBounds.size += { gui::TextPadding, gui::TextPadding };

			image.New((int)textBounds.w, (int)textBounds.h, 4);


			image.Fill({ 0.0f, 0.0f, 0.0f, 0.0f }, 0);


			printf("Rerasterising Textbox...\n");

			FloatRect bounds = { 0.0f, 0.0f, textBounds.w, textBounds.h };

			formattedPositions = gui::RenderTextSoftwareFormatted(image, str, fontManager, codeManager, fontSize,
				defaultWeight, {}, textWrap, { 1.0f, 1.0f, 1.0f, 1.0f }, textBounds, baseLine, formatting, bounds, minTextFormat, maxTextFormat, col);



			texture.CreateFromImage(image);


			PROFILE_END(rasterTime, "Text Box Raster");

		}


	};
}