
#include "Formatter.h"

namespace gui
{
	Formatter::Formatter(const std::string& str)
	{
		// Bit hacky to add newlines to fix some problems when headers are at the current end of the file
		m_String = str + "\n";
		m_Ptr = 0;

		Parse();
	}

	void Formatter::Parse()
	{

		char escape = '\\';

		while (m_Ptr < m_String.size())
		{
			char c = m_String[m_Ptr];

			switch (c)
			{
			case '*':

				if (PeekBack() == escape)
					break;

				if (Peek() == '*' && Peek(2) == '*')
				{
					uint32_t end = FindNext("***", 2);

					if (end != UINT32_MAX)
					{
						AddFormat(m_Ptr + 2, end, gui::TextFormatOption::Emphasis, 3, 3);

						while (m_Ptr != end + 3)
							m_Ptr++;
					}

					
				}
				else if (Peek() == '*' && PeekBack() != '*')
				{
					// We are at the start of a bold

					uint32_t end = FindNext("**", 1);



					if (end != UINT32_MAX)
					{
						if (Peek(end - m_Ptr + 2) != '*')
						{

							AddFormat(m_Ptr + 1, end, gui::TextFormatOption::Bold, 2, 2);

							while (m_Ptr != end + 2)
								m_Ptr++;
						}

					}


				
				}
				else if (PeekBack() != '*' && Peek() != '*')
				{
					// This could be italics

					uint32_t end = FindNext("*", 1);
					
				
					if (end != UINT32_MAX)
					{
						AddFormat(m_Ptr, end, gui::TextFormatOption::Italic, 1, 1);

						while (m_Ptr != end + 1)
							m_Ptr++;
					}
				}
				


				break;
			case '#':

				if (PeekBack() == escape)
					break;

				if (Peek() == ' ' && PeekBack() != '#')
				{
					// Header 1

					uint32_t end = FindNext("\n", 1);


					if (end != UINT32_MAX)
					{
						AddFormat(m_Ptr, end, gui::TextFormatOption::Header1, 1, 0);

						while (m_Ptr != end )
							m_Ptr++;
					}
				}
				else if (Peek() == '#' && Peek(2) ==' ' && PeekBack() != '#')
				{
					// Header 2

					uint32_t end = FindNext("\n", 1);


					if (end != UINT32_MAX)
					{
						AddFormat(m_Ptr + 1, end, gui::TextFormatOption::Header2, 2, 0);

						while (m_Ptr != end )
							m_Ptr++;
					}
				}
				else if (Peek() == '#' && Peek(2) == '#' && Peek(3) == ' ' && PeekBack() != '#')
				{
					// Header 3

					uint32_t end = FindNext("\n", 1);


					if (end != UINT32_MAX)
					{
						AddFormat(m_Ptr + 2, end, gui::TextFormatOption::Header3, 3, 0);

						while (m_Ptr != end )
							m_Ptr++;
					}
				}
				break;
			case '~':

				if (PeekBack() == escape)
					break;

				if (Peek() == '~')
				{
					// We are at the start of a bold

					uint32_t end = FindNext("~~", 1);



					if (end != UINT32_MAX)
					{
						if (Peek(end - m_Ptr + 2) != '*')
						{

							AddFormat(m_Ptr + 1, end, gui::TextFormatOption::StrikeThrough, 2, 2);

							while (m_Ptr != end + 2)
								m_Ptr++;
						}

					}



				}
				break;
			case '`':

				if (PeekBack() == escape)
					break;

				if (Peek() != '`')
				{
					uint32_t end = FindNext("`", 1);

					if (FindNext("\n", 1) < end)
						end = UINT32_MAX;


					if (end != UINT32_MAX)
					{

						AddFormat(m_Ptr, end, gui::TextFormatOption::InlineCode, 1, 1);



						while (m_Ptr != end + 1)
							m_Ptr++;


					}
				}
				else if (Peek() == '`' && Peek(2) == '`' && PeekBack() != '`')
				{
					uint32_t end = FindNext("```", 2);



					if (end != UINT32_MAX)
					{

						AddFormat(m_Ptr + 2, end, gui::TextFormatOption::CodeBlock, 3, 3);

						while (m_Ptr != end + 1)
							m_Ptr++;


					}
				}
				break;
			case '-':

				if (PeekBack() == escape)
					break;

				if (Peek() == '-' && Peek(2) == '-' && PeekBack() != '-')
				{
					uint32_t end = FindNext("\n", 1);

					if (end != UINT32_MAX)
					{

						AddFormat(m_Ptr + 2, end, gui::TextFormatOption::HorizontalRule, 3, 0);

						while (m_Ptr != end + 1)
							m_Ptr++;


					}
				}
				break;
			}


			m_Ptr++;
		}



	}


	

	uint32_t Formatter::FindNext(const std::string& str, uint32_t offset)
	{

		for (uint32_t i = m_Ptr + offset; i < m_String.size(); i++)
		{
			
			if (i + str.size() > m_String.size())
				return UINT32_MAX;

			std::string substr = m_String.substr(i, str.size());

			if (substr == str)
				return i;
		}

		return UINT32_MAX;
	}
}