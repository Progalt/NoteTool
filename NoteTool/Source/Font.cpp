
#include "Font.h"
#include <algorithm>
#include "ProfileUtil.h"

typedef std::pair<uint32_t, uint32_t> AlphabetCodepoints;

std::vector<AlphabetCodepoints> alphabetCodepoints =
{
	{ 0,  127 }		// Latin Script
};

Font::Font()
{
	if (FT_Init_FreeType(&m_FT) != 0)
	{

	}
}

Font::~Font()
{

	if (m_LoadedFace)
		FT_Done_Face(m_Face);

	FT_Done_FreeType(m_FT);
}

void Font::SetFont(const std::string& path, uint32_t pixelSize, bool aa)
{
	if (FT_New_Face(m_FT, path.c_str(), 0, &m_Face) != 0)
	{
		printf("Failed to load font\n");
		return;
	}

	if (FT_Set_Pixel_Sizes(m_Face, 0, pixelSize) != 0)
	{
		printf("Failed to set font size\n");
		return;
	}

	m_LoadedFace = true;
	m_AntiAlias = aa;
	if (!aa)
		m_Flags = 0;
	else
		m_Flags = 0;

	m_PixelSize = pixelSize;
}

void Font::RasterizeGlyph(const uint32_t codepoint, Image* img, uint32_t x, uint32_t y, Colour col)
{
	

	uint32_t index = FT_Get_Char_Index(m_Face, codepoint);
	if (FT_Load_Glyph(m_Face, index, FT_LOAD_RENDER | m_Flags) != 0)
	{
		printf("Failed to load char\n");
	}


	if (x + m_Face->glyph->bitmap.width > img->GetWidth() ||
		y + m_Face->glyph->bitmap.rows > img->GetHeight())
	{
		printf("Glyph too big to fit in image at location\n");
		return;
	}

	uint8_t r = col.AsByte(0), g = col.AsByte(1), b = col.AsByte(2);

	for (int i = 0; i < (int)m_Face->glyph->bitmap.rows; i++)
	{
		for (int j = 0; j < (int)m_Face->glyph->bitmap.width; j++)
		{

			unsigned char p = m_Face->glyph->bitmap.buffer[i * m_Face->glyph->bitmap.width + j];


			img->SetPixel(x + j, y + i, r, g, b, p);

		}

	}

}

Image Font::RasterizeGlyph(const uint32_t codepoint)
{
	Image img;

	uint32_t index = FT_Get_Char_Index(m_Face, codepoint);
	if (FT_Load_Glyph(m_Face, index, FT_LOAD_NO_BITMAP | m_Flags) != 0)
	{
		printf("Failed to load char\n");
	}

	img.New(m_Face->glyph->bitmap.width, m_Face->glyph->bitmap.rows, 4);
	img.Fill({ 255, 255, 255, 255 });	// White

	RasterizeGlyph(codepoint, &img, 0, 0);

	return img;

}

void Font::LoadAlphabet(Alphabet alphabet)
{
	m_Alphabets[alphabet].startCodepoint = alphabetCodepoints[(int)alphabet].first;
	m_Alphabets[alphabet].glyphs.resize(alphabetCodepoints[(int)alphabet].second - alphabetCodepoints[(int)alphabet].first);

	for (uint32_t i = alphabetCodepoints[(int)alphabet].first; i < alphabetCodepoints[(int)alphabet].second; i++)
	{
		LoadGlyphData(i);
	}

	m_Alphabets[alphabet].atlasWidth = 512;
	m_Alphabets[alphabet].atlasHeight = 512;

	PackGlyphs(alphabet);

	// Once all the glyphs are packed we can now rasterize them

	// Create the image with the atlas size
	Image img;
	img.New(m_Alphabets[alphabet].atlasWidth, m_Alphabets[alphabet].atlasHeight, 4);

	for (auto& g : m_Alphabets[alphabet].glyphs)
	{
		RasterizeGlyph(g.codepoint, &img, g.x, g.y);
	}

	m_Alphabets[alphabet].img = img;
	m_Alphabets[alphabet].texture.CreateFromImage(img);

}

GlyphData Font::GetCodePointData(const uint32_t codepoint)
{

	Alphabet codepointAlphabet = GetAlphabetForCodepoint(codepoint);

	uint32_t arrayOffset = codepoint - alphabetCodepoints[(int)codepointAlphabet].first;

	if (arrayOffset > m_Alphabets[codepointAlphabet].glyphs.size())
		return GlyphData();

	GlyphData data;
	data.advance = m_Alphabets[codepointAlphabet].glyphs[arrayOffset].advance;
	data.w = m_Alphabets[codepointAlphabet].glyphs[arrayOffset].w;
	data.h = m_Alphabets[codepointAlphabet].glyphs[arrayOffset].h;
	data.x = m_Alphabets[codepointAlphabet].glyphs[arrayOffset].x;
	data.y = m_Alphabets[codepointAlphabet].glyphs[arrayOffset].y;
	data.bearingX = m_Alphabets[codepointAlphabet].glyphs[arrayOffset].bearingX;
	data.bearingY = m_Alphabets[codepointAlphabet].glyphs[arrayOffset].bearingY;

	return data;
}

Alphabet Font::GetAlphabetForCodepoint(const uint32_t codepoint)
{
	for (uint32_t i = 0; i < alphabetCodepoints.size(); i++)
	{
		if (codepoint > alphabetCodepoints[i].first && codepoint < alphabetCodepoints[i].second)
			return (Alphabet)i;
	}

	return Alphabet::Latin;
}

void Font::LoadGlyphData(const uint32_t codepoint)
{
	uint32_t index = FT_Get_Char_Index(m_Face, codepoint);
	if (FT_Load_Glyph(m_Face, index, FT_LOAD_NO_BITMAP | m_Flags) != 0)
	{
		printf("Failed to load char\n");
	}

	Alphabet codepointAlphabet = GetAlphabetForCodepoint(codepoint);

	uint32_t arrayOffset = codepoint - alphabetCodepoints[(int)codepointAlphabet].first;

	m_Alphabets[codepointAlphabet].glyphs[codepoint].codepoint = codepoint;

	m_Alphabets[codepointAlphabet].glyphs[codepoint].advance = m_Face->glyph->advance.x >> 6;

	m_Alphabets[codepointAlphabet].glyphs[codepoint].w = m_Face->glyph->bitmap.width;
	m_Alphabets[codepointAlphabet].glyphs[codepoint].h = m_Face->glyph->bitmap.rows;

	m_Alphabets[codepointAlphabet].glyphs[codepoint].bearingX = m_Face->glyph->bitmap_left;
	m_Alphabets[codepointAlphabet].glyphs[codepoint].bearingY = m_Face->glyph->bitmap_top;
}

void Font::PackGlyphs(Alphabet alphabet)
{
	std::vector<Glyph>& glyphs = m_Alphabets[alphabet].glyphs;

	// make copy
	std::vector<Glyph> glyph2(glyphs.begin(), glyphs.end());

	std::sort(glyph2.begin(), glyph2.end(), [&](Glyph a, Glyph b) { return a.h > b.h; });

	// this is a naive rect packing method

	int xPos = 0;
	int yPos = 0;
	int largestHThisRow = 0;

	for (Glyph& rect : glyph2)
	{
		if ((xPos + rect.w + m_PackingPadding) > m_Alphabets[alphabet].atlasWidth)
		{
			yPos += largestHThisRow;
			xPos = 0;
			largestHThisRow = 0;
		}

		// If we go off the bottom edge of the image, then we've failed
		if ((yPos + rect.h + m_PackingPadding) > m_Alphabets[alphabet].atlasHeight)
		{
			printf("Cannot pack anymore rects into image\n");
			break;
		}

		// This is the position of the rectangle
		rect.x = xPos;
		rect.y = yPos;

		// Move along to the next spot in the row
		xPos += rect.w + m_PackingPadding;

		// Just saving the largest height in the new row
		if (rect.h > largestHThisRow)
			largestHThisRow = rect.h + m_PackingPadding;

		// Success!
		rect.packed = true;
	}

	uint32_t startCodepoint = m_Alphabets[alphabet].startCodepoint;

	// copy the glyphs back
	for (auto& glyph : glyph2)
	{
		glyphs[glyph.codepoint - startCodepoint] = glyph;
		//printf("Glyph Rect Pack: %d, %d\n", glyph.x, glyph.y);
	}
}

Image* Font::GetImage(Alphabet alphabet)
{
	if (m_Alphabets.find(alphabet) != m_Alphabets.end())
		return &m_Alphabets[alphabet].img;

	return nullptr;
}

GPUTexture* Font::GetTexture(Alphabet alphabet)
{
	if (m_Alphabets.find(alphabet) != m_Alphabets.end())
		return &m_Alphabets[alphabet].texture;

	return nullptr;
}