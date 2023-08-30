#pragma once
#include "Widget.h"
#include <functional>
#include "../Font.h"
#include "Utility.h"

namespace gui
{

	// A context menu is the little menu that could show up when right click
	class ContextMenu : public Widget
	{
	public:
		
		void Open()
		{
			SetVisible(true);
		}


		// Initialisation functions

		ContextMenu& AddOption(const std::string& text, std::function<void()> callback)
		{
			m_Options.push_back({ text, callback });

			return *this;
		}

		ContextMenu& AddDividor()
		{
			if (m_Options.size() > 0)
			{
				m_Options[m_Options.size() - 1].followedByDividor = true;
			}

			return *this;
		}

		void GenerateVertexList(DrawList& drawList) override
		{
			if (!m_Visible)
				return;

			// We ignore scissor rects

			IntRect parentScissor = drawList.GetCurrentScissor();

			// Set a null scissor rect to use whole screen
			drawList.SetScissor(IntRect());

			// Calculate overall size

			float height = 0.0f;// = m_Options.size() * (float)m_Font->GetMaxHeight();
			//height += m_OptionPadding * m_Options.size() + m_OptionPadding;


			// Loop through all options and find the largest
			float width = m_MinSize;
			for (auto& option : m_Options)
			{
				height += (float)m_Font->GetMaxHeight();
				height += m_OptionPadding;
				
				if (option.followedByDividor)
					height += m_OptionPadding + 1.0f;

				float len = gui::GetTextLength(option.text, m_Font);

				if (len > width)
					width = len;
			}

			height += m_OptionPadding;

			width += m_HorizontalPadding * 2.0f;

			m_Bounds.size = { width, height };

			Vector2f min = m_Bounds.position;
			Vector2f max = min + m_Bounds.size;

			// Draw the bg

			Shape bg = gui::GenerateRoundedQuad(min, max, m_Colour, m_Rounding);
			Shape outline = gui::GenerateRoundedQuad(min - Vector2f{ 1.0f, 1.0f }, max + Vector2f{ 1.0f, 1.0f }, m_BorderColour, m_Rounding);

			drawList.Add(outline.vertices, outline.indices);
			drawList.Add(bg.vertices, bg.indices);

			// Render text

			float y = m_OptionPadding + m_Bounds.y;
			for (auto& option : m_Options)
			{
				float x = m_HorizontalPadding + m_Bounds.x;

				// Rasterize the text if needed
				if (!option.createdGPUResources)
				{
					option.textImg.New((int)width, m_Font->GetMaxHeight() + 4, 4);
					option.textImg.Fill({ 0.0f, 0.0f, 0.0f, 0.0f });

					float baseLine = (float)m_Font->GetAscent();

					gui::RenderTextSoftware(option.textImg, option.text, m_Font, {}, width, { 1.0f, 1.0f, 1.0f, 1.0f }, FloatRect(), baseLine);

					option.textTexture.CreateFromImage(option.textImg);
				}

				Vector2f textMin = { x, y };
				Vector2f textMax = { x + width, y + m_Font->GetMaxHeight() + 4.0f };

				option.bounds.position = textMin;
				option.bounds.size = textMax - textMin;

				Shape textQuad = gui::GenerateQuad(textMin - Vector2f{ 0.0f, 1.0f }, textMax - Vector2f{ 0.0f, 1.0f }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });

				// before we draw the text we need to draw the hovered rect

				if (option.hovered)
				{
					Vector2f boxMin = { m_Bounds.x + 2.0f, textMin.y - 2.0f };
					Vector2f boxMax = { boxMin.x + width - 4.0f, textMax.y - 2.0f   };

					Shape hoverRect = gui::GenerateRoundedQuad(boxMin, boxMax, m_BorderColour, m_Rounding);

					drawList.Add(hoverRect.vertices, hoverRect.indices);
				}

				drawList.Add(textQuad.vertices, textQuad.indices, &option.textTexture);

				y += m_Font->GetMaxHeight() + m_OptionPadding;

				if (option.followedByDividor)
				{
					// If its followed by a dividor we want to render a line

					Shape div = gui::GenerateQuad({ m_Bounds.x,  y }, { m_Bounds.x + width,  y + 1.0f }, {}, {}, m_BorderColour);

					drawList.Add(div.vertices, div.indices);

					y += m_OptionPadding + 1.0f;
				}

				

			}

			// Context menu ignores children 

			// Reset the old scissor

			drawList.SetScissor(parentScissor);
		}

		void OnEvent() override
		{
			if (!m_Visible)
				return;

			if (m_Bounds.Contains((float)EventHandler::x, (float)EventHandler::y))
			{
				m_Hovered = true;
			}
			else
			{
				if (EventHandler::mouseButton[MOUSE_LEFT].clicks >= 1)
				{
					SetVisible(false);
				}
			}

			for (auto& option : m_Options)
			{
				option.hovered = false;
				if (option.bounds.Contains((float)EventHandler::x, (float)EventHandler::y))
				{
					option.hovered = true;

					if (EventHandler::mouseButton[MOUSE_LEFT].clicks >= 1)
					{
						if (option.callback)
							option.callback();
					}
				}
				
			}

		}

		// Setters
		
		void SetBorderColour(const Colour& col) { m_BorderColour = col; }

		void SetFont(Font* font) { m_Font = font; }

	private:

		struct Option
		{
			std::string text = "";
			std::function<void()> callback;

			bool followedByDividor = false;

			FloatRect bounds;

			Image textImg;
			GPUTexture textTexture;
			bool createdGPUResources;

			bool hovered = false;
		};

		Colour m_BorderColour;

		Font* m_Font;

		float m_OptionPadding = 6.0f;
		float m_HorizontalPadding = 8.0f;
		float m_MinSize = 96.0f;

		std::vector<Option> m_Options;

		bool m_Hovered = false;
	};
}