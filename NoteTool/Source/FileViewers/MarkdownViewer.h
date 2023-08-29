#pragma once

#include "FileViewer.h"
#include "../UI/TextBox.h"
#include "../UI/Text.h"#
#include <algorithm>


class MarkdownViewer : public FileViewer
{
public:
	void Hide() override
	{
		m_Title->SetVisible(false);
		m_TextBox->SetVisible(false);
		m_FileExt->SetVisible(false);
	}

	void Show() override
	{
		m_Title->SetVisible(true);
		m_TextBox->SetVisible(true);
		m_FileExt->SetVisible(true);
	}

	void Save() override;

	void ToggleFormatting(gui::TextFormatOption option, uint32_t start, uint32_t end)
	{
		auto within = [](uint32_t x, uint32_t min, uint32_t max)
			{

				if (x >= min && x < max)
					return true;

				return false;
			};

		uint32_t newStart = start;
		uint32_t newEnd = end;
		bool addFormat = true;

		uint32_t idx = 0;
		for (auto& format : m_Format)
		{
			if (start == format.start && end == format.end)
			{
				m_Format.erase(m_Format.begin() + idx);
				addFormat = false;
				continue;
			}

			int nmin = std::min(start, end);
			int nmax = std::max(start, end);

			if (within(nmin, format.start, format.end) && !within(nmax, format.start, format.end))
			{
				newStart = format.end - 1;
				format.end = nmin + 1;
				 

			}
			else if (!within(nmin, format.start, format.end) && within(nmax, format.start, format.end))
			{
				newEnd = format.start + 1;
				format.start = nmax - 1;
			
			}
			

			
			idx++;
		}

		if (addFormat)
			m_Format.push_back({ newStart, newEnd, option });


		CleanupFormatting();
	}

	void Refresh()
	{
		if (m_TextBox->string.size() == 0)
			m_Format.clear();

		m_TextBox->SetFormatting(m_Format);
	}

private:

	void CleanupFormatting()
	{
		auto intersects = [](uint32_t min1, uint32_t max1, uint32_t min2, uint32_t max2)
			{
				if (min2 > min1 && min2 < max1 || max2 > min1 && max2 < max1)
					return true;

				return false;
			};

		for (uint32_t i = 0; i < m_Format.size(); i++)
		{
			for (uint32_t j = 0; j < m_Format.size(); j++)
			{
				if (j == i)
					continue;

				gui::TextFormat& f1 = m_Format[i];
				gui::TextFormat& f2 = m_Format[j];

				if (f1.option == f2.option)
				{
					if (intersects(f1.start, f1.end, f2.start, f2.end))
					{
						f1.start = std::min(f1.start, f2.start);
						f1.end = std::max(f1.end, f2.end);
						printf("Merged Formatting \n");
						m_Format.erase(m_Format.begin() + j);
					}
				}
			}
		}
	}

	void LoadFileContents() override;

	void InitialiseGUIElements() override;

	// Plain text is easy to load :D 
	std::string m_FileContents = "";

	bool m_CodeHighlighting = false;
	bool m_ShowLineNumbers = false;

	gui::TextBox* m_TextBox;

	gui::Text* m_Title;
	gui::Text* m_FileExt;

	gui::Text* m_LineNumbers;

	std::vector<gui::TextFormat> m_Format;
};