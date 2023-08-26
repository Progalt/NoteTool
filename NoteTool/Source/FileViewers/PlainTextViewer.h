#pragma once

#include "FileViewer.h"


class PlainTextViewer : public FileViewer
{
public:


private:

	void LoadFileContents() override;

	// Plain text is easy to load :D 
	std::string m_FileContents = "";

	bool m_CodeHighlighting = false;
	bool m_ShowLineNumbers = false;
};