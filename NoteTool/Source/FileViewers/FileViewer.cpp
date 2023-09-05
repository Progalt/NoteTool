
#include "FileViewer.h"
#include "../WorkspaceUI.h"

void FileViewer::SetupTitleAndExt()
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
			m_FileExt->SetPosition({ xPos + 35.0f, dif });
		});

	m_Title->SetOnLoseCallback([&]()
		{
			// TODO: This doesn't work
			//printf("On Lose callback\n");
			if (!m_Title->string.empty())
			{

				std::string oldPath = m_File->PathWithoutName() + m_File->NameWithoutExtension() + m_File->extension;
				std::string newPath = m_File->PathWithoutName() + m_Title->string + m_File->extension;

				if (oldPath != newPath)
				{
					std::filesystem::rename(oldPath, newPath);

					m_File->name = m_Title->string;
					m_File->path = m_File->PathWithoutName() + m_File->NameWithoutExtension() + m_File->extension;

					if (parent)
						parent->RefreshGUI();
				}
			}


		});


	//float yPos = titleFont->GetPixelSize();
	//m_Title->SetPosition({ padding, yPos });

	m_FileExt = m_Panel->NewChild<gui::Text>();
	m_FileExt->SetFont(m_FontManager->Get(gui::FontWeight::Italic, 22));
	m_FileExt->SetString(m_File->extension);
	m_FileExt->SetColour(theme->fileExt);

	float dif = m_FontManager->Get(gui::FontWeight::Bold, 28)->GetMaxHeight();// -m_FontManager->Get(gui::FontWeight::Italic, 22)->GetAscent();
	float xPos = gui::GetTextLength(m_Title->string, m_FontManager->Get(gui::FontWeight::Bold, 28));
	m_FileExt->SetPosition({ xPos + 35.0f, yPos + dif });



	m_SavedCircle = m_Panel->NewChild<gui::Panel>();
	float saveYPos = (float)m_FontManager->Get(gui::FontWeight::Bold, 28)->GetPixelSize();
	saveYPos += (float)m_FontManager->Get(gui::FontWeight::Bold, 28)->GetAscent() / 2.0f;
	m_SavedCircle->SetBounds({ padding - 20.0f,saveYPos , 10.0f, 10.0f });
	m_SavedCircle->SetRounding(5.0f);
	m_SavedCircle->SetColour({ 1.0f, 0.3f, 0.2f, 1.0f });
	m_SavedCircle->SetVisible(false);


}