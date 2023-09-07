
#include "Panel.h"

namespace gui
{
	void Panel::GenerateVertexList(DrawList& drawList)
	{
		if (!m_Visible)
			return;



		if (!m_DummyPanel)
		{


			Colour col = m_Colour;
			col.a *= GetTransparency();

			Shape panelBg = gui::GenerateRoundedQuad(m_GlobalBounds.position, m_GlobalBounds.position + m_GlobalBounds.size, col, m_Rounding);



			if (m_Flags & PanelFlags::DrawBorder)
			{
				Colour highlightCol = m_Highlight;
				highlightCol.a *= GetTransparency();
				Shape highlight = gui::GenerateRoundedQuad({ m_GlobalBounds.position - Vector2f(1.0f, 1.0f) }, { m_GlobalBounds.position + m_GlobalBounds.size + Vector2f(1.0f, 1.0f) }, highlightCol, m_Rounding);
				drawList.Add(highlight.vertices, highlight.indices);
			}


			drawList.Add(panelBg.vertices, panelBg.indices);

			if (m_DrawScroll)
			{
				float scrollBarSize = 12.0f;
				float xPos = m_GlobalBounds.x + m_GlobalBounds.w - scrollBarSize;
				float yPos = m_GlobalBounds.y;
				Shape scrollBarBg = gui::GenerateQuad({ xPos, yPos }, { xPos + scrollBarSize, yPos + m_GlobalBounds.h }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, m_Colour);

				//drawList.Add(scrollBarBg.vertices, scrollBarBg.indices);

				float maxArea = m_GlobalBounds.h + m_MaxScrollableArea.y;

				float viewableRatio = m_GlobalBounds.h / maxArea;
				float scrollBarHeight = m_GlobalBounds.h * viewableRatio;
				float scrolledRatio = m_VisibleOffset.y / maxArea;
				float offset = m_GlobalBounds.h * scrolledRatio;

				Shape scrollBar = gui::GenerateRoundedQuad({ xPos, yPos + offset }, { xPos + 8.0f, yPos + scrollBarHeight + offset }, m_Highlight, 4.0f);

				drawList.Add(scrollBar.vertices, scrollBar.indices);
			}


		}

		drawList.SetScissor(m_GlobalBounds);

		if (m_Scrollable)
		{

			RecalculateAllBounds();
			GenerateChildVertexLists(drawList);

		}
		else
		{
			GenerateChildVertexLists(drawList);
		}

		drawList.SetScissor(IntRect());


	}

	void Panel::OnEvent()
	{
		if (m_GlobalBounds.Contains((float)EventHandler::x, (float)EventHandler::y))
		{
			if (m_Scrollable)
			{
			
				if (EventHandler::verticalScroll > 0)
				{
					m_ScrollTargetY -= m_ScrollAmount * 2.0f;
					//m_VisibleOffset.y = Lerp(m_VisibleOffset.y, target, EventHandler::deltaTime);

					if (m_ScrollTargetY < 0.0f)
						m_ScrollTargetY = 0.0f;
				}
				else if (EventHandler::verticalScroll < 0)
				{
					m_ScrollTargetY += m_ScrollAmount * 2.0f;
					//m_VisibleOffset.y = Lerp(m_VisibleOffset.y, target, EventHandler::deltaTime);
					//m_VisibleOffset.y += m_ScrollAmount;

					if (m_ScrollTargetY > m_MaxScrollableArea.y)
						m_ScrollTargetY = m_MaxScrollableArea.y;

				}

				// 25 feels responsive enough but smooth enough
				m_VisibleOffset.y = Lerp(m_VisibleOffset.y, m_ScrollTargetY, EventHandler::deltaTime * 25.0f);
			}

			if (EventHandler::mouseButton[MOUSE_LEFT].clicks == 1)
			{
				if (m_OnFocusCallback)
					m_OnFocusCallback(m_UserData);
			}

			if (m_ContextMenu && EventHandler::mouseButton[MOUSE_RIGHT].clicks >= 1)
			{
				m_ContextMenu->SetPosition({ (float)EventHandler::x + 5.0f, (float)EventHandler::y + 5.0f });
				m_ContextMenu->Open();
			}
		}
	}
}