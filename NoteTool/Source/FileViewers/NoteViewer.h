#pragma once

#include "FileViewer.h"
#include "NoteFile.h"


class NoteViewer : public FileViewer
{
public:

	void Hide() override
	{
		m_Title->SetVisible(false);
		m_FileExt->SetVisible(false);
		m_SavedCircle->SetVisible(false);
	}

	void Show() override
	{
		m_Title->SetVisible(true);
		m_FileExt->SetVisible(true);

		if (!m_Saved)
			m_SavedCircle->SetVisible(true);

		m_Panel->SetScrollable(true);

	}


	void Save() override;

private:

	NoteFile m_NoteFile;

	void LoadFileContents() override;

	void InitialiseGUIElements() override;

	void ReformatGUI();

	void InsertNewParagraphElement(NoteElement* parent, std::string text);
};