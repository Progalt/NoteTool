#pragma once
#include "Workspace.h"
#include "UI/Panel.h"
#include "UI/List.h"
#include "UI/TextBox.h"
#include "FileViewers/PlainTextViewer.h"
#include "FileViewers/MarkdownViewer.h"
#include "FileViewers/NoteViewer.h"
#include "Theme.h"
#include "UI/ButtonList.h"
#include "Allocator/LinearAllocator.h"
#include "OS.h"

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
		//m_AllocatedDataForFileSystem.clear();

		m_UserDataForFilesysten.FreeAll();

		m_ButtonList->Clear();

		gui::ButtonList::Collection& pins = m_ButtonList->NewCollection("Pins");
		pins.paddingGap = 32.0f;
		pins.iconOverride = IconType::Pin;
		pins.renderDivLine = true;

		// All files within the root of the workspace get added to the general
		AddDirectoryToList(&m_Workspace->GetRoot(), "General", true, m_ButtonList);

		for (auto& dir : m_Workspace->GetRoot().subdirectories)
		{
			AddDirectoryToList(&dir, dir.name, false, m_ButtonList);
		}


		// Add pins
	
		for (auto& workspacePin : m_Workspace->pins)
		{
			// If it equals nullptr we want to remove it from the list
			if (workspacePin == nullptr)
			{


				
			}

			printf("Pin %s\n", workspacePin->name.c_str());
			printf("Path: %s\n", m_Workspace->GetRoot().GetDirectoryOfFile(workspacePin)->path.generic_string().c_str());

			
			FileUserData* userData = m_UserDataForFilesysten.Allocate();
			Directory* dir = m_Workspace->GetRoot().GetDirectoryOfFile(workspacePin);
			userData->dir = dir;
			userData->dirIndex = dir->GetFileIndex(workspacePin);
			userData->workspace = m_Workspace;
			userData->file = &dir->GetFile(userData->dirIndex);

			gui::ButtonList::Button& pin = pins.AddButton(workspacePin->NameWithoutExtension(), m_OpenFileCallback, userData);
			pin.AddSideButton(IconType::Cross, [&](void* userData)
				{
					FileUserData* data = (FileUserData*)userData;

					File* file = &data->dir->GetFile(data->dirIndex);

					for (uint32_t i = 0; i < data->workspace->pins.size(); i++)
					{
						if (data->workspace->pins[i] == file)
						{
							data->workspace->pins.erase(data->workspace->pins.begin() + i);
							RefreshGUI();
						}
					}

				}, userData);
		}

	}

	std::string GetSelectedPath()
	{
		// TODO: Implement

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

		m_Panel->SetAnchor(gui::Anchor::BottomLeft);
		m_Panel->SetLockPosition(true);

	

	

		m_WorkspaceName = m_Panel->NewChild<gui::Text>();
		m_WorkspaceName->SetString(m_Workspace->GetRoot().name);
		m_WorkspaceName->SetFont(m_FontManager->Get(gui::FontWeight::Bold, 16));
		m_WorkspaceName->SetPosition({ 8.0f, 12.0f });
		m_WorkspaceName->SetAnchor(gui::Anchor::TopLeft);
			 
		m_TextArea->SetAnchor(gui::Anchor::BottomRight);
		m_TextArea->SetLockPosition(true);

	

		FloatRect startBounds = { 0.0f, 0.0f, m_TextArea->GetBounds().w, m_TextArea->GetBounds().h };
		WorkspaceTab* tab1 = NewTab();
		OpenNewTab(tab1, startBounds);
		 
	

		m_ActiveTab = tab1;

		 

		m_ButtonList = m_Panel->NewChild<gui::ButtonList>();

		m_ButtonList->SetFont(m_FontManager->Get(gui::FontWeight::Regular, 14));
		m_ButtonList->SetRounding(m_Theme->buttonRounding);
		m_ButtonList->SetBounds({ 4.0f, 72.0f, m_Panel->GetBounds().w - 4.0f * 2.0f, 400.0f });
		m_ButtonList->SetHoveredColour(m_Theme->accentColour);

	
		// wtf? 
		// for some reason calling this here causes a crash due to pins being unreadable but then immediately refreshing GUI after the function call fixes it all
		// What? How? Why? Am I going mad. 
		// TODO: Figure out why this happens? 
		//RefreshGUI();

		// Lets call this here to fix the pin issue for now
		m_ButtonList->Clear();

		gui::ButtonList::Collection& pins = m_ButtonList->NewCollection("Pins");
		pins.paddingGap = 32.0f;
		pins.iconOverride = IconType::Pin;
		pins.renderDivLine = true;

		// All files within the root of the workspace get added to the general
		AddDirectoryToList(&m_Workspace->GetRoot(), "General", true, m_ButtonList);

		for (auto& dir : m_Workspace->GetRoot().subdirectories)
		{
			AddDirectoryToList(&dir, dir.name, false, m_ButtonList);
		}
	}

	void AddDirectoryToList(Directory* dir, const std::string& name,  bool root, gui::ButtonList* buttonList)
	{
		
		FileUserData* createUserData = m_UserDataForFilesysten.Allocate();
		createUserData->dir = dir;
		createUserData->dirIndex = 0;
		createUserData->workspace = m_Workspace;

		gui::ButtonList::Collection& collection = buttonList->NewCollection(name);
		collection.AddSideButton(IconType::Plus, [&](void* userData) {

			FileUserData* data = (FileUserData*)userData;

			std::string path = data->dir->path.generic_string();
			path += "/New Note";
			CreateNewFile(path, ".json");

			NoteFile file;
			NoteElement* base = file.Insert(nullptr);
			base->type = NoteElementType::Paragraph;
			base->paragraph.text = "";
			file.WriteToJSON(path + ".json");

			Refresh();

			}, createUserData);

		for (size_t i = 0; i < dir->FileCount(); i++)
		{
			File& fileRef = dir->GetFile(i);

			// TODO: This is not the best 


			//m_AllocatedDataForFileSystem.push_back(FileUserData());

			//FileUserData* userData = &m_AllocatedDataForFileSystem[m_AllocatedDataForFileSystem.size() - 1];
			// Grab some allocated data from the linear allocator
			FileUserData* userData = m_UserDataForFilesysten.Allocate();
			userData->dir = dir;
			userData->dirIndex = i;
			userData->file = &fileRef;


			
			gui::ButtonList::Button& button = collection.AddButton(fileRef.NameWithoutExtension(), m_OpenFileCallback, userData);

			gui::ButtonList::SideButton& bin = button.AddSideButton(IconType::Bin, [&](void* userData) {

				FileUserData* data = (FileUserData*)userData;

				File* file = data->file;

				FileViewer* openViewer = GetViewer(file);

				// File is open in a viewer somewhere
				if (openViewer)
				{
					openViewer->Hide();


					
				}

				std::filesystem::path path = file->path;

				// TODO: Prompt this so we don't have miss clicks. 
				// TODO: We probably also want to move to recycle bin. Not just straight up delete it

				if (std::filesystem::exists(path))
				{
					std::filesystem::remove(path);
					printf("Deleting file: %s\n", path.generic_string().c_str());
				}
				
				Refresh();

				}, userData);

			bin.hoverColour = { 1.0f, 0.5f, 0.5f, 1.0f };

			button.AddSideButton(IconType::Pin, [&](void* userData) {

				FileUserData* data = (FileUserData*)userData;

				if (data->dir != nullptr)
				{

					File* file = &data->dir->GetFile(data->dirIndex);

					bool found = false;

					for (auto& pin : m_Workspace->pins)
						if (pin == file)
							found = true;

					if (!found)
					{
						m_Workspace->pins.push_back({ &data->dir->GetFile(data->dirIndex) });
						RefreshGUI();
					}
				}

				}, userData);

			
		}

		

	}
	
	void OpenNewTab(WorkspaceTab* tab, FloatRect bounds)
	{
		// TODO: Tab Scaling is off
		// Use percentage resize scaling? 

		tab->bounds = bounds;
		tab->panel = m_TextArea->NewChild<gui::Panel>();
		m_TextArea->InheritTheme(tab->panel);
		tab->panel->SetBounds(bounds);
		tab->panel->SetAnchor(gui::Anchor::BottomRight);
		tab->panel->SetDrawVerticalScrollBar(true);
		tab->panel->SetHighlightColour(m_Theme->panelHighlight);
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

	gui::Text* m_WorkspaceName;
	Font* m_Font;

	File* m_ActiveFile;

	Theme* m_Theme;

	gui::ButtonList* m_ButtonList;


	gui::Panel* m_TextArea;
	gui::FontManager* m_FontManager;
	gui::FontManager* m_CodeFontManager;


	//FileViewer* m_CurrentView;
	std::vector<FileViewer*> m_Viewers;

	WorkspaceTab* m_ActiveTab;
	std::vector<WorkspaceTab*> m_Tabs;

	FileViewer* GetViewer(File* file)
	{
		FileViewer* v = nullptr;

		for (auto& viewer : m_Viewers)
			if (*viewer->GetFile() == *file)
				v = viewer;

		return v;

	}


	struct FileUserData
	{
		Directory* dir;
		uint32_t dirIndex;
		Workspace* workspace;
		File* file;
	};

	//std::vector<FileUserData> m_AllocatedDataForFileSystem;

	LinearAllocator<FileUserData> m_UserDataForFilesysten;

	std::function<void(void*)> m_OpenFileCallback = [&](void* userData)
		{
			FileUserData* data = (FileUserData*)userData;

			File* file = data->file;

			if (!m_ActiveTab)
				return;

			
			if (m_ActiveTab->currentFileView)
			{
				printf("Destroying tab\n");
				
				m_ActiveTab->currentFileView->Hide();

				m_ActiveTab->panel->RemoveChildren();
				delete m_ActiveTab->currentFileView;
			}

			bool found = false;

			FileViewer* viewer = GetViewer(file);

	
		/*	if (viewer != nullptr)
			{

				m_ActiveTab->currentFileView = viewer;
				m_ActiveTab->currentFileView->Show();
				m_ActiveTab->panel->SetCurrentScrollDistance(0.0f);
				gui::EventHandler::cursorOffset = 0;

				found = true;


			}*/


			

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

					//m_Viewers.push_back(viewer);

					m_ActiveTab->currentFileView = viewer;

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


					//m_Viewers.push_back(viewer);

					m_ActiveTab->currentFileView = viewer;
				}

				if (file->type == FileType::Note)
				{
					NoteViewer* viewer = new NoteViewer;
					viewer->theme = m_Theme;
					viewer->SetFontManager(m_FontManager);
					viewer->SetParentPanel(m_ActiveTab->panel);
					viewer->SetCodeFontManager(m_CodeFontManager);
					viewer->SetFile(file);
					viewer->parent = this;


					//m_Viewers.push_back(viewer);

					m_ActiveTab->currentFileView = viewer;
				}
			}




		};


};