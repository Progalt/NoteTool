#pragma once

#include "Widget.h"
#include <functional>
#include "Utility.h"
#include "../Font.h"
#include "IconManager.h"

namespace gui
{
	class Button : public Widget
	{
	public:

		void GenerateVertexList(DrawList& drawList)
		{
			if (!m_Visible)
				return;
		

			if (m_Hovered && !m_Clicked)
			{
				m_Temp = Lerp(m_Temp, m_HoveredColour, EventHandler::deltaTime * m_HoverSpeed);
			}
			else
			{
				m_Temp = Lerp(m_Temp, m_Colour, EventHandler::deltaTime * m_HoverSpeed);
			}

			if (m_Clicked)
			{
			

				m_Clicked = false;
			}

			Colour borderCol = { 1.0f, 1.0f, 1.0f, 1.0f };

			Colour col = m_Temp;

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

			Colour textCol = { 1.0f, 1.0f, 1.0f, 1.0f };
			textCol.a *= GetTransparency();

			if (!m_Text.str.empty())
			{
				float lineSpacing = (float)m_Text.font->GetLineSpacing();
				Vector2i position;
				position.y = m_GlobalBounds.y + (m_GlobalBounds.h / 2.0f) - (float)m_Text.font->GetMaxHeight() / 2.0f;
				float textLength = gui::GetTextLength(m_Text.str, m_Text.font);

				switch (m_Text.alignment)
				{
				case Alignment::Centre:
					position.x = m_GlobalBounds.x + (m_GlobalBounds.w / 2.0f) - textLength / 2.0f ;
					break;
				case Alignment::Right:
					position.x = m_GlobalBounds.x + m_GlobalBounds.w - textLength - 5.0f;
					break;
				case Alignment::Left:
					position.x = m_GlobalBounds.x + 5.0f;
					break;
				}

				position.x += m_Text.offset;

				//gui::RenderText(drawList, m_Text.str, m_Text.font, position, 0.0f, textCol, m_GlobalBounds);

				if (m_Text.rerender)
				{
					RasterizeText();
					m_Text.rerender = false;
				}

				Shape quad = gui::GenerateQuad(position, position + m_Text.textBounds.size, { 0.0f, 0.0f }, { 1.0f, 1.0f }, textCol);

				drawList.Add(quad.vertices, quad.indices, &m_Text.texture);
			}

			if (m_Icon.type != IconType::None)
			{
				GPUTexture* icon = IconManager::GetInstance()[m_Icon.type];

				float iconSize = (float)icon->GetWidth();

				float yOffset = (m_GlobalBounds.h - iconSize) / 2.0f;

				Vector2f position;
				position.y = m_GlobalBounds.y + yOffset;

				switch (m_Icon.alignment)
				{
				case Alignment::Centre:
					position.x = m_GlobalBounds.x + (m_GlobalBounds.w / 2.0f) - iconSize / 2.0f;
					break;
				case Alignment::Right:
					position.x = m_GlobalBounds.x + m_GlobalBounds.w - iconSize - 5.0f;
					break;
				case Alignment::Left:
					position.x = m_GlobalBounds.x + 5.0f;
					break;
				}

				position.x += m_Icon.offset;

				Shape iconRect = gui::GenerateQuad(position, position + Vector2f{ iconSize, iconSize }, { 0.0f,  0.0f }, { 1.0f, 1.0f }, textCol);
				drawList.Add(iconRect.vertices, iconRect.indices, icon);
			}
			
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
				if (m_RequireDoubleClick && EventHandler::mouseButton[MouseButton::MOUSE_LEFT].clicks >= 2)
				{
					m_Callback(m_UserData);
					m_Clicked = true;
				}
				else if (!m_RequireDoubleClick && EventHandler::mouseButton[MouseButton::MOUSE_LEFT].clicks == 1)
				{
					m_Callback(m_UserData);
					m_Clicked = true;
				}

			}

		}
		
		void SetOnClick(std::function<void(void*)> callback) { m_Callback = callback; }


		void SetHighlightColour(Colour col) { m_Highlight = col; }
		void SetShadowColour(Colour col) { m_ShadowColour = col; }


		void SetHoveredColour(Colour col) { m_HoveredColour = col; }

		void SetText(const std::string& str, Font* font, Alignment alignment = Alignment::Centre, float offset = 0.0f)
		{
			m_Text.str = str;
			m_Text.font = font;
			m_Text.alignment = alignment;
			m_Text.rerender = true;
			m_Text.offset = offset;
		}

		void SetRequireDoubleClick(bool doubleClick)
		{
			m_RequireDoubleClick = doubleClick;
		}

		void SetUserData(void* userData) { m_UserData = userData; }

		void SetIcon(IconType type, Alignment align = Alignment::Centre, float offset = 0.0f)
		{
			m_Icon.type = type;
			m_Icon.alignment = align;
			m_Icon.offset = offset;
		}

	private:

		std::function<void(void*)> m_Callback;

		bool m_Clicked = false;

		bool m_RequireDoubleClick = false;

		Colour m_Highlight = Colour(1.0f, 1.0f, 1.0f, 0.0f);
		Colour m_HoveredColour;
		Colour m_ShadowColour = Colour(1.0f, 1.0f, 1.0f, 0.0f);

		Colour m_Temp = m_Colour;

		float m_HoverSpeed = 30.0f;

		struct
		{
			IconType type = IconType::None;
			Alignment alignment;
			float offset = 0.0f;
		} m_Icon;

		struct
		{
			std::string str = "";
			Font* font;
			Alignment alignment;

			FloatRect textBounds{};

			GPUTexture texture;
			Image image;
			bool rerender = true;
			float offset = 0.0f;
		

		} m_Text;

		void RasterizeText()
		{
			m_Text.textBounds.w = gui::GetTextLength(m_Text.str, m_Text.font) + gui::TextPadding;
			m_Text.textBounds.h = m_Text.font->GetMaxHeight() + gui::TextPadding;

			m_Text.image.New((int)m_Text.textBounds.w, m_Text.textBounds.h, 4);
			m_Text.image.Fill({ 0.0f, 0.0f, 0.0f, 0.0f });

			float baseLine = (float)m_Text.font->GetAscent();

			gui::RenderTextSoftware(m_Text.image, m_Text.str, m_Text.font, {}, m_Text.textBounds.w, { 1.0f, 1.0f, 1.0f, 1.0f }, m_Text.textBounds, baseLine);

			m_Text.texture.CreateFromImage(m_Text.image);


		}

		void* m_UserData;



	};
}