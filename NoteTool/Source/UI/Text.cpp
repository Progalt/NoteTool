
#include "Text.h"

namespace gui
{
	void Text::GenerateVertexList(DrawList& drawList)
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

		// Sometimes after a resize we can get a fractional coordinate. 
		// Due to the way the GPU rasterizes triangles fractional coordinates cause distorted text because sample positions are also on pixel borders. 
		// We round to remove the fractional part of the coordinate
		m_GlobalBounds.x = roundf(m_GlobalBounds.x);
		m_GlobalBounds.y = roundf(m_GlobalBounds.y);
		m_GlobalBounds.w = roundf(m_GlobalBounds.w);
		m_GlobalBounds.h = roundf(m_GlobalBounds.h);

		Shape shape = gui::GenerateQuad(m_GlobalBounds.position, m_GlobalBounds.position + m_GlobalBounds.size, { 0.0f, 0.0f }, { 1.0f, 1.0f }, col);

		drawList.Add(shape.vertices, shape.indices, &m_Texture);

		GenerateChildVertexLists(drawList);
	}

	void Text::SetString(const std::string& str)
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

	void Text::SetFont(Font* font) {
		m_Font = font;
		if (!m_String.empty())
		{
			m_Bounds.w = gui::GetTextLength(m_String, m_Font) + gui::TextPadding;
			m_Bounds.h = m_Font->GetMaxHeight() + gui::TextPadding;

			SetBounds(m_Bounds);
		}

		m_RerenderText = true;
	}

	void Text::RasterizeText()
	{
		m_Image.New((int)m_Bounds.w, m_Bounds.h, 4);
		m_Image.Fill({ 0.0f, 0.0f, 0.0f, 0.0f });

		float baseLine = (float)m_Font->GetAscent();

		gui::RenderTextSoftware(m_Image, m_String, m_Font, {}, m_Bounds.w, { 1.0f, 1.0f, 1.0f, 1.0f }, m_Bounds, baseLine);

		m_Texture.CreateFromImage(m_Image);


		m_Image.~Image();
	}
}