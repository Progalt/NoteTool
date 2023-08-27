#pragma once

#include "FileViewer.h"
#include "../UI/TextBox.h"


class PlainTextViewer : public FileViewer
{
public:

	void Hide() override
	{
		m_TextBox->SetVisible(false);
	}

	void Show() override
	{
		m_TextBox->SetVisible(true);
	}

	void Save() override;

private:

	void LoadFileContents() override;

	void InitialiseGUIElements() override;

	// Plain text is easy to load :D 
	std::string m_FileContents = "";

	bool m_CodeHighlighting = false;
	bool m_ShowLineNumbers = false;

	gui::TextBox* m_TextBox;

};