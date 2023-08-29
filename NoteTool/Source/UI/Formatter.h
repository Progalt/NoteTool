#pragma once

#include <string>
#include <vector>



namespace gui
{
	enum class TextFormatOption
	{
		None,
		Bold,
		Italic,
		Emphasis
	};

	struct TextFormat
	{
		uint32_t start, end;

		TextFormatOption option;
	};


	class Formatter
	{
	public:
		Formatter(const std::string& str);

		std::vector<TextFormat> GetFormatting() const { return m_Output; };

		std::string GetStringWithoutFormatting() const { return m_FormattedString; }
	private:

		std::vector<TextFormat> m_Output;

		uint32_t m_Ptr;
		std::string m_String;

		std::string m_FormattedString = "";

		void Parse();

		char Peek(uint32_t offset = 1)
		{
			if (m_Ptr + offset < m_String.size())
				return m_String[m_Ptr + offset];

			return '\0';
		}

		char PeekBack()
		{
			if (m_Ptr > 0)
				return m_String[m_Ptr - 1];

			return 0;
		}

		uint32_t FindNext(const std::string& str, uint32_t offset = 0);

		void AddFormat(uint32_t start, uint32_t end, TextFormatOption opt) { m_Output.push_back({ start, end, opt }); }
		
	};
}