#pragma once

#include "UI/Panel.h"
#include "UI/Text.h"
#include "UI/FontManager.h"
#include "UI/EventHandler.h"
#include <string>

enum class ModalType
{
	Success, 
	Failure
};

class ModalPopup
{
public:

	void Initialise(gui::Panel* parent, gui::FontManager* fontManager, float width)
	{
		m_FontManager = fontManager;

		m_Panel = parent->NewChild<gui::Panel>();

		m_Panel->SetBounds({ width - (float)250.0f / 2.0f, 10.0f, 250.0f, 30.0f });
		m_Panel->SetVisible(false);
		m_Panel->SetRounding(12.0f);
		m_Panel->SetTransparency(1.0f);
		m_Panel->SetColour({ 75, 181, 67, 255 });
		m_Panel->SetAnchor(gui::Anchor::Centre);

		m_Text = m_Panel->NewChild<gui::Text>();
		m_Text->SetFont(fontManager->Get(gui::FontWeight::Bold, 18));
		//m_Text->SetPosition({ m_T, 25.0f });
		
	}

	void DisplayModal(ModalType type, const std::string& str, float xPos)
	{
		m_Text->SetString(str);
		float textXPos = m_Panel->GetBounds().w / 2.0f - gui::GetTextLength(str, m_FontManager->Get(gui::FontWeight::Bold, 18)) / 2.0f;
		m_Text->SetPosition({ textXPos, 21.0f });

		switch (type)
		{
			case ModalType::Success:

				m_Panel->SetColour({ 75 - 20, 181 - 20, 67 - 20, 255 });		// Success Green :)

				break;
		}

		//m_Panel->SetPosition({ xPos - (float)m_Panel->GetBounds().w / 2.0f, 10.0f });
		m_Panel->SetVisible(true);

		m_CurrentModalTime = 0.0f;
	}

	void ModalUpdate()
	{
		m_CurrentModalTime += gui::EventHandler::deltaTime;

		static float yPos = -30.0f;
		float moveSpeed = 200.0f;

		

		if (m_CurrentModalTime < 0.5f)
		{
			yPos += moveSpeed * gui::EventHandler::deltaTime;
		}

		if (m_CurrentModalTime > 1.5f)
		{
			yPos -= moveSpeed * gui::EventHandler::deltaTime;
		}

		if (yPos > 10.0f)
			yPos = 10.0f;


		m_Panel->SetPosition({ m_Panel->GetBounds().x, yPos });

		if (m_CurrentModalTime > m_ModalTime)
		{
			m_Panel->SetVisible(false);
			yPos = -30.0f;
		}
	}

private:

	gui::Panel* m_Panel;

	gui::Text* m_Text;

	gui::FontManager* m_FontManager;

	float m_ModalTime = 2.0f;
	float m_CurrentModalTime = 0.0f;
};