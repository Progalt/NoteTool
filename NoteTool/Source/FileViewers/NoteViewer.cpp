
#include "NoteViewer.h"
#include "../UI/TextBoxSimplified.h"
#include "../UI/Shape.h"

NoteViewer::~NoteViewer()
{
	
}

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
		case NoteElementType::Header2:
		{


			gui::TextBoxSimplified* text = (gui::TextBoxSimplified*)element->userData;

			float fontSize = m_FontManager->Get(gui::FontWeight::Bold, (element->type == NoteElementType::Header1) ? 32 : 24)->GetMaxHeight();
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
		case NoteElementType::BulletPoint:
		{
			gui::TextBoxSimplified* text = (gui::TextBoxSimplified*)element->userData;

			float fontSize = m_FontManager->Get(gui::FontWeight::Light, 16)->GetMaxHeight();
			text->SetFontManager(m_FontManager);
			text->SetBounds({ m_Margin + 32.0f, yOffset, m_Panel->GetBounds().w - m_Margin * 2.0f, fontSize + 2.0f });

			float padding = elementEndPadding;
			if (element->next)
				if (element->next->type == NoteElementType::BulletPoint)
					padding = 4.0f;

			yOffset += text->GetTextBoxHeight() + padding;

			element->bounds = text->GetBounds();

			element->userData = text;
		}
		break;
		case NoteElementType::Quote:
		{
			gui::TextBoxSimplified* text = (gui::TextBoxSimplified*)element->userData;

			float fontSize = m_FontManager->Get(gui::FontWeight::Light, 14)->GetMaxHeight();
			yOffset += fontSize * 2.0f;
			text->SetFontManager(m_FontManager);
			text->SetBounds({ m_Margin + 32.0f, yOffset, m_Panel->GetBounds().w - m_Margin * 2.0f, fontSize + 2.0f });

			

			yOffset += text->GetTextBoxHeight() + fontSize * 2.0f;

		}
		break;
		case NoteElementType::CodeBlock:
		{
			gui::TextBoxSimplified* text = (gui::TextBoxSimplified*)element->userData;

			float fontSize = m_CodeFontManager->Get(gui::FontWeight::Regular, 14)->GetMaxHeight();
			yOffset += fontSize * 2.0f;
			//text->SetFontManager(m_CodeFontManager);
			text->SetBounds({ m_Margin + 32.0f, yOffset, m_Panel->GetBounds().w - m_Margin * 2.0f, fontSize + 2.0f });



			yOffset += text->GetTextBoxHeight() + fontSize;

		}
		break;
		}

		element = element->next;
	}

	m_Panel->SetScrollable(true);
	m_Panel->SetScrollableArea(Vector2f(0.0f, yOffset));
	m_Panel->SetDrawVerticalScrollBar(true);
}

void NoteViewer::InitialiseGUIElements()
{

	SetupTitleAndExt();

	NoteElement* element = m_NoteFile.Start();

	float yOffset = 98.0f;

	float elementEndPadding = 16.0f;

	while (element != nullptr)
	{


		InitialiseGUIElement(element);


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
		case NoteElementType::BulletPoint:
			element->paragraph.text = ((gui::TextBoxSimplified*)element->userData)->string;
			break;
		case NoteElementType::Quote:
			element->quote.title = ((gui::TextBoxSimplified*)element->userData)->GetChild<gui::TextBoxSimplified>(1)->string;
			element->quote.text = ((gui::TextBoxSimplified*)element->userData)->string;
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

	InitialiseGUIElement(newElement);

	return newElement;
}

void NoteViewer::InitialiseGUIElement(NoteElement* element)
{
	switch (element->type)
	{
	case NoteElementType::Header1:
	case NoteElementType::Header2:
	{
		gui::TextBoxSimplified* text = m_Panel->NewChild<gui::TextBoxSimplified>();

		int fontPointSize = (element->type == NoteElementType::Header1) ? 32 : 24;
		float fontSize = m_FontManager->Get(gui::FontWeight::Bold, fontPointSize)->GetMaxHeight();
		text->SetFontManager(m_FontManager);
		text->SetBounds({ m_Margin, 10.0f, m_Panel->GetBounds().w - m_Margin * 2.0f, fontSize + 2.0f });
		text->SetFontSize(fontPointSize, gui::FontWeight::Bold);
		text->string = element->header.text;

		text->SetOnEditCallback([&](void* userData) {

			NoteElement* baseElement = (NoteElement*)userData;

			OnEdit(baseElement);

			ReformatGUI();
			}, element);

	
		element->userData = text;
	}
	break;
	case NoteElementType::Paragraph:
	{
		gui::TextBoxSimplified* text = m_Panel->NewChild<gui::TextBoxSimplified>();

		float fontSize = m_FontManager->Get(gui::FontWeight::Light, 14)->GetMaxHeight();
		text->SetFontManager(m_FontManager);
		text->SetBounds({ m_Margin, 10.0f, m_Panel->GetBounds().w - m_Margin * 2.0f, fontSize + 2.0f });
		text->SetFontSize(16, gui::FontWeight::Light);
		text->string = element->paragraph.text;

		text->SetOnEditCallback([&](void* userData) {

			NoteElement* baseElement = (NoteElement*)userData;

			OnEdit(baseElement);

			ReformatGUI();
			}, element);

	

		element->userData = text;
	}
	break;
	case NoteElementType::Dividor:
	{
		gui::ShapeWidget* shape = m_Panel->NewChild<gui::ShapeWidget>();

		shape->SetColour({ 0.4f, 0.4f, 0.4f, 1.0f });
		shape->SetPosition({ m_Margin, 10.0f });
		shape->SetAsRectangle({ m_Panel->GetBounds().w - m_Margin * 2.0f, 1.0f });

		element->userData = shape;


	}
	break;
	case NoteElementType::BulletPoint:
	{
		gui::TextBoxSimplified* text = m_Panel->NewChild<gui::TextBoxSimplified>();

		float fontSize = m_FontManager->Get(gui::FontWeight::Light, 14)->GetMaxHeight();
		text->SetFontManager(m_FontManager);
		text->SetBounds({ m_Margin + 32.0f, 10.0f, m_Panel->GetBounds().w - m_Margin * 2.0f, fontSize + 2.0f });
		text->SetFontSize(16, gui::FontWeight::Light);
		text->string = element->paragraph.text;

		text->SetOnEditCallback([&](void* userData) {

			NoteElement* baseElement = (NoteElement*)userData;

			OnEdit(baseElement);
			EnterDuplicateElement(baseElement);

			ReformatGUI();
			}, element);

		gui::ShapeWidget* circle = text->NewChild<gui::ShapeWidget>();
		circle->SetColour({ 1.0f, 1.0f, 1.0f, 1.0f });
		circle->SetPosition({ -16.0f, fontSize / 2.0f + 2.0f });
		circle->SetAsCircle(2.5f);



		element->userData = text;
	}
	break;
	case NoteElementType::Quote:
	{
		gui::TextBoxSimplified* text = m_Panel->NewChild<gui::TextBoxSimplified>();



		float fontSize = m_FontManager->Get(gui::FontWeight::Light, 14)->GetMaxHeight();

		
		text->SetFontManager(m_FontManager);
		text->SetBounds({ m_Margin + 32.0f, 10.0f, m_Panel->GetBounds().w - m_Margin * 2.0f, fontSize + 2.0f });
		text->SetFontSize(16, gui::FontWeight::Light);
		text->string = element->quote.text;

		text->SetOnEditCallback([&](void* userData) {

			NoteElement* baseElement = (NoteElement*)userData;

			OnEdit(baseElement);

			ReformatGUI();

			gui::TextBoxSimplified* text = (gui::TextBoxSimplified*)baseElement->userData;

			gui::ShapeWidget* shape = text->GetChild<gui::ShapeWidget>(0);
			float fontSize = m_FontManager->Get(gui::FontWeight::Light, 14)->GetMaxHeight();

			shape->SetAsRoundedRectangle({ m_Panel->GetBounds().w - m_Margin * 2.0f, text->GetTextBoxHeight() + fontSize * 3.0f }, 8.0f);

			}, element);



		gui::ShapeWidget* square = text->NewChild<gui::ShapeWidget>();
		square->SetColour({ 0.2f, 0.2f, 0.2f, 1.0f });
		square->SetPosition({ -32.0f, -fontSize * 2.0f });
		square->SetAsRoundedRectangle({ m_Panel->GetBounds().w - m_Margin * 2.0f, text->GetTextBoxHeight() + fontSize * 3.0f }, 8.0f);

		fontSize = m_FontManager->Get(gui::FontWeight::Bold, 16)->GetMaxHeight();

		gui::TextBoxSimplified* title = text->NewChild<gui::TextBoxSimplified>();
		title->SetFontManager(m_FontManager);
		title->SetBounds({ 0.0f, -fontSize * 1.0f, m_Panel->GetBounds().w - m_Margin * 2.0f, fontSize + 2.0f });
		title->SetFontSize(16, gui::FontWeight::BoldItalic);
		title->string = (element->quote.title.size() > 0) ? element->quote.title : "Note";
		title->SetIgnoreScissor(true);


		element->userData = text;
	}
	break;
	case NoteElementType::CodeBlock:
	{
		gui::TextBoxSimplified* text = m_Panel->NewChild<gui::TextBoxSimplified>();

		float fontSize = m_CodeFontManager->Get(gui::FontWeight::Regular, 14)->GetMaxHeight();


		text->SetFontManager(m_CodeFontManager);
		text->SetBounds({ m_Margin + 32.0f, 10.0f, m_Panel->GetBounds().w - m_Margin * 2.0f, fontSize + 2.0f });
		text->SetFontSize(14, gui::FontWeight::Regular);
		text->string = element->code.text;

		text->SetOnEditCallback([&](void* userData) {

			NoteElement* baseElement = (NoteElement*)userData;

			//OnEdit(baseElement);

			Command(baseElement);

			gui::TextBoxSimplified* text = (gui::TextBoxSimplified*)baseElement->userData;

			gui::ShapeWidget* shape = text->GetChild<gui::ShapeWidget>(0);
			float fontSize = m_FontManager->Get(gui::FontWeight::Light, 14)->GetMaxHeight();

			shape->SetAsRoundedRectangle({ m_Panel->GetBounds().w - m_Margin * 2.0f, text->GetTextBoxHeight() + fontSize * 1.5f }, 8.0f);

			ReformatGUI();



			}, element);

		gui::ShapeWidget* square = text->NewChild<gui::ShapeWidget>();
		square->SetColour({ 0.2f, 0.2f, 0.2f, 1.0f });
		square->SetPosition({ -32.0f, -fontSize * 0.5f });
		square->SetAsRoundedRectangle({ m_Panel->GetBounds().w - m_Margin * 2.0f, text->GetTextBoxHeight() + fontSize * 1.5f }, 8.0f);

		element->userData = text;
	}
		break;
	}


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
		OS::GetInstance().DebugPrint("Created new paragraph element\n");

		textBox->RemoveFocus();

		gui::TextBoxSimplified* newTextBox = (gui::TextBoxSimplified*)newElement->userData;

		newTextBox->TakeFocus();
	}
}

void NoteViewer::NewDividorOnNewLines(NoteElement* base)
{
	gui::TextBoxSimplified* textBox = (gui::TextBoxSimplified*)base->userData;

	std::string str = textBox->string;

	// We add a dividor if there is three - 
	// or you can use /div
	size_t pos = str.find("---");


	if (pos != std::string::npos)
	{
		textBox->string.erase(pos, 3);

		bool insertParagraph = (base->next == nullptr);

		NoteElement* newElement = InsertNewElement(base, NoteElementType::Dividor);
		OS::GetInstance().DebugPrint("Created new dividor element\n");

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
					if (str.substr(pos + 1, cmd.size()) == cmd && gui::EventHandler::enter)
					{
						// it matches

						bool insertParagraph = (base->next == nullptr);

						NoteElement* newElement = InsertNewElement(base, type);

						textBox->string.erase(pos, cmd.size() + 2);

						if (textBox->string.size() == 0)
							RemoveElement(base);

						// Take focus if the new element was a text based one
						if (type == NoteElementType::Paragraph || type == NoteElementType::Header1
							|| type == NoteElementType::Header2 || type == NoteElementType::BulletPoint 
							|| type == NoteElementType::Quote || type == NoteElementType::CodeBlock)
						{
							textBox->RemoveFocus();

							gui::TextBoxSimplified* newTextBox = (gui::TextBoxSimplified*)newElement->userData;

							newTextBox->TakeFocus();
						}



						if (type == NoteElementType::Dividor && insertParagraph)
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

void NoteViewer::RemoveElement(NoteElement* element)
{
	// TODO: Move this to the file

	NoteElement* parent = FindParent(element);


	OS::GetInstance().DebugPrint("Attempting remove element\n");

	// TODO: Actually delete the widget
	// Memory wise its all handled correctly on close. We just hide it until then. 
	gui::Widget* widget = (gui::Widget*)element->userData;
	widget->SetVisible(false);

	if (parent)
	{
		if (element->next)
		{
				parent->next = element->next;
		}
		else
		{
				parent->next = nullptr;
		}
	}
	else
	{
		// Assume root

		if (element->next)
			m_NoteFile.ReassignRoot(element->next);
	}
}

NoteElement* NoteViewer::FindParent(NoteElement* element)
{
	NoteElement* el = m_NoteFile.Start();

	if (el == element)
		return nullptr;

	while (el != nullptr)
	{
		if (el->next == element)
			break;

		el = el->next;
	}

	return el;
}

void NoteViewer::DeleteOnBackspace(NoteElement* element)
{
	gui::TextBoxSimplified* textBox = (gui::TextBoxSimplified*)element->userData;

	std::string str = textBox->string;

	if (str.size() == 0 && gui::EventHandler::backspace)
	{
		RemoveElement(element);
	}
}

void NoteViewer::EnterDuplicateElement(NoteElement* element)
{
	gui::TextBoxSimplified* textBox = (gui::TextBoxSimplified*)element->userData;


	if (textBox->string.size() > 1 && gui::EventHandler::enter)
	{
		size_t pos = textBox->string.find_last_of("\n");

		if (pos != std::string::npos && pos > textBox->string.size() - 2)
		{
			textBox->string.erase(pos);

			NoteElement* afterPara = InsertNewElement(element, element->type);

			textBox->RemoveFocus();

			gui::TextBoxSimplified* newTextBox = (gui::TextBoxSimplified*)afterPara->userData;

			newTextBox->TakeFocus();
		}
	}
	else if (gui::EventHandler::enter)
	{
		
		NoteElement* afterPara = InsertNewElement(element, NoteElementType::Paragraph);

		textBox->RemoveFocus();

		gui::TextBoxSimplified* newTextBox = (gui::TextBoxSimplified*)afterPara->userData;

		newTextBox->TakeFocus();

		RemoveElement(element);

	}
}