#pragma once

#include "FileViewer.h"
#include "../UI/TextBox.h"
#include "../UI/Text.h"


class PlainTextViewer : public FileViewer
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

private:

	void LoadFileContents() override;

	void InitialiseGUIElements() override;

	// Plain text is easy to load :D 
	std::string m_FileContents = "";

	bool m_CodeHighlighting = false;
	bool m_ShowLineNumbers = false;

	gui::TextBox* m_TextBox;

	gui::Text* m_Title;
	gui::Text* m_FileExt;

};