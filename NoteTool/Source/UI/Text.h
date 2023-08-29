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

			Colour col = m_Colour;
			col.a *= GetTransparency();

			if (!m_String.empty() && m_Font && m_RerenderText)
			{
				

				RasterizeText();

				//gui::RenderText(drawList, m_String, m_Font, m_GlobalBounds.position, 0.0f, col, FloatRect());

				m_RerenderText = false;
			}

			Shape shape = gui::GenerateQuad(m_GlobalBounds.position, m_GlobalBounds.position + m_GlobalBounds.size, { 0.0f, 0.0f }, { 1.0f, 1.0f }, col);

			drawList.Add(shape.vertices, shape.indices, &m_Texture);

			GenerateChildVertexLists(drawList);
		}

		void SetString(const std::string& str)
		{
			m_String = str;

			if (m_Font)
			{
				m_Bounds.w = gui::GetTextLength(m_String, m_Font) + gui::TextPadding;
				m_Bounds.h = m_Font->GetMaxHeight() + gui::TextPadding;

				SetBounds(m_Bounds);
			}

			m_RerenderText = true;
		}

		void SetFont(Font* font) { 
			m_Font = font; 
			if (!m_String.empty())
			{
				m_Bounds.w = gui::GetTextLength(m_String, m_Font) + gui::TextPadding;
				m_Bounds.h = m_Font->GetMaxHeight() + gui::TextPadding;

				SetBounds(m_Bounds);
			}

			m_RerenderText = true;
		}

		const std::string GetString()
		{
			return m_String;
		}

	private:

		std::string m_String;
		Font* m_Font;

		GPUTexture m_Texture;
		Image m_Image;
		bool m_RerenderText = true;

		void RasterizeText()
		{
			m_Image.New((int)m_Bounds.w, m_Bounds.h, 4);
			m_Image.Fill({ 0.0f, 0.0f, 0.0f, 0.0f });

			float baseLine = (float)m_Font->GetAscent();

			gui::RenderTextSoftware(m_Image, m_String, m_Font, {}, m_Bounds.w, { 1.0f, 1.0f, 1.0f, 1.0f }, m_Bounds, baseLine);

			m_Texture.CreateFromImage(m_Image);
		}
	};
}