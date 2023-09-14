
#include "NoteViewer.h"
#include "../UI/TextBoxSimplified.h"
#include "../UI/Shape.h"

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

	while (element != nullptr)
	{
		switch (element->type)
		{
		case NoteElementType::Header1:
		{
			gui::TextBoxSimplified* text = (gui::TextBoxSimplified*)element->userData;

			float fontSize = m_FontManager->Get(gui::FontWeight::Bold, 32)->GetMaxHeight();
			text->SetFontManager(m_FontManager);
			text->SetBounds({ m_Margin, yOffset, m_Panel->GetBounds().w - m_Margin * 2.0f, fontSize + 2.0f });

			element->bounds = text->GetBounds();

			yOffset += text->GetTextBoxHeight() + elementEndPadding;

			element->userData = text;
		}
		break;
		case NoteElementType::Header2:
		{
			gui::TextBoxSimplified* text = (gui::TextBoxSimplified*)element->userData;

			float fontSize = m_FontManager->Get(gui::FontWeight::Bold, 24)->GetMaxHeight();
			text->SetFontManager(m_FontManager);
			text->SetBounds({ m_Margin, yOffset, m_Panel->GetBounds().w - m_Margin * 2.0f, fontSize + 2.0f });

			element->bounds = text->GetBounds();

			yOffset += text->GetTextBoxHeight() + elementEndPadding;
		}
		break;
		case NoteElementType::Paragraph:
		{
			gui::TextBoxSimplified* text = (gui::TextBoxSimplified*)element->userData;

			float fontSize = m_FontManager->Get(gui::FontWeight::Light, 14)->GetMaxHeight();
			text->SetFontManager(m_FontManager);
			text->SetBounds({ m_Margin, yOffset, m_Panel->GetBounds().w - m_Margin * 2.0f, fontSize + 2.0f });

			yOffset += text->GetTextBoxHeight() + elementEndPadding;

			element->bounds = text->GetBounds();

			element->userData = text;
		}
		break;
		case NoteElementType::Dividor:
		{
			gui::ShapeWidget* shape = (gui::ShapeWidget*)element->userData;
			shape->SetPosition({ m_Margin, yOffset });

			yOffset += 2.0f + elementEndPadding;

			element->bounds = { m_Margin, yOffset - 4.0f, m_Panel->GetBounds().w - m_Margin * 2.0f, 8.0f };
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

	while (element != nullptr)
	{


		switch (element->type)
		{
		case NoteElementType::Header1:
		{
			gui::TextBoxSimplified* text = m_Panel->NewChild<gui::TextBoxSimplified>();

			float fontSize = m_FontManager->Get(gui::FontWeight::Bold, 32)->GetMaxHeight();
			text->SetFontManager(m_FontManager);
			text->SetBounds({ m_Margin, yOffset, m_Panel->GetBounds().w - m_Margin * 2.0f, fontSize + 2.0f });
			text->SetFontSize(32, gui::FontWeight::Bold);
			text->string = element->header.text;

			text->SetOnEditCallback([&](void* userData) {

				NoteElement* baseElement = (NoteElement*)userData;

				OnEdit(baseElement);

				ReformatGUI();
				}, element);

			yOffset += text->GetBounds().h + elementEndPadding;
			
			element->userData = text;
		}
			break;
		case NoteElementType::Header2:
		{
			gui::TextBoxSimplified* text = m_Panel->NewChild<gui::TextBoxSimplified>();

			float fontSize = m_FontManager->Get(gui::FontWeight::Bold, 24)->GetMaxHeight();
			text->SetFontManager(m_FontManager);
			text->SetBounds({ m_Margin, yOffset, m_Panel->GetBounds().w - m_Margin * 2.0f, fontSize + 2.0f });
			text->SetFontSize(24, gui::FontWeight::Bold);
			text->string = element->header.text;

			text->SetOnEditCallback([&](void* userData) {

				NoteElement* baseElement = (NoteElement*)userData;

				OnEdit(baseElement);

				ReformatGUI();
				}, element);

			yOffset += text->GetBounds().h + elementEndPadding;

			element->userData = text;
		}
			break;
		case NoteElementType::Paragraph:
		{
			gui::TextBoxSimplified* text = m_Panel->NewChild<gui::TextBoxSimplified>();

			float fontSize = m_FontManager->Get(gui::FontWeight::Light, 14)->GetMaxHeight();
			text->SetFontManager(m_FontManager);
			text->SetBounds({ m_Margin, yOffset, m_Panel->GetBounds().w - m_Margin * 2.0f, fontSize + 2.0f });
			text->SetFontSize(16, gui::FontWeight::Light);
			text->string = element->paragraph.text;

			text->SetOnEditCallback([&](void* userData) {

				NoteElement* baseElement = (NoteElement*)userData;

				OnEdit(baseElement);

				ReformatGUI();
				}, element);

			yOffset += text->GetBounds().h + elementEndPadding;

			element->userData = text;
		}
			break;
		case NoteElementType::Dividor:
		{
			gui::ShapeWidget* shape = m_Panel->NewChild<gui::ShapeWidget>();

			shape->SetColour({ 0.4f, 0.4f, 0.4f, 1.0f });
			shape->SetPosition({ m_Margin, yOffset });
			shape->SetAsRectangle({ m_Panel->GetBounds().w - m_Margin * 2.0f, 1.0f });

			element->userData = shape;

			yOffset += 2.0f + elementEndPadding;
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

NoteElement* NoteViewer::InsertNewElement(NoteElement* parent, NoteElementType type)
{
	
	NoteElement* newElement = m_NoteFile.Insert(parent);

	newElement->type = type;

	switch (newElement->type)
	{
	case NoteElementType::Paragraph:
	{
		gui::TextBoxSimplified* text = m_Panel->NewChild<gui::TextBoxSimplified>();

		float fontSize = m_FontManager->Get(gui::FontWeight::Light, 14)->GetMaxHeight();
		text->SetFontManager(m_FontManager);
		text->SetBounds({ m_Margin, 10.0f, m_Panel->GetBounds().w - m_Margin * 2.0f, fontSize + 2.0f });
		text->SetFontSize(16, gui::FontWeight::Light);
		text->string = newElement->paragraph.text;
		newElement->userData = text;

		text->SetOnEditCallback([&](void* userData) {

			NoteElement* baseElement = (NoteElement*)userData;

			OnEdit(baseElement);

			ReformatGUI();
			}, newElement);
	}
		break;
	case NoteElementType::Dividor:
	{		
		gui::ShapeWidget* shape = m_Panel->NewChild<gui::ShapeWidget>();

		shape->SetColour({ 0.4f, 0.4f, 0.4f, 1.0f });
		shape->SetPosition({ m_Margin, 0.0f });
		shape->SetAsRectangle({ m_Panel->GetBounds().w - m_Margin * 2.0f, 1.0f });

		newElement->userData = shape;
	}
		break;
	case NoteElementType::Header1:
	{
		gui::TextBoxSimplified* text = m_Panel->NewChild<gui::TextBoxSimplified>();

		float fontSize = m_FontManager->Get(gui::FontWeight::Bold, 32)->GetMaxHeight();
		text->SetFontManager(m_FontManager);
		text->SetBounds({ m_Margin, 10.0f, m_Panel->GetBounds().w - m_Margin * 2.0f, fontSize + 2.0f });
		text->SetFontSize(32, gui::FontWeight::Bold);
		text->string = newElement->header.text;

		text->SetOnEditCallback([&](void* userData) {

			NoteElement* baseElement = (NoteElement*)userData;

			OnEdit(baseElement);

			ReformatGUI();
			}, newElement);

		

		newElement->userData = text;
	}
		break;
	case NoteElementType::Header2:
	{
		gui::TextBoxSimplified* text = m_Panel->NewChild<gui::TextBoxSimplified>();

		float fontSize = m_FontManager->Get(gui::FontWeight::Bold, 24)->GetMaxHeight();
		text->SetFontManager(m_FontManager);
		text->SetBounds({ m_Margin, 10.0f, m_Panel->GetBounds().w - m_Margin * 2.0f, fontSize + 2.0f });
		text->SetFontSize(24, gui::FontWeight::Bold);
		text->string = newElement->header.text;

		text->SetOnEditCallback([&](void* userData) {

			NoteElement* baseElement = (NoteElement*)userData;

			OnEdit(baseElement);

			ReformatGUI();
			}, newElement);



		newElement->userData = text;
	}
	break;
	}

	return newElement;
}

void NoteViewer::NewParagraphOnNewlines(NoteElement* base)
{
	gui::TextBoxSimplified* textBox = (gui::TextBoxSimplified*)base->userData;

	std::string str = textBox->string;

	size_t pos = str.find("\n\n");


	if (pos != std::string::npos)
	{
		textBox->string.erase(pos, 2);


		NoteElement* newElement = InsertNewElement(base, NoteElementType::Paragraph);
		printf("Created new paragraph element\n");

		textBox->RemoveFocus();

		gui::TextBoxSimplified* newTextBox = (gui::TextBoxSimplified*)newElement->userData;

		newTextBox->TakeFocus();
	}
}

void NoteViewer::NewDividorOnNewLines(NoteElement* base)
{
	gui::TextBoxSimplified* textBox = (gui::TextBoxSimplified*)base->userData;

	std::string str = textBox->string;

	size_t pos = str.find("---");


	if (pos != std::string::npos)
	{
		textBox->string.erase(pos, 3);

		bool insertParagraph = (base->next == nullptr);

		NoteElement* newElement = InsertNewElement(base, NoteElementType::Dividor);
		printf("Created new dividor element\n");

		textBox->RemoveFocus();

		// Also insert a new paragraph after 
		// but only if the current next is nullptr so we have a paragraph afters
		if (insertParagraph)
		{
			NoteElement* afterPara = InsertNewElement(newElement, NoteElementType::Paragraph);

			gui::TextBoxSimplified* newTextBox = (gui::TextBoxSimplified*)afterPara->userData;

			newTextBox->TakeFocus();
		}
	}
}

void NoteViewer::Command(NoteElement* base)
{
	gui::TextBoxSimplified* textBox = (gui::TextBoxSimplified*)base->userData;

	std::string str = textBox->string;

	size_t pos = 0;
	size_t lastpos = 0;

	// we loop because sometimes an element could contain many non-command related slashes 
	while (pos != std::string::npos)
	{
		
		pos = str.find("/", lastpos);
		lastpos = pos + 1;

		if (pos != std::string::npos)
		{
			// We have a slash now we want to see if it matches any of our commands and then a newline

			for (auto& [cmd, type] : Commands)
			{
				// range check
				if (pos + cmd.size() + 1 < str.size())
				{
					if (str.substr(pos + 1, cmd.size()) == cmd)
					{
						// it matches

						bool insertParagraph = (base->next == nullptr);

						NoteElement* newElement = InsertNewElement(base, type);

						textBox->string.erase(pos, cmd.size() + 2);

						// Take focus if the new element was a text based one
						if (type == NoteElementType::Paragraph || type == NoteElementType::Header1
							|| type == NoteElementType::Header2)
						{
							textBox->RemoveFocus();

							gui::TextBoxSimplified* newTextBox = (gui::TextBoxSimplified*)newElement->userData;

							newTextBox->TakeFocus();
						}



						if (type == NoteElementType::Dividor)
						{
							NoteElement* afterPara = InsertNewElement(newElement, NoteElementType::Paragraph);

							textBox->RemoveFocus();

							gui::TextBoxSimplified* newTextBox = (gui::TextBoxSimplified*)afterPara->userData;

							newTextBox->TakeFocus();
						}
					}
				}
			}
		}
	}
}

void NoteViewer::OnEdit(NoteElement* base)
{
	NewParagraphOnNewlines(base);
	NewDividorOnNewLines(base);
	Command(base);
}