#pragma once

#include "FileViewer.h"
#include "NoteFile.h"

static const std::vector<std::pair<std::string, NoteElementType>> Commands = {
	{ "div", NoteElementType::Dividor },
	{ "paragraph", NoteElementType::Paragraph },
	{ "h1", NoteElementType::Header1 },
	{ "h2", NoteElementType::Header2 },
	{ "bullet", NoteElementType::BulletPoint },
	{ "quote", NoteElementType::Quote } 
};

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

	float m_Margin = 64.0f;

	NoteFile m_NoteFile;

	void LoadFileContents() override;

	void InitialiseGUIElements() override;

	void ReformatGUI();

	NoteElement* InsertNewElement(NoteElement* parent, NoteElementType type);

	void NewParagraphOnNewlines(NoteElement* base);
	void NewDividorOnNewLines(NoteElement* base);

	void Command(NoteElement* base);

	void OnEdit(NoteElement* base);

	void RemoveElement(NoteElement* element);

	NoteElement* FindParent(NoteElement* element);

	void DeleteOnBackspace(NoteElement* element);
	void EnterDuplicateElement(NoteElement* element);

	void InitialiseGUIElement(NoteElement* element);
};