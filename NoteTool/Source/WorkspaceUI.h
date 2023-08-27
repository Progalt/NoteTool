#pragma once
#include "Workspace.h"
#include "UI/Panel.h"
#include "UI/List.h"
#include "UI/TextBox.h"
#include "FileViewers/PlainTextViewer.h"

// UI to show directory tree
class WorkspaceUI
{
public:

	~WorkspaceUI()
	{
		for (auto& viewer : m_Viewers)
			delete viewer;
	}

	void Init(gui::Panel* panel, Workspace* workspace, Font* font)
	{
		this->m_Panel = panel;
		m_Workspace = workspace;
		m_Font = font;

		SetupGUI();
	}


	void SetTextArea(gui::Panel* panel) { m_TextArea = panel; }

	void SetFontManager(gui::FontManager* fontManager) { m_FontManager = fontManager; }
	
	void TriggerSave() 
	{ 
		if (m_CurrentView)
		{
			m_CurrentView->Save();
		}
	}

private:

	void SetupGUI()
	{
		m_FileList = m_Panel->NewChild<gui::List>();

		Directory& root = m_Workspace->GetRoot();

		//gui::ListEntry entry(m_Workspace->GetRoot().name, NULL);
		
		AddDirectoryToList(root, nullptr);


		m_FileList->SetFont(m_Font);
		m_FileList->SetBounds({ 0.0f, 0.0f, 240.0f, 400.0f });
		m_FileList->SetHoveredColour({ 0.05f, 0.05f, 0.05f, 1.0f });
		m_FileList->SetTextColour({ 0.65f, 0.65f, 0.65f, 1.0f });

	}

	void AddDirectoryToList(Directory& dir, gui::ListEntry* entry)
	{

		
		
		for (size_t i = 0; i < dir.DirectoryCount(); i++)
		{
			Directory& directory = dir.GetDirectory(i);

			gui::ListEntry dirEntry = gui::ListEntry(directory.name, NULL);
			
			AddDirectoryToList(dir.GetDirectory(i), &dirEntry);

			if (entry)
				entry->AddChild(dirEntry);
			else
				m_FileList->AddEntry(dirEntry);
		}

		for (size_t i = 0; i < dir.FileCount(); i++)
		{
			File& file = dir.GetFile(i);

			gui::ListEntry fileEntry = gui::ListEntry(file.name, [&](void* userData)
				{
					File* file = (File*)userData;

					if (m_CurrentView)
						m_CurrentView->Hide();

					bool found = false;

					for (auto& viewer : m_Viewers)
					{
						if (*viewer->GetFile() == *file)
						{
							m_CurrentView = viewer;
							m_CurrentView->Show();
							gui::EventHandler::cursorOffset = 0;

							found = true;
						}


					}

					if (!found)
					{
						if (file->type == FileType::PlainText)
						{
							PlainTextViewer* viewer = new PlainTextViewer;
							viewer->SetFontManager(m_FontManager);
							viewer->SetParentPanel(m_TextArea);
							viewer->SetFile(file);

							m_Viewers.push_back(viewer);

							m_CurrentView = viewer;
						}
					}

				}, &file);
			
			if (entry)
				entry->AddChild(fileEntry);
			else
				m_FileList->AddEntry(fileEntry);
		}



		

	}


	Workspace* m_Workspace;
	gui::Panel* m_Panel;

	gui::List* m_FileList;
	Font* m_Font;

	File* m_ActiveFile;


	gui::Panel* m_TextArea;
	gui::FontManager* m_FontManager;

	FileViewer* m_CurrentView;
	std::vector<FileViewer*> m_Viewers;


};