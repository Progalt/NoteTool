#pragma once

#include <nlohmann/json.hpp>
#include <fstream>
#include "Colour.h"
#include "Font.h"

using json = nlohmann::json;

enum class FontWeight
{
	Regular, Bold
};

struct LanguageHighlighting
{
	std::vector<std::string> types;
	std::vector<std::string> keywords;
};

static const std::string KnownLanguages[] = {
	"cpp", 
	"java",
	"csharp",
	"python",
	"javascript",
	"html",
	"css",
	"rust",
	"dart"
};

constexpr uint32_t KnownLanguageCount = 8;

class Theme
{
public:

	void LoadFromThemeJSON(const std::string& filepath)
	{
		std::ifstream file(filepath);

		assert(file.is_open());

		json j;
		file >> j;

		info.name = j["name"];
		info.version = j["version"];
		info.author = j["author"];

		if (j.find("use-windows-dark-mode") != j.end())
		{
			useWindowsDarkTheme = j["use-windows-dark-mode"];
		}

		backgroundColour = ColourFromJSON(j["background"]);
		panelBackground = ColourFromJSON(j["panel-bg"]);
		panelHighlight = ColourFromJSON(j["panel-highlight"]);

		accentColour = ColourFromJSON(j["button-accent"]);
		accentHighlight = ColourFromJSON(j["button-highlight"]);
		accentShadow = ColourFromJSON(j["button-shadow"]);

		hoverModifier = ColourFromJSON(j["hover-modifier"]);
		clickModifier = ColourFromJSON(j["click-modifier"]);
		buttonRounding = j["button-rounding"];

		textMain = ColourFromJSON(j["text-main"]);
		textSub = ColourFromJSON(j["text-sub"]);
	}

	void AttemptToLoadLanguageHighlighting()
	{
		for (uint32_t i = 0; i < KnownLanguageCount; i++)
		{

		}
	}

	struct Info
	{
		std::string name;
		std::string version;
		std::string author;
	} info;

	bool useWindowsDarkTheme = false;		// Windows Only
	Colour backgroundColour;
	Colour panelBackground;
	Colour panelHighlight;

	Colour accentColour;
	Colour accentHighlight;
	Colour accentShadow;

	Colour hoverModifier;
	Colour clickModifier;

	Colour textMain;
	Colour textSub;

	float buttonRounding;

	std::unordered_map<std::string, LanguageHighlighting> languageHighlighting;


private:

	Colour ColourFromJSON(json j)
	{
		int r, g, b, a = 255;
		r = j[0];
		g = j[1];
		b = j[2];
		return Colour(r, g, b, a);
	}

};