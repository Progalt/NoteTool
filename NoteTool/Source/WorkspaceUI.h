#pragma once
#include "Workspace.h"
#include "UI/Panel.h"
#include "UI/List.h"
#include "UI/TextBox.h"
#include "FileViewers/PlainTextViewer.h"
#include "FileViewers/MarkdownViewer.h"
#include "Theme.h"

struct WorkspaceTab
{
	std::string tabName;
	bool saved;
	FileViewer* currentFileView;
	gui::Panel* panel;
	FloatRect bounds;
};

// UI to show directory tree
class WorkspaceUI
{
public:

	~WorkspaceUI()
	{
		for (auto& viewer : m_Viewers)
			delete viewer;

		for (auto& tab : m_Tabs)
		{
			delete tab;
		}
	}

	void Init(gui::Panel* panel, Workspace* workspace, Font* font)
	{
		this->m_Panel = panel;
		m_Workspace = workspace;
		m_Font = font;
		m_Panel->SetAnchor(gui::Anchor::CentreLeft);

		//m_Tabs.push_back(WorkspaceTab());
		//m_ActiveTab = &m_Tabs[m_Tabs.size() - 1];

	

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

	void SetTheme(Theme* theme) { m_Theme = theme; }

private:

	WorkspaceTab* NewTab()
	{
		WorkspaceTab* tab = new WorkspaceTab();
		m_Tabs.push_back(tab);
		return tab;
	}

	void SetupGUI()
	{
		m_FileList = m_Panel->NewChild<gui::List>();

		m_Panel->SetAnchor(gui::Anchor::BottomLeft);
		m_Panel->SetLockPosition(true);

	

	

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


		FloatRect startBounds = { 0.0f, 0.0f, m_TextArea->GetBounds().w, m_TextArea->GetBounds().h };
		WorkspaceTab* tab1 = NewTab();
		OpenNewTab(tab1, startBounds);

	

		m_ActiveTab = tab1;

		RefreshGUI();

		m_FileList->SetFont(m_Font);
		m_FileList->SetBounds({ 5.0f, 30.0f, m_Panel->GetBounds().w - 5.0f, m_Panel->GetBounds().h - 30.0f });
		m_FileList->SetHoveredColour({ 0.05f, 0.05f, 0.05f, 1.0f });
		m_FileList->SetTextColour({ 0.65f, 0.65f, 0.65f, 1.0f });
		
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

					if (!m_ActiveTab)
						return;

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
							viewer->theme = m_Theme;
							viewer->SetFontManager(m_FontManager);
							viewer->SetParentPanel(m_ActiveTab->panel);
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
							viewer->theme = m_Theme;
							viewer->SetFontManager(m_FontManager);
							viewer->SetParentPanel(m_ActiveTab->panel);
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

	void OpenNewTab(WorkspaceTab* tab, FloatRect bounds)
	{
		tab->bounds = bounds;
		tab->panel = m_TextArea->NewChild<gui::Panel>();
		m_TextArea->InheritTheme(tab->panel);
		tab->panel->SetBounds(bounds);
		tab->panel->SetAnchor(gui::Anchor::BottomRight);
		//tab->panel->SetLockPosition(true);
		//tab->panel->SetLockFlags(gui::ResizeLock::LockPositionY);

		gui::ContextMenu* menu = tab->panel->GetContextMenu();
		menu->SetFont(m_FontManager->Get(gui::FontWeight::Regular, 12));
		menu->SetColour(m_Theme->panelBackground);
		menu->SetBorderColour(m_Theme->panelHighlight);
		menu->SetPosition({ 100.0f, 100.0f });
		menu->SetRounding(m_Theme->buttonRounding);

		menu->AddOption("Split Vertically", [&]()
			{
				float padding = 3.0f;
				m_ActiveTab->bounds.w /= 2.0f;
				m_ActiveTab->bounds.w -= padding;
				m_ActiveTab->panel->SetBounds(m_ActiveTab->bounds);
				m_ActiveTab->panel->RecalculateAllBounds();
				m_ActiveTab->panel->SetAnchor(gui::Anchor::BottomRight);
				//m_ActiveTab->panel->SetLockFlags(gui::ResizeLock::LockPositionY);
				

				FloatRect newBounds = m_ActiveTab->bounds;
				newBounds.x += m_ActiveTab->bounds.w + padding * 2.0f;
				
				WorkspaceTab* t = NewTab();
				OpenNewTab(t, newBounds);
			});
		

		tab->panel->SetOnFocusCallback([&](void* userData) 
			{
				printf("Changing active tab\n");
				m_ActiveTab = (WorkspaceTab*)userData;

			}, tab);
	}


	Workspace* m_Workspace;
	gui::Panel* m_Panel;

	gui::List* m_FileList;
	gui::Text* m_WorkspaceName;
	Font* m_Font;

	File* m_ActiveFile;

	Theme* m_Theme;


	gui::Panel* m_TextArea;
	gui::FontManager* m_FontManager;
	gui::FontManager* m_CodeFontManager;

	gui::Text* m_NothingOpenText;

	//FileViewer* m_CurrentView;
	std::vector<FileViewer*> m_Viewers;

	WorkspaceTab* m_ActiveTab;
	std::vector<WorkspaceTab*> m_Tabs;


};