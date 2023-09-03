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
		Emphasis,
		Header1,
		Header2,
		Header3,
		StrikeThrough,
		InlineCode,
		CodeBlock,
		HorizontalRule,
		Highlight
	};

	struct TextFormat
	{
		uint32_t start, end;
		uint32_t formatterStartSize;
		uint32_t formatterEndSize;

		TextFormatOption option;
	};


	class Formatter
	{
	public:
		Formatter(const std::string& str);

		std::vector<TextFormat> GetFormattingForBaseString() const { return m_Output; };



	private:


		std::vector<TextFormat> m_Output;

		uint32_t m_Ptr;
		std::string m_String;

		void Parse();

		char Peek(uint32_t offset = 1)
		{
			if (m_Ptr + offset < m_String.size())
				return m_String[m_Ptr + offset];

			return '\0';
		}

		std::string PeekStr(uint32_t offset = 1, uint32_t count = 1)
		{
			if (m_Ptr + offset + count < m_String.size())
				return m_String.substr(m_Ptr + offset, count);

			return "";
		}

		char PeekBack()
		{
			if (m_Ptr > 0)
				return m_String[m_Ptr - 1];

			return 0;
		}

		bool IsAtEnd()
		{
			return (m_Ptr + 1 >= m_String.size());
		}

		uint32_t FindNext(const std::string& str, uint32_t offset = 0);

		void AddFormat(uint32_t start, uint32_t end, TextFormatOption opt, uint32_t formatterStartSize = 0, uint32_t formatterEndSize = 0) 
		{ 
			m_Output.push_back({ start, end, formatterStartSize, formatterEndSize, opt }); 
		}

		char Advance(uint32_t offset = 1)
		{
			m_Ptr += offset;
			return m_String[m_Ptr];
		}
		
		bool Match(char expected)
		{
			if (IsAtEnd())
				return false;

			if (m_String[m_Ptr] != expected)
				return false;
			
			m_Ptr++;
			return true;

		}
	};
}