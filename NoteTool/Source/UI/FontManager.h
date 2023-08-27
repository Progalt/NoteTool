#pragma once
#include <string>
#include "../Font.h"
#include <unordered_map>

namespace gui
{
	enum class FontWeight
	{
		Regular, Bold, Italics, Light,
		Black, ExtraLight
	};

	struct FontEntry
	{
		FontWeight weight;
		uint32_t pixelSize;

		bool operator==(const FontEntry& fe) const
		{
			return weight == fe.weight && pixelSize == fe.pixelSize;
		}
	};

	

	class FontManager
	{
	public:

		void SetInitialFont(const std::string& name);

		Font* Get(FontWeight weight, uint32_t pixelSize);

	private:

		void AddFont(FontWeight weight, uint32_t pixelSize);

		std::string GetFontPath(FontWeight weight, uint32_t pixelSize);

		struct FontEntry_hash_fn
		{
			size_t operator()(const FontEntry& fe) const
			{
				size_t h1 = std::hash<int>()((int)fe.weight);
				size_t h2 = std::hash<uint32_t>()((uint32_t)fe.pixelSize);

				return h1 ^ h2;
			}
		};
		
		std::string m_InitialFont;

		std::unordered_map<FontEntry, Font, FontEntry_hash_fn> m_Fonts;

		

	};
}