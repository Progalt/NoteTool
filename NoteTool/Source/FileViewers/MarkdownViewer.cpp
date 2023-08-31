
#include "MarkdownViewer.h"

#include <fstream>
#include <sstream>
#include <cassert>
#include "../WorkspaceUI.h"

MarkdownViewer::~MarkdownViewer()
{

}

void MarkdownViewer::LoadFileContents()
{
	std::ifstream file(m_File->path);

	assert(file.is_open());

	std::stringstream stream;
	stream << file.rdbuf();

	m_FileContents = stream.str();

}

void MarkdownViewer::InitialiseGUIElements()
{

	SetupTitleAndExt();

	float padding = 30.0f;
	float yPos = m_FontManager->Get(gui::FontWeight::Bold, 28)->GetPixelSize();


	m_TextBox = m_Panel->NewChild<gui::TextBox>();


	m_TextBox->SetFontManager(m_FontManager);
	m_TextBox->SetCodeFontManager(m_CodeFontManager);
	m_TextBox->SetFontSize(16);



	m_TextBox->SetBounds({ padding, yPos + 45.0f, m_Panel->GetBounds().w - padding * 3.0f, m_Panel->GetBounds().h });
	m_TextBox->string = m_FileContents;
	m_TextBox->SetAnchor(gui::Anchor::BottomRight);
	m_TextBox->SetLockPosition(true);
	m_TextBox->SetShouldFormat(true);

	float scrollPadding = 64.0f;

	m_TextBox->SetOnEditCallback([&]() { 
		m_Saved = false; 
		m_SavedCircle->SetVisible(true); 
		printf("Panel  height: %f\n", m_Panel->GetBounds().h);
		printf("Text box height: %f\n", m_TextBox->GetTextBoxHeight());
		if (m_TextBox->GetTextBoxHeight() > m_Panel->GetBounds().h)
		{
			m_Panel->SetScrollable(true);
			m_Panel->SetScrollableArea(Vector2f(0.0f, m_TextBox->GetTextBoxHeight()));
		}
		else
		{
			m_Panel->SetScrollableArea(Vector2f(0.0f, 0.0f));
		}
		 });


}

void MarkdownViewer::Save()
{
	m_FileContents = m_TextBox->string;

	std::ofstream file(m_File->PathWithoutName() + m_File->NameWithoutExtension() + m_File->extension);

	assert(file.is_open());

	file << m_FileContents;

	file.close();

	m_Saved = true;

	m_SavedCircle->SetVisible(false);
}