#pragma once

#include <ft2build.h>
#include <freetype/freetype.h>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include "Image.h"
#include "Platform.h"

enum class Alphabet
{
	Latin
};
struct GlyphData
{
	int w = 0, h = 0, x = 0, y = 0;
	int bearingX = 0, bearingY = 0;
	uint32_t advance = 0;
};

class Font
{
public:

	Font();
	~Font();

	void SetFont(const std::string& path, uint32_t pixelSize, bool aa = true);

	void LoadAlphabet(Alphabet alphabet);

	Image* GetImage(Alphabet alphabet);

	GPUTexture* GetTexture(Alphabet alphabet);

	Image RasterizeGlyph(const uint32_t codepoint);

	void RasterizeGlyph(const uint32_t codepoint, Image* img, uint32_t x, uint32_t y);

	GlyphData GetCodePointData(const uint32_t codepoint);

	Alphabet GetAlphabetForCodepoint(const uint32_t codepoint);

	uint32_t GetPixelSize() const { return m_PixelSize; }

	uint32_t GetLineSpacing() { return (uint32_t)m_Face->height / 64; }

private:

	void LoadGlyphData(const uint32_t codepoint);

	void PackGlyphs(Alphabet alphabet);

	struct Glyph
	{
		int x = 0, y = 0;
		int w = 0, h = 0;
		int bearingX = 0, bearingY = 0;
		uint32_t codepoint = 0;

		uint32_t advance = 0;

		bool packed = false;
	};

	struct GlyphPack
	{

		uint32_t startCodepoint = 0;

		std::vector<Glyph> glyphs;

		uint32_t atlasWidth, atlasHeight;

		Image img;
		GPUTexture texture;
	};

	uint32_t m_PackingPadding = 2;

	std::unordered_map<Alphabet, GlyphPack> m_Alphabets;

	FT_Library m_FT;

	FT_Face m_Face;
	bool m_LoadedFace;

	uint32_t m_PixelSize;

	bool m_AntiAlias;

	int m_Flags = 0;
};