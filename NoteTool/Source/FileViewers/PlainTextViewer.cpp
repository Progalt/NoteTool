
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

	float padding = 30.0f;




	m_Title = m_Panel->NewChild<gui::TextBox>();
	m_Title->SetFontManager(m_FontManager);
	m_Title->SetFontSize(28, gui::FontWeight::Bold);
	m_Title->string = m_File->NameWithoutExtension();
	float yPos = m_FontManager->Get(gui::FontWeight::Bold, 28)->GetPixelSize();
	m_Title->SetBounds({ padding, yPos, m_Panel->GetBounds().w, (float)m_FontManager->Get(gui::FontWeight::Bold, 28)->GetMaxHeight() });

	m_Title->SetOnEditCallback([&]()
		{
			float dif = m_FontManager->Get(gui::FontWeight::Bold, 28)->GetMaxHeight();
			float xPos = gui::GetTextLength(m_Title->string, m_FontManager->Get(gui::FontWeight::Bold, 28));
			m_FileExt->SetPosition({ xPos + 35.0f, yPos + dif });
		});

	m_Title->SetOnOnLoseCallback([&]() 
		{

			std::string oldPath = m_File->PathWithoutName() + m_File->NameWithoutExtension() + m_File->extension;
			std::string newPath = m_File->PathWithoutName() + m_Title->string + m_File->extension;

			if (oldPath != newPath)
			{
				std::filesystem::rename(oldPath, newPath);

				m_File->name = m_Title->string;
				m_File->path = m_File->PathWithoutName() + m_File->NameWithoutExtension() + m_File->extension;

				parent->RefreshGUI();
			}

			
		});
	

	//float yPos = titleFont->GetPixelSize();
	//m_Title->SetPosition({ padding, yPos });

	m_FileExt = m_Panel->NewChild<gui::Text>();
	m_FileExt->SetFont(m_FontManager->Get(gui::FontWeight::Italic, 22));
	m_FileExt->SetString(m_File->extension);
	m_FileExt->SetColour({ 0.08f, 0.08f ,0.08f , 1.0f });

	float dif = m_FontManager->Get(gui::FontWeight::Bold, 28)->GetMaxHeight();// -m_FontManager->Get(gui::FontWeight::Italic, 22)->GetAscent();
	float xPos = gui::GetTextLength(m_Title->string, m_FontManager->Get(gui::FontWeight::Bold, 28));
	m_FileExt->SetPosition({ xPos + 35.0f, yPos + dif });

	
	m_TextBox = m_Panel->NewChild<gui::TextBox>();

	m_SavedCircle = m_Panel->NewChild<gui::Panel>();
	float saveYPos = (float)m_FontManager->Get(gui::FontWeight::Bold, 28)->GetPixelSize();
	saveYPos += (float)m_FontManager->Get(gui::FontWeight::Bold, 28)->GetAscent() / 2.0f;
	m_SavedCircle->SetBounds({ padding - 20.0f,saveYPos , 10.0f, 10.0f });
	m_SavedCircle->SetRounding(5.0f);
	m_SavedCircle->SetColour({ 1.0f, 0.3f, 0.2f, 1.0f });
	m_SavedCircle->SetVisible(false);

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