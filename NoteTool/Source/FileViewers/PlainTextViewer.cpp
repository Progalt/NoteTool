
#include "PlainTextViewer.h"
#include <fstream>
#include <sstream>
#include <cassert>

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

	float padding = 30.0f;

	Font* titleFont = m_FontManager->Get(gui::FontWeight::Bold, 28);


	m_Title = m_Panel->NewChild<gui::Text>();
	m_Title->SetFont(titleFont);
	m_Title->SetString(m_File->NameWithoutExtension());


	

	float yPos = titleFont->GetPixelSize();
	m_Title->SetPosition({ padding, yPos });

	m_FileExt = m_Panel->NewChild<gui::Text>();
	m_FileExt->SetFont(m_FontManager->Get(gui::FontWeight::Italics, 22));
	m_FileExt->SetString(m_File->extension);
	m_FileExt->SetColour({ 0.08f, 0.08f ,0.08f , 1.0f });

	float dif = titleFont->GetAscent() - m_FontManager->Get(gui::FontWeight::Italics, 22)->GetAscent();

	float xPos = gui::GetTextLength(m_File->NameWithoutExtension(), titleFont);
	m_FileExt->SetPosition({ xPos + 35.0f, yPos + dif });

	
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

}

void PlainTextViewer::Save()
{
	m_FileContents = m_TextBox->string;

	std::ofstream file(m_File->path);

	assert(file.is_open());

	file << m_FileContents;

	file.close();
}