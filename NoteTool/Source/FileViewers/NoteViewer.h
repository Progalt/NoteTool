#pragma once

#include "FileViewer.h"
#include "NoteFile.h"

static const std::vector<std::pair<std::string, NoteElementType>> Commands = {
	{ "div", NoteElementType::Dividor },
	{ "paragraph", NoteElementType::Paragraph },
	{ "h1", NoteElementType::Header1 },
	{ "h2", NoteElementType::Header2 },
	{ "bullet", NoteElementType::BulletPoint },
	{ "quote", NoteElementType::Quote },
	{ "code", NoteElementType::CodeBlock }
};

static std::unordered_map<std::string, Colour> QuoteBoxColours = {
	{ "note", { 0.2f, 0.2f, 0.2f, 1.0f } },
	{ "quote", { 0.2f, 0.2f, 0.2f, 1.0f } },
	{ "warning", { 161, 112, 8, 100 } },
	{ "success", { 58, 138, 45, 100 } },
	{ "done", { 58, 138, 45, 100 } },
	{ "error", { 138, 45, 45, 100 } },
	{ "bug", { 138, 45, 45, 100 } },
	{ "danger", { 138, 45, 45, 100 } },
	{ "info", { 45, 80, 166, 100 } },
	{ "hint", { 45, 80, 166, 100 } },
	{ "todo", { 45, 80, 166, 100 } }
};

class NoteViewer : public FileViewer
{
public:

	~NoteViewer();

	void Hide() override
	{
		m_Title->SetVisible(false);
		if (m_ShowExtension)
			m_FileExt->SetVisible(false);
		m_SavedCircle->SetVisible(false);

		NoteElement* element = m_NoteFile.Start();

		while (element != nullptr)
		{
			((gui::Widget*)element->userData)->SetVisible(false);

			element = element->next;
		}
	}

	void Show() override
	{
		m_Title->SetVisible(true);
		if (m_ShowExtension)
			m_FileExt->SetVisible(true);

		if (!m_Saved)
			m_SavedCircle->SetVisible(true);

		m_Panel->SetScrollable(true);

		NoteElement* element = m_NoteFile.Start();

		while (element != nullptr)
		{
			((gui::Widget*)element->userData)->SetVisible(true);

			element = element->next;
		}
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

	uint32_t m_BaseTextSize = 16;
	uint32_t m_SubheaderSize = 24;
	uint32_t m_HeaderSize = 32;
};