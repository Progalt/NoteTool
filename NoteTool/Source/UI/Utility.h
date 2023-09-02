#pragma once
#include "GeometryUtil.h"

#include "Widget.h"

#include "../Font.h"
#include "FontManager.h"
#include "Formatter.h"


namespace gui
{
	constexpr uint32_t TextPadding = 2;


	inline void RenderText(DrawList& list, const std::string& text, Font* font, Vector2i position, float textWrap, Colour col, FloatRect bounds)
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

			FloatRect glyphBounds;
			glyphBounds.position = vertices[0].position;
			glyphBounds.size = vertices[2].position - vertices[0].position;

			if (!bounds.IsNull())
				if (!bounds.Intersects(glyphBounds))
					continue;

			list.Add(vertices, indices, font->GetTexture(alphabet));

		}

	}

	// This software rasterizes text into an image
	inline void RenderTextSoftware(Image& img, const std::string& text, Font* font, Vector2i position, float textWrap, Colour col, FloatRect bounds, float baseLine)
	{
		float x = 0.0f;

		uint32_t lineCount = 0;

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
			float ypos = -(float)data.bearingY + (lineCount * font->GetLineSpacing()) + baseLine;

			//xpos += (float)position.x;
			//ypos += (float)position.y;

			if (maxX < xpos + (float)data.advance)
				maxX = xpos + (float)data.advance;

			if (maxY < ypos)
				maxY = ypos;


			//m_GlyphPos[i] = { xpos, ypos };

			x += (float)data.advance;
			
			
			font->RasterizeGlyph(codepoint, &img, (int)xpos, (int)ypos);

			
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

	inline uint32_t GetLineCount(const std::string& text, Font* font, float textWrap)
	{
		uint32_t count;

		float x = 0.0f;
		float y = 0.0f;

		uint32_t lineCount = 0;

		//if (m_String == m_PreviousString)
		//	return;

		float maxX = 0.0f, maxY = 0.0f;

		for (uint32_t i = 0; i < text.size(); i++)
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



			//m_GlyphPos[i] = { xpos, ypos };

			if (!controlChar)
				x += (float)data.advance;
			y = (float)lineCount * (float)font->GetLineSpacing();

		}
		
		return lineCount;
	}

	inline uint32_t GetLineOfChar(uint32_t idx, const std::string& text, Font* font, float textWrap)
	{
		uint32_t count;

		float x = 0.0f;
		float y = 0.0f;

		uint32_t lineCount = 0;

		//if (m_String == m_PreviousString)
		//	return;

		float maxX = 0.0f, maxY = 0.0f;

		for (uint32_t i = 0; i <  idx; i++)
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



			//m_GlyphPos[i] = { xpos, ypos };

			if (!controlChar)
				x += (float)data.advance;
			y = (float)lineCount * (float)font->GetLineSpacing();

		}

		return lineCount;
	}

	inline uint32_t GetLastIdxOfLine(uint32_t line, const std::string& text, Font* font, float textWrap)
	{
		uint32_t count;

		float x = 0.0f;
		float y = 0.0f;

		uint32_t lineCount = 0;

		//if (m_String == m_PreviousString)
		//	return;

		float maxX = 0.0f, maxY = 0.0f;

		for (uint32_t i = 0; i < text.size(); i++)
		{



			uint32_t codepoint = text[i];

			bool controlChar = false;

			if (codepoint == '\n')
			{
				if (lineCount == line)
					return i - 1;

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
					if (lineCount == line)
						return i - 1;

					x = 0;
					lineCount++;

					
				}
			}



			//m_GlyphPos[i] = { xpos, ypos };

			if (!controlChar)
				x += (float)data.advance;
			y = (float)lineCount * (float)font->GetLineSpacing();

		}
		
		return 0;
	}

	inline Vector2f GetPositionOfChar(uint32_t idx, const std::string& text, Font* font, float textWrap)
	{
		if (idx > text.size())
			idx = text.size();

		float x = 0.0f;

		float xpos = 0.0f;
		float ypos = 0.0f;

		uint32_t lineCount = 0;

		//if (m_String == m_PreviousString)
		//	return;

		float maxX = 0.0f, maxY = 0.0f;
		bool controlCharacter = false;

		for (uint32_t i = 0; i < idx; i++)
		{
			controlCharacter = false;


			uint32_t codepoint = text[i];

			if (codepoint == '\n')
			{
				x = 0;
				lineCount++;
				controlCharacter = true;
			}

			if (codepoint == '\t')
			{
				x += font->GetCodePointData(' ').advance * 4;
				controlCharacter = true;
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


			if (!controlCharacter)
			{
				x += (float)data.advance;
			}

			xpos = x + data.bearingX;
			
			ypos = lineCount * font->GetLineSpacing();


			
			
			if (maxX < xpos + (float)data.advance)
				maxX = xpos + (float)data.advance;

			if (maxY < ypos)
				maxY = ypos;



			
		}

		return { xpos, ypos };

	}


	inline uint32_t GetNearestCharFromPoint(Vector2f point, const std::string& text, Font* font, float textWrap)
	{

		float x = 0.0f;
		float y = 0.0f;

		uint32_t lineCount = 0;

		//if (m_String == m_PreviousString)
		//	return;

		float maxX = 0.0f, maxY = 0.0f;

		uint32_t nearest = UINT32_MAX;
		float dist = 1000000.0f;

		uint32_t mouseLine = 0;

		float my = point.y;

		mouseLine = (int)(my / (float)font->GetLineSpacing());

		if (point.x < 0.0f && point.y < 0.0f)
			return 0;

		for (uint32_t i = 0; i < text.size(); i++)
		{



			uint32_t codepoint = text[i];

			bool controlChar = false;

			// Handle control characters
			// These aren't actually rendered but do effect how the text should be rendered
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




			if (!controlChar)
				x += (float)data.advance;
			y = (float)lineCount * (float)font->GetLineSpacing();

			if (lineCount == mouseLine)
			{

				// test the distance the point
				Vector2f charPoint(x, y);
				float newDist = point.Distance(charPoint);

				// if the new distance is closer than the current closest set this as the current closest
				if (newDist < dist)
				{
					dist = newDist;
					nearest = i;

				}

				
			}

		}

		if (nearest == UINT32_MAX)
			nearest = text.size();

		return nearest;
	}

	inline Font* GetFontForFormat(TextFormatOption format, FontManager* fontManager, FontManager* codeFontManager, FontWeight defaultWeight, uint32_t textSize)
	{

		switch (format)
		{
		case TextFormatOption::None:
			return fontManager->Get(defaultWeight, textSize);
			break;
		case TextFormatOption::Bold:
			return fontManager->Get(FontWeight::Bold, textSize);
			break;
		case TextFormatOption::Italic:
			return fontManager->Get((defaultWeight == FontWeight::ExtraLight) ? FontWeight::ExtraLightItalic : FontWeight::Italic, textSize);
			break;
		case TextFormatOption::Emphasis:
			return fontManager->Get(FontWeight::BoldItalic, textSize);
			break;
		case TextFormatOption::Header1:
			return fontManager->Get(FontWeight::Bold, textSize + 14);
			break;
		case TextFormatOption::Header2:
			return fontManager->Get(FontWeight::Bold, textSize + 8);
			break;
		case TextFormatOption::Header3:
			return fontManager->Get(FontWeight::Bold, textSize + 4);
			break;
		case TextFormatOption::InlineCode:
		case TextFormatOption::CodeBlock:
			return  codeFontManager->Get(gui::FontWeight::Regular, textSize - 2);
			break;
		default:
			return fontManager->Get(defaultWeight, textSize);
			break;
		}
	}

	inline Vector2f GetTextBoxSizeFormatted(const std::string& text,
		FontManager* fontManager,
		FontManager* codeFontManager,
		uint32_t textSize,
		FontWeight defaultWeight,
		Vector2i position,
		float textWrap, Colour col, FloatRect bounds, float baseLine, std::vector<TextFormat> formatting)
	{
		float x = 0.0f;

		uint32_t lineCount = 0;

		float maxX = 0.0f, maxY = 0.0f;

		float lineOffset = 0.0f;

		Font* font = fontManager->Get(defaultWeight, textSize);

		for (uint32_t i = 0; i < text.size(); i++)
		{
			// lets see if this index is formatted
			TextFormatOption formatOption = TextFormatOption::None;
			uint32_t currentFormatEnd = UINT32_MAX;
			uint32_t currentFormatStart = UINT32_MAX;

			for (auto& formats : formatting)
			{
				if (i > formats.start && i < formats.end)
				{
					formatOption = formats.option;
					currentFormatEnd = formats.end;
					currentFormatStart = formats.start;
				}
			}



			font = GetFontForFormat(formatOption, fontManager, codeFontManager, defaultWeight, textSize);


			if (i == currentFormatStart || i == currentFormatEnd)
				x += font->GetPixelSize();


			uint32_t codepoint = text[i];

			if (codepoint == '\n')
			{
				x = 0;
				lineCount++;
				lineOffset += font->GetLineSpacing();
				//continue;
			}

			if (codepoint == '\t')
			{
				x += font->GetCodePointData(' ').advance * 4;
				//continue;
			}


			Alphabet alphabet = font->GetAlphabetForCodepoint(codepoint);
			GlyphData data = font->GetCodePointData(codepoint);


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
					lineOffset += font->GetLineSpacing();
				}
			}

			float xpos = x + data.bearingX;
			float ypos = -(float)data.bearingY + lineOffset + baseLine;

			//xpos += (float)position.x;
			//ypos += (float)position.y;

			//m_GlyphPos[i] = { xpos, ypos };

			x += (float)data.advance;


			if (maxX < xpos + (float)data.advance)
				maxX = xpos + (float)data.advance;

			if (maxY < ypos + font->GetMaxHeight())
				maxY = ypos + font->GetMaxHeight();


		}

		return { maxX, maxY };

	}

	inline void RenderTextSoftwareFormatted(Image& img, const std::string& text,
		FontManager* fontManager,
		FontManager* codeFontManager,
		uint32_t textSize,
		FontWeight defaultWeight,
		Vector2i position,
		float textWrap, Colour col, FloatRect bounds, float baseLine, std::vector<TextFormat> formatting,
		uint32_t formatHideExcludeStart = 0, uint32_t formatHideExcludeEnd = 0, Colour baseCol = { 1.0f, 1.0f, 1.0f, 1.0f})
	{
		float x = 0.0f;

		uint32_t lineCount = 0;

		float maxX = 0.0f, maxY = 0.0f;

		Font* font = fontManager->Get(defaultWeight, textSize);

		float lineOffset = 0.0f;

		for (uint32_t i = 0; i < text.size(); i++)
		{
			// lets see if this index is formatted
			TextFormatOption formatOption = TextFormatOption::None;

			uint32_t currentFormatEnd = UINT32_MAX;
			uint32_t currentFormatStart = UINT32_MAX;
			uint32_t currentStartFormatterSize = 0;
			uint32_t currentEndFormatterSize = 0;

			for (auto& formats : formatting)
			{
				if (i > formats.start - formats.formatterStartSize && i < formats.end + formats.formatterEndSize)
				{
					if (i > formats.start && i < formats.end)
					{
						formatOption = formats.option;
					}
					currentFormatEnd = formats.end;
					currentFormatStart = formats.start;
					currentStartFormatterSize = formats.formatterStartSize;
					currentEndFormatterSize = formats.formatterEndSize;
				}
			}

			bool renderChar = true;
			bool formatChar = false;

			if (currentFormatStart != UINT32_MAX && currentFormatEnd != UINT32_MAX)
			{
				uint32_t min = currentFormatStart - currentStartFormatterSize;
				uint32_t max = currentFormatEnd + currentEndFormatterSize;

				bool minIntersects = (min >= formatHideExcludeStart && min <= formatHideExcludeEnd);
				bool maxIntersects = (max >= formatHideExcludeStart && max <= formatHideExcludeEnd);


				if (!minIntersects && !maxIntersects || formatHideExcludeStart == formatHideExcludeEnd)
				{

					if (i >= currentFormatStart - currentStartFormatterSize && i <= currentFormatStart)
					{
						renderChar = false;
					}
					else if (i >= currentFormatEnd && i <= currentFormatEnd + currentEndFormatterSize)
					{
						renderChar = false;
					}
				}
				else
				{
					if (i >= currentFormatStart - currentStartFormatterSize && i <= currentFormatStart)
					{
						formatChar = true;
					}
					else if (i >= currentFormatEnd && i <= currentFormatEnd + currentEndFormatterSize)
					{
						formatChar = true;
					}
				}
			}



			Font* font = GetFontForFormat(formatOption, fontManager, codeFontManager, defaultWeight, textSize);


			//if (i == currentFormatStart || i == currentFormatEnd )
			//	x += font->GetPixelSize();

			// Strike through doesn't affect the font at all
			bool renderStrikeThrough = formatOption == TextFormatOption::StrikeThrough ? true : false;

			uint32_t codepoint = text[i];

			if (codepoint == '\n')
			{
				x = 0.0f;
				lineCount++;
				lineOffset += font->GetLineSpacing();
				continue;
			}

			if (codepoint == '\t')
			{
				x += font->GetCodePointData(' ').advance * 4;
				continue;
			}


			Alphabet alphabet = font->GetAlphabetForCodepoint(codepoint);
			GlyphData data = font->GetCodePointData(codepoint);


			// This is if word wrapped is enabled
			if (textWrap != 0.0f)
			{
				// We want to wrap per word so lets get the word lengths
				int wordLength = 0;
				int wordOffset = x;
				bool forceNewLine = false;
				for (uint32_t w = i; w < text.size(); w++)
				{
					if (text[w] == ' ' || text[w] == '\n')
						break;

					uint32_t cp = text[w];

					GlyphData d = font->GetCodePointData(cp);
					wordOffset += d.advance;
					wordLength += d.advance;
				}

				// if the word length is greater than the wrap limit go onto a new line 
				if (wordOffset > textWrap || forceNewLine)
				{
					x = 0;
					lineCount++;
					lineOffset += font->GetLineSpacing();
				}
			}


			float yposNoBearing = lineOffset + baseLine;

			float xpos = x + data.bearingX;
			float ypos = -(float)data.bearingY + yposNoBearing;


			if (maxX < xpos + (float)data.advance)
				maxX = xpos + (float)data.advance;

			Colour col = baseCol;

			if (formatChar)
				col = { 0.25f, 0.25f, 0.25f, 1.0f };

			if (renderChar)
			{
				x += (float)data.advance;



				font->RasterizeGlyph(codepoint, &img, (int)xpos, (int)ypos, col);

				if (renderStrikeThrough)
				{
					uint32_t max = data.w + data.advance;
					if (i == currentFormatEnd - 1)
						max = data.w;

					for (uint32_t p = 0; p < max; p++)
					{
						img.SetPixel((int)xpos + p, (int)yposNoBearing - font->GetAscent() / 2 + 3, col);
					}
				}

			}

		}

	}

	inline Vector2f GetPositionOfCharFormatted(uint32_t idx, const std::string& text, FontManager* fontManager,
		FontManager* codeFontManager,
		uint32_t textSize,
		FontWeight defaultWeight, float textWrap, std::vector<TextFormat> formatting)
	{
		if (idx > text.size())
			idx = text.size();

		float x = 0.0f;

		float xpos = 0.0f;
		float ypos = 0.0f;

		uint32_t lineCount = 0;

		//if (m_String == m_PreviousString)
		//	return;

		float maxX = 0.0f, maxY = 0.0f;
		bool controlCharacter = false;

		Font* font = fontManager->Get(defaultWeight, textSize);

		float lineOffset = 0.0f;

		for (uint32_t i = 0; i < idx; i++)
		{
			controlCharacter = false;

			TextFormatOption formatOption = TextFormatOption::None;

			uint32_t currentFormatEnd = UINT32_MAX;
			uint32_t currentFormatStart = UINT32_MAX;

			for (auto& formats : formatting)
			{
				if (i > formats.start && i < formats.end)
				{
					formatOption = formats.option;
					currentFormatEnd = formats.end;
					currentFormatStart = formats.start;
				}
			}



			Font* font = GetFontForFormat(formatOption, fontManager, codeFontManager, defaultWeight, textSize);



			if (i == currentFormatStart || i == currentFormatEnd)
				x += font->GetPixelSize();

			uint32_t codepoint = text[i];

			if (codepoint == '\n')
			{
				x = 0;
				lineCount++;
				controlCharacter = true;
				lineOffset += font->GetLineSpacing();
			}

			if (codepoint == '\t')
			{
				x += font->GetCodePointData(' ').advance * 4;
				controlCharacter = true;
			}


			Alphabet alphabet = font->GetAlphabetForCodepoint(codepoint);
			GlyphData data = font->GetCodePointData(codepoint);


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
					lineOffset += font->GetLineSpacing();
				}
			}


			if (!controlCharacter)
			{
				x += (float)data.advance;
			}

			xpos = x + data.bearingX;

			ypos = lineOffset;




			if (maxX < xpos + (float)data.advance)
				maxX = xpos + (float)data.advance;

			if (maxY < ypos)
				maxY = ypos;




		}

		return { xpos, ypos };

	}

	inline uint32_t GetNearestCharFromPointFormatted(Vector2f point, uint32_t idx, const std::string& text, FontManager* fontManager,
		FontManager* codeFontManager,
		uint32_t textSize,
		FontWeight defaultWeight, float textWrap, std::vector<TextFormat> formatting)
	{

		float x = 0.0f;
		float y = 0.0f;

		uint32_t lineCount = 0;

		//if (m_String == m_PreviousString)
		//	return;

		float maxX = 0.0f, maxY = 0.0f;

		uint32_t nearest = UINT32_MAX;
		float dist = 1000000.0f;

		float my = point.y;



		if (point.x < 0.0f && point.y < 0.0f)
			return 0;

		for (uint32_t i = 0; i < text.size(); i++)
		{

			TextFormatOption formatOption = TextFormatOption::None;

			uint32_t currentFormatEnd = UINT32_MAX;
			uint32_t currentFormatStart = UINT32_MAX;

			for (auto& formats : formatting)
			{
				if (i > formats.start && i < formats.end)
				{
					formatOption = formats.option;
					currentFormatEnd = formats.end;
					currentFormatStart = formats.start;
				}
			}



			Font* font = GetFontForFormat(formatOption, fontManager, codeFontManager, defaultWeight, textSize);


			if (i == currentFormatStart || i == currentFormatEnd)
				x += font->GetPixelSize();

			uint32_t codepoint = text[i];


			bool controlChar = false;

			// Handle control characters
			// These aren't actually rendered but do effect how the text should be rendered
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




			if (!controlChar)
				x += (float)data.advance;
			y = (float)lineCount * (float)font->GetLineSpacing();


			// test the distance the point
			Vector2f charPoint(x, y);
			float newDist = point.Distance(charPoint);

			// if the new distance is closer than the current closest set this as the current closest
			if (newDist < dist)
			{
				dist = newDist;
				nearest = i;

			}




		}

		if (nearest == UINT32_MAX)
			nearest = text.size();

		return nearest;
	}
}