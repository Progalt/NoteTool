#pragma once
#include "Workspace.h"
#include "UI/Panel.h"
#include "UI/List.h"
#include "UI/TextBox.h"
#include "FileViewers/PlainTextViewer.h"
#include "FileViewers/MarkdownViewer.h"

struct WorkspaceTab
{
	std::string tabName;
	bool saved;
	FileViewer* currentFileView;
};

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
		m_Panel->SetAnchor(gui::Anchor::CentreLeft);

		m_Tabs.push_back(WorkspaceTab());
		m_ActiveTab = &m_Tabs[m_Tabs.size() - 1];

		SetupGUI();
	}


	void SetTextArea(gui::Panel* panel) { m_TextArea = panel; }

	void SetFontManager(gui::FontManager* fontManager) { m_FontManager = fontManager; }

	void SetCodeFontManager(gui::FontManager* codeManager) { m_CodeFontManager = codeManager; }
	
	void TriggerSave() 
	{ 
		if (m_ActiveTab->currentFileView)
		{
			m_ActiveTab->currentFileView->Save();
		}
	}

	void Refresh()
	{
		m_Workspace->Refresh();
		RefreshGUI();
	}

	void RefreshGUI()
	{
		m_FileList->Clear();

		Directory& root = m_Workspace->GetRoot();

		//gui::ListEntry entry(m_Workspace->GetRoot().name, NULL);

		AddDirectoryToList(root, nullptr);


	}

	std::string GetSelectedPath()
	{
		gui::ListEntry* sel = m_FileList->GetSelected();

		if (sel)
		{
			if (sel->directory)
			{
				Directory* dir = (Directory*)sel->userData;
				assert(dir);
				return dir->path.generic_string();
			}
			
		}

		return "";
	}

	void CloseCurrentTab()
	{
		m_ActiveTab->currentFileView->Hide();
		m_ActiveTab->currentFileView = nullptr;

		
	}

	FileViewer* GetCurrentFileViewer()
	{
		return m_ActiveTab->currentFileView;
	}

private:

	void SetupGUI()
	{
		m_FileList = m_Panel->NewChild<gui::List>();

		m_Panel->SetAnchor(gui::Anchor::BottomLeft);
		m_Panel->SetLockPosition(true);

		RefreshGUI();

		m_FileList->SetFont(m_Font);
		m_FileList->SetBounds({ 5.0f, 30.0f, m_Panel->GetBounds().w - 5.0f, m_Panel->GetBounds().h - 30.0f });
		m_FileList->SetHoveredColour({ 0.05f, 0.05f, 0.05f, 1.0f });
		m_FileList->SetTextColour({ 0.65f, 0.65f, 0.65f, 1.0f });

		m_WorkspaceName = m_Panel->NewChild<gui::Text>();
		m_WorkspaceName->SetString(m_Workspace->GetRoot().name);
		m_WorkspaceName->SetFont(m_FontManager->Get(gui::FontWeight::Bold, 14));
		m_WorkspaceName->SetPosition({ 24.0f, 64.0f });
		m_WorkspaceName->SetAnchor(gui::Anchor::TopLeft);
			 
		m_TextArea->SetAnchor(gui::Anchor::BottomRight);
		m_TextArea->SetLockPosition(true);

		m_NothingOpenText = m_TextArea->NewChild<gui::Text>();
		m_NothingOpenText->SetString("No file is open");
		m_NothingOpenText->SetFont(m_FontManager->Get(gui::FontWeight::Bold, 24));
		m_NothingOpenText->SetBounds({ m_TextArea->GetBounds().w / 2.0f - gui::GetTextLength(m_NothingOpenText->GetString(), m_FontManager->Get(gui::FontWeight::Bold, 24)), m_TextArea->GetBounds().h / 2.0f, 300.0f, 50.0f });
		m_NothingOpenText->SetAnchor(gui::Anchor::Centre);
		
	}

	

	void AddDirectoryToList(Directory& dir, gui::ListEntry* entry)
	{

		
		
		for (size_t i = 0; i < dir.DirectoryCount(); i++)
		{
			Directory& directory = dir.GetDirectory(i);

			gui::ListEntry* dirEntry = m_FileList->NewListEntry(directory.name, NULL, &directory, true);
			
			AddDirectoryToList(dir.GetDirectory(i), dirEntry);

			if (entry)
				entry->AddChild(dirEntry);
			else
				m_FileList->AddEntry(dirEntry);
		}

		for (size_t i = 0; i < dir.FileCount(); i++)
		{
			File& file = dir.GetFile(i);

			gui::ListEntry* fileEntry = m_FileList->NewListEntry(file.isCodeFile ? file.name : file.NameWithoutExtension(), [&](void* userData)
				{
					File* file = (File*)userData;

					if (m_ActiveTab->currentFileView)
						m_ActiveTab->currentFileView->Hide();

					bool found = false;

					for (auto& viewer : m_Viewers)
					{
						if (*viewer->GetFile() == *file)
						{
							m_ActiveTab->currentFileView = viewer;
							m_ActiveTab->currentFileView->Show();
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
							viewer->SetCodeFontManager(m_CodeFontManager);
							viewer->SetFile(file);
							viewer->parent = this;

							m_Viewers.push_back(viewer);

							m_ActiveTab->currentFileView = viewer;

							m_NothingOpenText->SetVisible(false);
						}

						if (file->type == FileType::Markdown)
						{
							MarkdownViewer* viewer = new MarkdownViewer;
							viewer->SetFontManager(m_FontManager);
							viewer->SetParentPanel(m_TextArea);
							viewer->SetCodeFontManager(m_CodeFontManager);
							viewer->SetFile(file);
							viewer->parent = this;

							m_Viewers.push_back(viewer);

							m_ActiveTab->currentFileView = viewer;

							m_NothingOpenText->SetVisible(false);
						}
					}


				}, &file, false);
			
			if (entry)
				entry->AddChild(fileEntry);
			else
				m_FileList->AddEntry(fileEntry);
		}



		

	}


	Workspace* m_Workspace;
	gui::Panel* m_Panel;

	gui::List* m_FileList;
	gui::Text* m_WorkspaceName;
	Font* m_Font;

	File* m_ActiveFile;


	gui::Panel* m_TextArea;
	gui::FontManager* m_FontManager;
	gui::FontManager* m_CodeFontManager;

	gui::Text* m_NothingOpenText;

	//FileViewer* m_CurrentView;
	std::vector<FileViewer*> m_Viewers;

	WorkspaceTab* m_ActiveTab;
	std::vector<WorkspaceTab> m_Tabs;

	struct
	{
		std::vector<gui::Button*> activeButtons;
		gui::Panel* panel;

	} m_TabArea;

};