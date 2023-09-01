
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

				drawList.Add(scrollBarBg.vertices, scrollBarBg.indices);

				// TODO: Fix this scrollbar 
				float visibleAreaFraction = m_GlobalBounds.h / (m_MaxScrollableArea.y + m_GlobalBounds.h);
				float scrollBarHeight = m_GlobalBounds.h * visibleAreaFraction;

				float offset = m_VisibleOffset.y - m_MaxScrollableArea.y + scrollBarHeight / 1.5f;



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
				static float target = m_VisibleOffset.y;

				if (EventHandler::verticalScroll > 0)
				{
					target -= m_ScrollAmount * 2.0f;
					//m_VisibleOffset.y = Lerp(m_VisibleOffset.y, target, EventHandler::deltaTime);

					if (target < 0.0f)
						target = 0.0f;
				}
				else if (EventHandler::verticalScroll < 0)
				{
					target += m_ScrollAmount * 2.0f;
					//m_VisibleOffset.y = Lerp(m_VisibleOffset.y, target, EventHandler::deltaTime);
					//m_VisibleOffset.y += m_ScrollAmount;

					if (target > m_MaxScrollableArea.y)
						target = m_MaxScrollableArea.y;

				}

				// 25 feels responsive enough but smooth enough
				m_VisibleOffset.y = Lerp(m_VisibleOffset.y, target, EventHandler::deltaTime * 25.0f);
			}
		}
	}
}