#pragma once
#include <vector>
#include "../Renderer/Renderer.h"

#include "Widget.h"

#include "../Font.h"

namespace gui
{
	template<typename _Ty>
	inline _Ty Lerp(_Ty a, _Ty b, _Ty t)
	{
		return a + t * (b - a);
	}

	template<typename _Ty>
	inline Colour Lerp(Colour a, Colour b, _Ty t)
	{
		Colour out;
		out.r = Lerp(a.r, b.r, t);
		out.g = Lerp(a.g, b.g, t);
		out.b = Lerp(a.b, b.b, t);
		out.a = Lerp(a.a, b.a, t);
		return out;
	}

	struct Shape
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
	};

	inline Shape GenerateQuad(Vector2f min, Vector2f max, Vector2f uvMin, Vector2f uvMax, Colour col)
	{
		Shape output;
		output.indices = { 0, 1, 3, 1, 2, 3 };

		output.vertices = {
			{ min, uvMin, col },
			{ {max.x, min.y}, { uvMax.x, uvMin.y}, col},
			{ max, uvMax, col },
			{ {min.x, max.y}, { uvMin.x, uvMax.y}, col},
		};

		return output;
	}

	// 0 up and 2 down
	inline Shape GenerateAlignedEqualTri(Vector2f min, Vector2f max, Colour col, int dir)
	{
		Shape output;
		output.indices = { 0, 1, 2 };

		Vector2f zeroUV = { 0.0f, 0.0f };

		switch (dir)
		{
		case 0:

			// Right 
			output.vertices = {
				{ min, zeroUV, col },
				{ {min.x + (max.x - min.x) / 2.0f, min.y + (max.y - min.y) / 2.0f}, zeroUV, col},
				{ { min.x, max.y }, zeroUV, col}
			};

			break;
		case 2:

			// Down
			output.vertices = {
				{ min, zeroUV, col },
				{ {max.x, min.y}, zeroUV, col},
				{ { min.x + (max.x - min.x) / 2.0f, max.y}, zeroUV, col}
			};

			break;
		case 1:
			break;
		case 3:
			break;
		}

		return output;
	}

	inline Shape GenerateRoundedQuad(Vector2f rmin, Vector2f rmax, Colour col, float rounding)
	{
		uint32_t detail = 4;
		if (rounding > 10.0f)
			detail = 8;

		float halfWidth = ((rmax.x - rmin.x) / 2.0f);
		float halfHeight = ((rmax.y - rmin.y) / 2.0f);

		Vector2f center = rmin + Vector2f{ halfWidth, halfHeight };

		float rectWidth = halfWidth - rounding;

		// basic center rect

		std::vector<Vertex> vertices;

		Vector2f min = { center.x - (float)rectWidth, center.y - (float)halfHeight };
		Vector2f max = { center.x + (float)rectWidth, center.y + (float)halfHeight };

		vertices.push_back({ { min.x, min.y }, { 0.0f, 0.0f }, col });
		vertices.push_back({ { max.x, min.y }, { 0.0f, 0.0f }, col });
		vertices.push_back({ { min.x, max.y }, { 0.0f, 0.0f }, col });
		vertices.push_back({ { max.x, max.y }, { 0.0f, 0.0f }, col });

		std::vector<unsigned int> indices = { 0, 1, 2, 1, 2, 3 };


		// Draw the 2 side rectangles


		min = { center.x - (float)halfWidth, center.y - (float)halfHeight + (float)rounding };
		max = { center.x - (float)halfWidth + (float)rounding, center.y + (float)halfHeight - (float)rounding };

		uint32_t vertexOffset = 4;

		vertices.push_back({ { min.x, min.y }, { 0.0f, 0.0f }, col });
		vertices.push_back({ { max.x, min.y }, { 0.0f, 0.0f }, col });
		vertices.push_back({ { min.x, max.y }, { 0.0f, 0.0f }, col });
		vertices.push_back({ { max.x, max.y }, { 0.0f, 0.0f }, col });

		indices.push_back(vertexOffset + 0);
		indices.push_back(vertexOffset + 1);
		indices.push_back(vertexOffset + 2);
		indices.push_back(vertexOffset + 1);
		indices.push_back(vertexOffset + 2);
		indices.push_back(vertexOffset + 3);

		min = { center.x + (float)halfWidth, center.y - (float)halfHeight + (float)rounding };
		max = { center.x + (float)halfWidth - (float)rounding, center.y + (float)halfHeight - (float)rounding };

		vertices.push_back({ { min.x, min.y }, { 0.0f, 0.0f }, col });
		vertices.push_back({ { max.x, min.y }, { 0.0f, 0.0f }, col });
		vertices.push_back({ { min.x, max.y }, { 0.0f, 0.0f }, col });
		vertices.push_back({ { max.x, max.y }, { 0.0f, 0.0f }, col });

		vertexOffset = 8;

		indices.push_back(vertexOffset + 0);
		indices.push_back(vertexOffset + 1);
		indices.push_back(vertexOffset + 2);
		indices.push_back(vertexOffset + 1);
		indices.push_back(vertexOffset + 2);
		indices.push_back(vertexOffset + 3);

		min = { center.x - (float)rectWidth, center.y - (float)halfHeight };
		max = { center.x + (float)rectWidth, center.y + (float)halfHeight };


		// Lets draw the rounded sections

		// this allows it to be done in loops

		// radians
		float cornerAngleBase[4] =
		{
			0.0,
			(M_PI / 2.0),
			(M_PI),
			((M_PI / 2.0) * 3.0)
		};

		Vector2f cornerCenters[4] =
		{
			{ max.x, max.y - (float)rounding },
			{ max.x, min.y + (float)rounding },
			{ min.x, min.y + (float)rounding },
			{ min.x, max.y - (float)rounding }
		};

		// loop through each corner and create the rounded edges

		for (uint32_t b = 0; b < 4; b++)
		{

			vertexOffset = vertices.size();

			vertices.push_back({ { cornerCenters[b].x, cornerCenters[b].y }, {0.0f, 0.0f}, col });

			// Top left corner
			for (uint32_t i = 0; i < detail + 1; i++)
			{
				// pi/2 radians or 90 degrees

				float angle = i * ((M_PI / 2.0) / static_cast<float>(detail));

				float s = sin(angle + cornerAngleBase[b]) * rounding;
				float c = cos(angle + cornerAngleBase[b]) * rounding;

				vertices.push_back({ { cornerCenters[b].x + (float)s, cornerCenters[b].y + (float)c}, {0.0f, 0.0f}, col });
			}

			for (uint32_t i = 1; i < detail + 1; i++)
			{
				indices.push_back(vertexOffset + 0);
				indices.push_back(vertexOffset + i);
				indices.push_back(vertexOffset + i + 1);
			}
		}

		Shape out;
		out.vertices = vertices;
		out.indices = indices;

		return out;
	}

	inline void RenderText(DrawList& list, const std::string& text, Font* font, Vector2i position, float textWrap, Colour col)
	{
		float x = 0.0f;

		uint32_t lineCount = 0;

		//if (m_String == m_PreviousString)
		//	return;

		float maxX = 0.0f, maxY = 0.0f;

		for (uint32_t i = 0; i < text.size(); i++)
		{



			uint32_t codepoint = text[i];

			if (codepoint == '\n')
			{
				x = 0;
				lineCount++;
				continue;
			}

			if (codepoint == '\t')
			{
				x += font->GetCodePointData(' ').advance * 4;
				continue;
			}


			Alphabet alphabet = font->GetAlphabetForCodepoint(codepoint);
			GlyphData data = font->GetCodePointData(codepoint);

			//Sprite& spr = m_Sprites[i];

			//spr.SetTexture(m_Font->GetTexture(alphabet), IntRect(data.x, data.y, data.w, data.h));

			//spr.SetScale(1.0f, 1.0f);


			// This is if word wrapped is enabled
			if (textWrap != 0.0f)
			{
				// We want to wrap per word so lets get the word lengths
				int wordOffset = x;
				for (uint32_t w = i; w < text.size(); w++)
				{
					if (text[w] == ' ')
						break;

					uint32_t cp = text[w];

					GlyphData d = font->GetCodePointData(cp);
					wordOffset += d.advance;
				}

				// if the word length is greater than the wrap limit go onto a new line 
				if (wordOffset > textWrap)
				{
					x = 0;
					lineCount++;
				}
			}

			float xpos = x + data.bearingX;
			float ypos = -(float)data.bearingY + (lineCount * font->GetLineSpacing());

			xpos += (float)position.x;
			ypos += (float)position.y;

			if (maxX < xpos + (float)data.advance)
				maxX = xpos + (float)data.advance;

			if (maxY < ypos)
				maxY = ypos;


			//m_GlyphPos[i] = { xpos, ypos };

			x += (float)data.advance;

			Vector2f uvMin = { (float)data.x , (float)data.y };
			Vector2f uvMax = { (float)data.x + (float)data.w, (float)data.y + (float)data.h };

			Vector2f texDim = { (float)font->GetTexture(alphabet)->GetWidth(), (float)font->GetTexture(alphabet)->GetHeight() };

			uvMin = uvMin / texDim;
			uvMax = uvMax / texDim;

			std::vector<Vertex> vertices(4);
			vertices[0].colour = col;
			vertices[0].position = { xpos, ypos };
			vertices[0].texCoord = uvMin;

			vertices[1].colour = col;
			vertices[1].position = { xpos + (float)data.w, ypos };
			vertices[1].texCoord = { uvMax.x, uvMin.y };

			vertices[2].colour = col;
			vertices[2].position = { xpos + (float)data.w, ypos + (float)data.h };
			vertices[2].texCoord = uvMax;

			vertices[3].colour = col;
			vertices[3].position = { xpos , ypos + (float)data.h };
			vertices[3].texCoord = { uvMin.x, uvMax.y };

			std::vector<uint32_t> indices = { 0, 1, 3, 1, 2, 3 };

			

			list.Add(vertices, indices, font->GetTexture(alphabet));

		}

	}

	inline float GetTextLength(const std::string& text, Font* font)
	{
		float x = 0.0f;

		uint32_t lineCount = 0;

		//if (m_String == m_PreviousString)
		//	return;

		float maxX = 0.0f, maxY = 0.0f;

		for (uint32_t i = 0; i < text.size(); i++)
		{



			uint32_t codepoint = text[i];

			if (codepoint == '\n')
			{
				x = 0;
				lineCount++;
				continue;
			}

			if (codepoint == '\t')
			{
				x += font->GetCodePointData(' ').advance * 4;
				continue;
			}


			Alphabet alphabet = font->GetAlphabetForCodepoint(codepoint);
			GlyphData data = font->GetCodePointData(codepoint);

			float xpos = x + data.bearingX;
			float ypos = -(float)data.bearingY + (lineCount * font->GetLineSpacing());


			if (maxX < xpos + (float)data.advance)
				maxX = xpos + (float)data.advance;

			x += (float)data.advance;
		}

		return maxX;
	}

	inline Vector2f GetPositionOfChar(uint32_t idx, const std::string& text, Font* font, float textWrap)
	{
		assert(idx <= text.size());

		float x = 0.0f;

		uint32_t lineCount = 0;

		//if (m_String == m_PreviousString)
		//	return;

		float maxX = 0.0f, maxY = 0.0f;

		for (uint32_t i = 0; i < idx; i++)
		{



			uint32_t codepoint = text[i];

			bool controlChar = false;

			if (codepoint == '\n')
			{
				x = 0;
				lineCount++;
				controlChar = true;
			}

			if (codepoint == '\t')
			{
				x += font->GetCodePointData(' ').advance * 4;
				controlChar = true;
			}


			Alphabet alphabet = font->GetAlphabetForCodepoint(codepoint);
			GlyphData data = font->GetCodePointData(codepoint);

			//Sprite& spr = m_Sprites[i];

			//spr.SetTexture(m_Font->GetTexture(alphabet), IntRect(data.x, data.y, data.w, data.h));

			//spr.SetScale(1.0f, 1.0f);


			// This is if word wrapped is enabled
			if (textWrap != 0.0f)
			{
				// We want to wrap per word so lets get the word lengths
				int wordOffset = x;
				for (uint32_t w = i; w < text.size(); w++)
				{
					if (text[w] == ' ' || text[w] == '\n')
						break;

					uint32_t cp = text[w];

					GlyphData d = font->GetCodePointData(cp);
					wordOffset += d.advance;
				}

				// if the word length is greater than the wrap limit go onto a new line 
				if (wordOffset > textWrap)
				{
					x = 0;
					lineCount++;
				}
			}

			float xpos = x + data.bearingX;
			float ypos = lineCount * font->GetLineSpacing();


			if (maxX < xpos + (float)data.advance)
				maxX = xpos + (float)data.advance;

			if (maxY < ypos)
				maxY = ypos;


			//m_GlyphPos[i] = { xpos, ypos };

			x += (float)data.advance;


			if (i == idx - 1)
			{
				if (controlChar)
					return { xpos + data.bearingX, ypos };
				else 
					return { xpos, ypos };
			}
		}

	}
}