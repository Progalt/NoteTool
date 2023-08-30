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

		void GenerateVertexList(DrawList& drawList) override;

		void OnEvent() override;

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