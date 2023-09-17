
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



	if (!file.is_open())
	{
		assert(false);
		OS::GetInstance().DebugPrint("Cannot open file to load contents\n");

	}

	std::stringstream stream;
	stream << file.rdbuf();

	m_FileContents = stream.str();

}

void MarkdownViewer::InitialiseGUIElements()
{

	SetupTitleAndExt();

	float padding = 30.0f;
	float yPos = m_FontManager->Get(gui::FontWeight::Bold, 28)->GetAscent();

	m_Panel->SetDrawVerticalScrollBar(true);
	Colour col = { 0.06f, 0.06f, 0.06f, 1.0f };
	col.ToSRGB();
	m_Panel->SetHighlightColour(col);

	m_TextBox = m_Panel->NewChild<gui::TextBox>();


	m_TextBox->SetFontManager(m_FontManager);
	m_TextBox->SetCodeFontManager(m_CodeFontManager);
	m_TextBox->SetFontSize(16, gui::FontWeight::Light);



	m_TextBox->SetBounds({ padding, yPos + 55.0f, m_Panel->GetBounds().w - padding * 3.0f, m_Panel->GetBounds().h });
	m_TextBox->string = m_FileContents;
	m_TextBox->SetAnchor(gui::Anchor::BottomRight);
	m_TextBox->SetLockPosition(true);
	m_TextBox->SetShouldFormat(true);
	m_TextBox->SetBlockColour(theme->blockColour);

	float scrollPadding = 128.0f;

	m_TextBox->SetOnEditCallback([&]() { 
		m_Saved = false; 
		m_SavedCircle->SetVisible(true); 
		OS::GetInstance().DebugPrint("Panel  height: %f\n", m_Panel->GetBounds().h);
		OS::GetInstance().DebugPrint("Text box height: %f\n", m_TextBox->GetTextBoxHeight());
	
			m_Panel->SetScrollable(true);
			m_Panel->SetScrollableArea(Vector2f(0.0f, m_TextBox->GetTextBoxHeight()));
		
		
		 });

	m_TextBox->ShowBlankText("Ideas are brewing for sure!");

}

void MarkdownViewer::Save()
{
	m_FileContents = m_TextBox->string;

	std::ofstream file(m_File->PathWithoutName() + m_File->NameWithoutExtension() + m_File->extension);

	if (!file.is_open())
	{
		assert(false);
		OS::GetInstance().DebugPrint("Cannot open file for saving\n");
	}

	file << m_FileContents;

	file.close();

	m_Saved = true;

	m_SavedCircle->SetVisible(false);
}

