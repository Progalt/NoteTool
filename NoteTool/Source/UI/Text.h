#pragma once
#include "Widget.h"
#include "../Font.h"
#include "Utility.h"

namespace gui
{
	class Text : public Widget
	{
	public:

		void GenerateVertexList(DrawList& drawList) override
		{
			if (!m_Visible)
				return;

			if (!m_String.empty() && m_Font)
			{
				Colour col = m_Colour;
				col.a *= GetTransparency();

				gui::RenderText(drawList, m_String, m_Font, m_GlobalBounds.position, 0.0f, col);
			}

			GenerateChildVertexLists(drawList);
		}

		void SetString(const std::string& str)
		{
			m_String = str;

			if (m_Font)
				m_Bounds.w = gui::GetTextLength(m_String, m_Font);
		}

		void SetFont(Font* font) { 
			m_Font = font; 
			if (!m_String.empty())
				m_Bounds.w = gui::GetTextLength(m_String, m_Font);
		}

		const std::string GetString()
		{
			return m_String;
		}

	private:

		std::string m_String;
		Font* m_Font;
	};
}