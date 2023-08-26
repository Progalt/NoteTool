#pragma once

#include "Widget.h"
#include "EventHandler.h"
#include "Utility.h"

namespace gui
{
	enum class PanelFlags
	{

	};

	inline PanelFlags operator|(PanelFlags lh, PanelFlags rh)
	{
		return static_cast<PanelFlags>(static_cast<int>(lh) | static_cast<int>(rh));
	}



	class Panel : public Widget
	{
	public:


		void GenerateVertexList(DrawList& drawList) override 
		{ 
			if (!m_Visible)
				return;

			drawList.SetScissor(m_GlobalBounds);

			if (!m_DummyPanel)
			{
				

				Colour col = m_Colour;
				col.a *= GetTransparency();

				Shape panelBg = gui::GenerateRoundedQuad(m_GlobalBounds.position, m_GlobalBounds.position + m_GlobalBounds.size, col, m_Rounding);


			
				if (m_Highlight.a != 0.0f)
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
					float visibleAreaFraction = m_GlobalBounds.h / (m_MaxScrollableArea.y + m_GlobalBounds.h) ;
					float scrollBarHeight = m_GlobalBounds.h * visibleAreaFraction;

					float offset = m_VisibleOffset.y - m_MaxScrollableArea.y + scrollBarHeight / 1.5f;

				

					Shape scrollBar = gui::GenerateRoundedQuad({ xPos, yPos + offset }, { xPos + 8.0f, yPos + scrollBarHeight + offset }, m_Highlight, 4.0f);

					drawList.Add(scrollBar.vertices, scrollBar.indices);
				}
			} 

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

		void OnEvent()
		{
			if (m_GlobalBounds.Contains((float)EventHandler::x, (float)EventHandler::y))
			{
				if (m_Scrollable)
				{
					static float target = m_VisibleOffset.y;

					if (EventHandler::verticalScroll > 0)
					{
						target -= m_ScrollAmount;
						//m_VisibleOffset.y = Lerp(m_VisibleOffset.y, target, EventHandler::deltaTime);

						if (target < 0.0f)
							target = 0.0f;
					}
					else if (EventHandler::verticalScroll < 0)
					{
						 target += m_ScrollAmount;
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

		// A dummy panel is a panel purely used for formatting reasons and not to actually be rendered
		void SetDummyPanel(bool dummy) { m_DummyPanel = dummy; }

		void SetHighlightColour(Colour col) { m_Highlight = col; }
		void SetShadowColour(Colour col) { m_ShadowColour = col; }

		void SetDrawVerticalScrollBar(bool draw) { m_DrawScroll = draw; }

		void SetScrollable(bool scrollable) { m_Scrollable = scrollable; }
		
		void SetScrollableArea(Vector2f area) { m_MaxScrollableArea = area; }

	private:

		bool m_DummyPanel;

		float m_PopUpTime = 0.5f;

		Colour m_Highlight = Colour(1.0f, 1.0f, 1.0f, 0.0f);
		Colour m_ShadowColour = Colour(1.0f, 1.0f, 1.0f, 0.0f);

		Colour m_ScrollBg = Colour(0.001f, 0.001f, 0.001f, 0.001f);

		bool m_DrawScroll = false;

		bool m_Scrollable = false;

		float m_ScrollAmount = 20.0f;

		Vector2f m_MaxScrollableArea = Vector2f(10000.0f, 100000.0f);

	};
}