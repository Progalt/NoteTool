
#include "NoteViewer.h"
#include "../UI/TextBoxSimplified.h"

void NoteViewer::LoadFileContents()
{

	m_NoteFile.LoadFromJSON(m_File->path);

	m_NoteFile.Print();
}

void NoteViewer::ReformatGUI()
{
	m_SavedCircle->SetVisible(true);

	NoteElement* element = m_NoteFile.Start();

	float yOffset = 98.0f;

	float elementEndPadding = 16.0f;
	float margin = 32.0f;

	while (element != nullptr)
	{
		switch (element->type)
		{
		case NoteElementType::Header1:
		{
			gui::TextBoxSimplified* text = (gui::TextBoxSimplified*)element->userData;

			float fontSize = m_FontManager->Get(gui::FontWeight::Bold, 32)->GetMaxHeight();
			text->SetFontManager(m_FontManager);
			text->SetBounds({ margin, yOffset, m_Panel->GetBounds().w - margin * 2.0f, fontSize + 2.0f });

			yOffset += text->GetTextBoxHeight() + elementEndPadding;

			element->userData = text;
		}
		break;
		case NoteElementType::Header2:
		{
			gui::TextBoxSimplified* text = (gui::TextBoxSimplified*)element->userData;

			float fontSize = m_FontManager->Get(gui::FontWeight::Bold, 24)->GetMaxHeight();
			text->SetFontManager(m_FontManager);
			text->SetBounds({ margin, yOffset, m_Panel->GetBounds().w - margin * 2.0f, fontSize + 2.0f });

			yOffset += text->GetTextBoxHeight() + elementEndPadding;
		}
		break;
		case NoteElementType::Paragraph:
		{
			gui::TextBoxSimplified* text = (gui::TextBoxSimplified*)element->userData;

			float fontSize = m_FontManager->Get(gui::FontWeight::Light, 14)->GetMaxHeight();
			text->SetFontManager(m_FontManager);
			text->SetBounds({ margin, yOffset, m_Panel->GetBounds().w - margin * 2.0f, fontSize + 2.0f });

			yOffset += text->GetTextBoxHeight() + elementEndPadding;

			element->userData = text;
		}
		break;
		}

		element = element->next;
	}

	m_Panel->SetScrollable(true);
	m_Panel->SetScrollableArea(Vector2f(0.0f, yOffset));
}

void NoteViewer::InitialiseGUIElements()
{

	SetupTitleAndExt();

	NoteElement* element = m_NoteFile.Start();

	float yOffset = 98.0f;

	float elementEndPadding = 16.0f;
	float margin = 48.0f;

	while (element != nullptr)
	{


		switch (element->type)
		{
		case NoteElementType::Header1:
		{
			gui::TextBoxSimplified* text = m_Panel->NewChild<gui::TextBoxSimplified>();

			float fontSize = m_FontManager->Get(gui::FontWeight::Bold, 32)->GetMaxHeight();
			text->SetFontManager(m_FontManager);
			text->SetBounds({ margin, yOffset, m_Panel->GetBounds().w - margin * 2.0f, fontSize + 2.0f });
			text->SetFontSize(32, gui::FontWeight::Bold);
			text->string = element->header.text;

			text->SetOnEditCallback([&](void* userData) {
				ReformatGUI();
				});

			yOffset += text->GetBounds().h + elementEndPadding;
			
			element->userData = text;
		}
			break;
		case NoteElementType::Header2:
		{
			gui::TextBoxSimplified* text = m_Panel->NewChild<gui::TextBoxSimplified>();

			float fontSize = m_FontManager->Get(gui::FontWeight::Bold, 24)->GetMaxHeight();
			text->SetFontManager(m_FontManager);
			text->SetBounds({ margin, yOffset, m_Panel->GetBounds().w - margin * 2.0f, fontSize + 2.0f });
			text->SetFontSize(24, gui::FontWeight::Bold);
			text->string = element->header.text;

			text->SetOnEditCallback([&](void* userData) {
				ReformatGUI();
				});

			yOffset += text->GetBounds().h + elementEndPadding;

			element->userData = text;
		}
			break;
		case NoteElementType::Paragraph:
		{
			gui::TextBoxSimplified* text = m_Panel->NewChild<gui::TextBoxSimplified>();

			float fontSize = m_FontManager->Get(gui::FontWeight::Light, 14)->GetMaxHeight();
			text->SetFontManager(m_FontManager);
			text->SetBounds({ margin, yOffset, m_Panel->GetBounds().w - margin * 2.0f, fontSize + 2.0f });
			text->SetFontSize(16, gui::FontWeight::Light);
			text->string = element->paragraph.text;

			text->SetOnEditCallback([&](void* userData) {

				ReformatGUI();
				}, element);

			yOffset += text->GetBounds().h + elementEndPadding;

			element->userData = text;
		}
			break;
		}



		element = element->next;
	}

}

void NoteViewer::Save()
{
	NoteElement* element = m_NoteFile.Start();

	while (element != nullptr)
	{
		switch (element->type)
		{
		case NoteElementType::Header1:
			element->header.text = ((gui::TextBoxSimplified*)element->userData)->string;
			break;
		case NoteElementType::Header2:
			element->header.text = ((gui::TextBoxSimplified*)element->userData)->string;
			break;
		case NoteElementType::Paragraph:
			element->paragraph.text = ((gui::TextBoxSimplified*)element->userData)->string;
			break;
		};

		element = element->next;
	}

	m_Saved = true;

	m_SavedCircle->SetVisible(false);

	m_NoteFile.WriteToJSON(m_File->path);
}

