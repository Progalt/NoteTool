#pragma once

#include "Widget.h"
#include "../Font.h"
#include "Utility.h"

namespace gui
{
	class DropDown : public Widget
	{
	public:

		void GenerateVertexList(DrawList& drawList) override
		{
			if (!m_DropDownPanel)
				CreateDropDownGUI();

			if (!m_Visible)
				return;

			// Draw base widget

			if (m_Hovered && !m_Clicked)
			{
				m_Temp = Lerp(m_Temp, m_HoveredColour, EventHandler::deltaTime * m_HoverSpeed);
			}
			else
			{
				m_Temp = Lerp(m_Temp, m_Colour, EventHandler::deltaTime * m_HoverSpeed);
			}

			if (m_Clicked)
			{


				m_Clicked = false;
			}

			Colour borderCol = { 1.0f, 1.0f, 1.0f, 1.0f };

			Colour col = m_Temp;

			col.a *= GetTransparency();

			Shape bg = gui::GenerateRoundedQuad({ m_GlobalBounds.position }, { m_GlobalBounds.position + m_GlobalBounds.size }, col, m_Rounding);

			if (m_ShadowColour.a != 0.0f)
			{
				Colour shadowCol = m_ShadowColour;
				shadowCol.a *= GetTransparency();
				Shape shadow = gui::GenerateRoundedQuad({ m_GlobalBounds.position - Vector2f(0.75f, 0.0f) }, { m_GlobalBounds.position + m_GlobalBounds.size + Vector2f(0.75f, 1.0f) }, shadowCol, m_Rounding);
				drawList.Add(shadow.vertices, shadow.indices);
			}

			if (m_Highlight.a != 0.0f && m_ShadowColour.a != 0.0f)
			{
				Colour highlightCol = m_Highlight;
				highlightCol.a *= GetTransparency();
				Shape highlight = gui::GenerateRoundedQuad({ m_GlobalBounds.position - Vector2f(0.75f, 1.0f) }, { m_GlobalBounds.position + m_GlobalBounds.size + Vector2f(0.75f, -1.0f) }, highlightCol, m_Rounding);
				drawList.Add(highlight.vertices, highlight.indices);
			}
			else if (m_Highlight.a != 0.0f && m_ShadowColour.a == 0.0f)
			{
				Colour highlightCol = m_Highlight;
				highlightCol.a *= GetTransparency();
				Shape highlight = gui::GenerateRoundedQuad({ m_GlobalBounds.position - Vector2f(0.75f, 1.0f) }, { m_GlobalBounds.position + m_GlobalBounds.size + Vector2f(0.75f, 1.0f) }, highlightCol, m_Rounding);
				drawList.Add(highlight.vertices, highlight.indices);
			}


			drawList.Add(bg.vertices, bg.indices, nullptr);

			float halfPos = m_GlobalBounds.y + (m_GlobalBounds.h / 2.0f);
			Shape tri = gui::GenerateAlignedEqualTri({ m_GlobalBounds.x + m_GlobalBounds.w - 25.0f, halfPos - 2.5f }, { m_GlobalBounds.x + m_GlobalBounds.w - 15.0f, halfPos + 3.0f }, m_DropDownTriColour, 2);

			drawList.Add(tri.vertices, tri.indices, nullptr);

			// Draw text

			std::string text = m_DefaultMessage;

			if (!m_CurrentSelection.empty())
				text = m_CurrentSelection;

			float lineSpacing = (float)m_Font->GetLineSpacing();
			Vector2i position;
			position.y = m_GlobalBounds.y + lineSpacing;
			float textLength = gui::GetTextLength(text, m_Font);

			Colour textCol = { 1.0f, 1.0f, 1.0f, 1.0f };
			textCol.a *= GetTransparency(); 

			position.x = m_GlobalBounds.x + 5.0f;

			gui::RenderText(drawList, text, m_Font, position, 0.0f, textCol);

			if (m_IsOpen)
				m_DropDownPanel->SetVisible(true);
			else 
				m_DropDownPanel->SetVisible(false);

			GenerateChildVertexLists(drawList);
		}

		void OnEvent() override
		{
			if (!m_Visible)
				return;

			m_Hovered = false;
			bool withinDropDownPanel = false;
			if (m_GlobalBounds.Contains((float)EventHandler::x, (float)EventHandler::y))
			{
				m_Hovered = true;
			}

			if (m_IsOpen)
			{
				if (m_DropDownPanel->GetBounds().Contains((float)EventHandler::x, (float)EventHandler::y))
				{
					withinDropDownPanel = true;
				}

			}

			if (m_Hovered)
			{

				if ( EventHandler::mouseButton[MouseButton::MOUSE_LEFT].clicks >= 1)
				{
					m_Clicked = true;
					m_IsOpen = !m_IsOpen;
				}

			}
			else
			{
				if (EventHandler::mouseButton[MouseButton::MOUSE_LEFT].clicks >= 1 && !withinDropDownPanel)
				{
					m_IsOpen = false;
				}
			}

		}

		void SetSelection(const std::string& selection)
		{
			assert(ExistsInList(selection));

			m_CurrentSelection = selection;
		}

		void SetList(std::vector<std::string> list)
		{
			m_SelectionList = list;
		}

		void SetFont(Font* font)
		{
			m_Font = font;
		}


		void SetHighlightColour(Colour col) { m_Highlight = col; }
		void SetShadowColour(Colour col) { m_ShadowColour = col; }
		void SetHoveredColour(Colour col) { m_HoveredColour = col; }


	private:

		bool ExistsInList(const std::string& str)
		{
			for (auto& list : m_SelectionList)
				if (list == str)
					return true;

			return false;
		}

		void CreateDropDownGUI()
		{
			float entrySize = 25.0f;

			m_DropDownPanel = NewChild<Panel>();
			m_DropDownPanel->SetBounds({ 0.0f, m_GlobalBounds.h, m_GlobalBounds.w, 200.0f });
			m_DropDownPanel->SetColour(m_Colour);
			m_DropDownPanel->SetRounding(0.0f);
			m_DropDownPanel->SetHighlightColour(m_Highlight);
			m_DropDownPanel->SetDrawVerticalScrollBar(true);
			m_DropDownPanel->SetScrollable(true);
			m_DropDownPanel->SetScrollableArea({ 0.0f, entrySize * (float)m_SelectionList.size() - m_DropDownPanel->GetBounds().h });

		

			for (size_t i = 0; i < m_SelectionList.size(); i++)
			{
				Button* button = m_DropDownPanel->NewChild<Button>();
				button->SetRounding(0.0f);
				button->SetColour(m_Colour);
				button->SetHoveredColour(m_HoveredColour);
				button->SetBounds({ 0.0f, (float)i * entrySize, m_GlobalBounds.w - 12.0f, entrySize });
				button->SetText(m_SelectionList[i], m_Font, gui::Alignment::Left);
				button->SetUserData(&m_SelectionList[i]);
				button->SetOnClick([&](void* userData) { m_CurrentSelection = *(std::string*)userData; });

				m_DropDownButtons.push_back(button);
			}
		}

		Font* m_Font;

		std::vector<std::string> m_SelectionList;
		std::string m_CurrentSelection = "";

		std::string m_DefaultMessage = "None Selected";

		bool m_IsOpen = false;

		Colour m_Highlight = Colour(1.0f, 1.0f, 1.0f, 0.0f);
		Colour m_HoveredColour;
		Colour m_ShadowColour = Colour(1.0f, 1.0f, 1.0f, 0.0f);

		Colour m_DropDownTriColour = Colour(0.8f, 0.8f, 0.8f, 1.0f);

		bool m_Clicked = false;
		Colour m_Temp = m_Colour;

		float m_HoverSpeed = 30.0f;


		// Drop down gui

		Panel* m_DropDownPanel;
		std::vector<Button*> m_DropDownButtons;
	};
}