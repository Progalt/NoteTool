
#include "Formatter.h"

namespace gui
{
	Formatter::Formatter(const std::string& str)
	{
		m_String = str;
		m_FormattedString = str;
		m_Ptr = 0;

		Parse();
	}

	void Formatter::Parse()
	{

		while (m_Ptr < m_String.size())
		{
			char c = m_FormattedString[m_Ptr];

			switch (c)
			{
			case '*':

				if (Peek() == '*' && Peek(2) == '*')
				{
					uint32_t end = FindNext("***", 2);

					if (end != UINT32_MAX)
					{
						AddFormat(m_Ptr + 2, end, gui::TextFormatOption::Emphasis);

						while (m_Ptr != end + 3)
							m_Ptr++;
					}

					
				}
				else if (Peek() == '*')
				{
					// We are at the start of a bold

					uint32_t end = FindNext("**", 1);



					if (end != UINT32_MAX)
					{
						if (Peek(end - m_Ptr + 2) != '*')
						{

							AddFormat(m_Ptr + 1, end, gui::TextFormatOption::Bold);

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
						AddFormat(m_Ptr, end, gui::TextFormatOption::Italic);

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