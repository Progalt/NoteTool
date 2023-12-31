
#include "PlainTextViewer.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include "../WorkspaceUI.h"

void PlainTextViewer::LoadFileContents()
{
	std::ifstream file(m_File->path);

	assert(file.is_open());

	std::stringstream stream;
	stream << file.rdbuf();

	m_FileContents = stream.str();

}

void PlainTextViewer::InitialiseGUIElements()
{

	SetupTitleAndExt();

	float padding = 30.0f;
	float yPos = m_FontManager->Get(gui::FontWeight::Bold, 28)->GetPixelSize();

	m_TextBox = m_Panel->NewChild<gui::TextBox>();


	if (m_File->isCodeFile)
	{
		m_TextBox->SetFontManager(m_CodeFontManager);
		m_TextBox->SetFontSize(14);
		m_ShowLineNumbers = true;
	}
	else
	{
		m_TextBox->SetFontManager(m_FontManager);
		m_TextBox->SetFontSize(16);
	}
	

	m_TextBox->SetBounds({ padding, yPos + 45.0f, m_Panel->GetBounds().w - padding * 3.0f, m_Panel->GetBounds().h });
	m_TextBox->string = m_FileContents;
	m_TextBox->SetAnchor(gui::Anchor::BottomRight);
	m_TextBox->SetLockPosition(true);

	m_TextBox->SetOnEditCallback([&]() { m_Saved = false; m_SavedCircle->SetVisible(true); });

}

void PlainTextViewer::Save()
{
	m_FileContents = m_TextBox->string;

	std::ofstream file(m_File->PathWithoutName() + m_File->NameWithoutExtension() + m_File->extension);

	assert(file.is_open());

	file << m_FileContents;

	file.close();

	m_Saved = true;

	m_SavedCircle->SetVisible(false);
}