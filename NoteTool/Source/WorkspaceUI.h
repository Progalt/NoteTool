#pragma once
#include "Workspace.h"
#include "UI/Panel.h"
#include "UI/List.h"

// UI to show directory tree
class WorkspaceUI
{
public:

	void Init(gui::Panel* panel, Workspace* workspace, Font* font)
	{
		this->m_Panel = panel;
		m_Workspace = workspace;
		m_Font = font;

		SetupGUI();
	}



	

private:

	void SetupGUI()
	{
		m_FileList = m_Panel->NewChild<gui::List>();

		Directory& root = m_Workspace->GetRoot();

		gui::ListEntry entry(m_Workspace->GetRoot().name, NULL);
		
		AddDirectoryToList(root, &entry);


		m_FileList->AddEntry(entry);
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

			entry->AddChild(dirEntry);
		}

		for (size_t i = 0; i < dir.FileCount(); i++)
		{
			File& file = dir.GetFile(i);

			gui::ListEntry fileEntry = gui::ListEntry(file.name, NULL);
			
			entry->AddChild(fileEntry);
		}



		

	}

	Workspace* m_Workspace;
	gui::Panel* m_Panel;

	gui::List* m_FileList;
	Font* m_Font;

	File* m_ActiveFile;
};