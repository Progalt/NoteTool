
#include "Formatter.h"

namespace gui
{
	Formatter::Formatter(const std::string& str)
	{
		// Bit hacky to add newlines to fix some problems when headers are at the current end of the file
		m_String =  str + "\n";
		m_Ptr = 0;

		Parse();
	}

	void Formatter::Parse()
	{

		char escape = '\\';
		m_CurrentLine = 0;

		while (m_Ptr < m_String.size())
		{
			char c = m_String[m_Ptr];

			switch (c)
			{
			case '*':

				if (PeekBack() == escape)
					break;

				if (Match('*'))
				{

					if (Match('*'))
					{
						if (Match('*'))
						{
							uint32_t start = m_Ptr;

							while (PeekStr(1, 3) != "***" && !IsAtEnd()) { Advance(); }


							uint32_t end = m_Ptr;

							AddFormat(start, end, gui::TextFormatOption::Emphasis, 3, 3);
							Advance(3);
						}
						else
						{
							// Bold

							uint32_t start = m_Ptr;

							while (PeekStr(1, 2) != "**" && !IsAtEnd()) { Advance(); }
							

							uint32_t end = m_Ptr;

							AddFormat(start, end, gui::TextFormatOption::Bold, 2, 2);
							Advance(2);							
						}
					}
					else
					{
						uint32_t start = m_Ptr;

						while (Peek() != '*' && !IsAtEnd()) { Advance(); }

						uint32_t end = m_Ptr;

						AddFormat(start, end, gui::TextFormatOption::Italic, 1, 1);
						Advance();
						

					}
				}
				


				break;
			case '#':

				if (PeekBack() == escape)
					break;

				if (Match('#'))
				{
					TextFormatOption option = gui::TextFormatOption::None;
					uint32_t formatterStartSize = 0;

					
					if (Match('#'))
					{
						if (Match('#'))
						{
							option = gui::TextFormatOption::Header3;
							formatterStartSize = 4;
						}
						else if (Match(' '))
						{
							option = gui::TextFormatOption::Header2;
							formatterStartSize = 3;
						}
					}
					else if (Match(' '))
					{
						option = gui::TextFormatOption::Header1;
						formatterStartSize = 2;
					}

					uint32_t start = m_Ptr;

					while (Peek() != '\n' && !IsAtEnd()) { Advance(); }
					Advance();

					uint32_t end = m_Ptr;

					AddFormat(start, end, option, formatterStartSize, 0);
				}
				break;
			case '~':

				if (PeekBack() == escape)
					break;

				if (Match('~'))
				{
					if (Match('~'))
					{
						uint32_t start = m_Ptr;

						while (PeekStr(1, 2) != "~~" && !IsAtEnd()) { Advance(); }


						uint32_t end = m_Ptr;

						AddFormat(start, end, gui::TextFormatOption::StrikeThrough, 2, 2);
						Advance(2);
					}



				}
				break;
			case '`':

				if (PeekBack() == escape)
					break;

				if (Match('`'))
				{
					if (Match('`') && Match('`'))
					{
						uint32_t start = m_Ptr;

						while (PeekStr(1, 3) != "```" && !IsAtEnd()) { Advance(); }


						uint32_t end = m_Ptr;

						AddFormat(start, end, gui::TextFormatOption::CodeBlock, 3, 3);
						Advance(3);
					}
					else
					{
						// Inline code

						uint32_t start = m_Ptr;

						while (Peek() != '`' && !IsAtEnd()) { Advance(); }

						uint32_t end = m_Ptr;

						AddFormat(start, end, gui::TextFormatOption::InlineCode, 1, 1);
						Advance();
					}
				}
				break;
			case '-':

				if (PeekBack() == escape)
					break;

				if (Match('-'))
				{
					bool m2 = Match('-');
					bool m3 = Match('-');

					if (m2 && m3)
					{
						uint32_t start = m_Ptr;

						while (Peek() != '\n' && !IsAtEnd()) { Advance(); }

						uint32_t end = m_Ptr;

						AddFormat(start, end, gui::TextFormatOption::HorizontalRule, 3, 0);
						Advance();

					}
				}

				break;
			case '=':

				if (PeekBack() == escape)
					break;

				if (Match('='))
				{
					if (Match('='))
					{
						uint32_t start = m_Ptr;

						while (PeekStr(1, 2) != "==" && !IsAtEnd()) { Advance(); }


						uint32_t end = m_Ptr;

						AddFormat(start, end, gui::TextFormatOption::Highlight, 2, 2);
						Advance(2);
					}



				}

				break;
			case '>':
				if (Match('>'))
				{
					uint32_t start = m_Ptr;
					AddFormat(start, start, gui::TextFormatOption::Quote, 1, 0);
				}
				break;
			case '\n':
				m_CurrentLine++;
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