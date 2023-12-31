
#include "FontManager.h"
#include <filesystem>
#include <cassert>

namespace gui
{
	void FontManager::SetInitialFont(const std::string& name)
	{
		m_InitialFont = name;

		AddFont(gui::FontWeight::Regular, 12);
	}


	std::string FontManager::GetFontPath(FontWeight weight, uint32_t pixelSize)
	{
		// Fonts are required to be stored in a *fontname*-*font-weight*.ttf style
		// Within the same folder which is themes. 
		std::string weightStr;

		switch (weight)
		{
		case FontWeight::Regular:
			weightStr = "Regular";
			break;
		case FontWeight::Bold:
			weightStr = "Bold";
			break;
		case FontWeight::Italic:
			weightStr = "Italic";
			break;
		case FontWeight::Light:
			weightStr = "Light";
			break;
		case FontWeight::Black:
			weightStr = "Black";
			break;
		case FontWeight::ExtraLight:
			weightStr = "ExtraLight";
			break;
		case FontWeight::BoldItalic:
			weightStr = "BoldItalic";
			break;
		case FontWeight::ExtraLightItalic:
			weightStr = "ExtraLightItalic";
			break;
		}

		std::string filepath = "Themes/" + m_InitialFont + "-" + weightStr + ".ttf";

		return filepath;
	}

	Font* FontManager::Get(FontWeight weight, uint32_t pixelSize)
	{
		if (m_Fonts.find({ weight, pixelSize }) != m_Fonts.end())
			return &m_Fonts[{ weight, pixelSize }];

		std::string filepath = GetFontPath(weight, pixelSize);

		bool exists = std::filesystem::exists(filepath);

		if (!exists)
			return Get(gui::FontWeight::Regular, pixelSize);
		
		AddFont(weight, pixelSize);

		return &m_Fonts[{ weight, pixelSize }];
	}

	void FontManager::AddFont(FontWeight weight, uint32_t pixelSize)
	{

		std::string filepath = GetFontPath(weight, pixelSize);

		bool exists = std::filesystem::exists(filepath);

		assert(exists && "Filepath specified for font does not exist");

		if (exists)
		{
			Font* font = &m_Fonts[{ weight, pixelSize }];

			font->SetFont(filepath, pixelSize);
			font->LoadAlphabet(Alphabet::Latin);
		}

	}
}