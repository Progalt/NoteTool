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
			if (!m_DummyPanel)
			{
				Colour col = m_Colour;
				col.a *= GetTransparency();

				Shape panelBg = gui::GenerateRoundedQuad(m_GlobalBounds.position, m_GlobalBounds.position + m_GlobalBounds.size, col, m_Rounding);


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

				drawList.Add(panelBg.vertices, panelBg.indices);
			}

			GenerateChildVertexLists(drawList);
		}

		void OnEvent()
		{
			
		}

		// A dummy panel is a panel purely used for formatting reasons and not to actually be rendered
		void SetDummyPanel(bool dummy) { m_DummyPanel = dummy; }

		void SetHighlightColour(Colour col) { m_Highlight = col; }
		void SetShadowColour(Colour col) { m_ShadowColour = col; }
		
	private:

		bool m_DummyPanel;

		float m_PopUpTime = 0.5f;

		Colour m_Highlight = Colour(1.0f, 1.0f, 1.0f, 0.0f);
		Colour m_ShadowColour = Colour(1.0f, 1.0f, 1.0f, 0.0f);


	};
}