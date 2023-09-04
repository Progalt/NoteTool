#pragma once

#include "Widget.h"
#include "EventHandler.h"
#include "Utility.h"
#include "ContextMenu.h"

namespace gui
{
	enum class PanelFlags
	{
		None = 1 << 0,
		DrawBorder = 1 << 1
	};

	inline PanelFlags operator|(PanelFlags lh, PanelFlags rh)
	{
		return static_cast<PanelFlags>(static_cast<int>(lh) | static_cast<int>(rh));
	}

	inline bool operator&(PanelFlags lh, PanelFlags rh)
	{
		return static_cast<int>(lh) & static_cast<int>(rh);
	}


	class Panel : public Widget
	{
	public:


		void GenerateVertexList(DrawList& drawList) override;

		void OnEvent() override;

		// A dummy panel is a panel purely used for formatting reasons and not to actually be rendered
		void SetDummyPanel(bool dummy) { m_DummyPanel = dummy; }

		void SetHighlightColour(Colour col) { m_Highlight = col; }
		void SetShadowColour(Colour col) { m_ShadowColour = col; }

		void SetDrawVerticalScrollBar(bool draw) { m_DrawScroll = draw; }

		void SetScrollable(bool scrollable) { m_Scrollable = scrollable; }
		
		void SetScrollableArea(Vector2f area) { m_MaxScrollableArea = area; }

		void SetCurrentScrollDistance(float dist) {
			m_VisibleOffset.y = dist; 
			m_ScrollTargetY = dist;
		}

		void SetFlags(PanelFlags flag) { m_Flags = flag; }

		ContextMenu* GetContextMenu()
		{
			if (!m_ContextMenu)
			{
				m_ContextMenu = NewChild<ContextMenu>();
				m_ContextMenu->SetVisible(false);
			}

			return m_ContextMenu;
		}

	private:

		PanelFlags m_Flags;

		bool m_DummyPanel;

		float m_PopUpTime = 0.5f;

		Colour m_Highlight = Colour(1.0f, 1.0f, 1.0f, 0.0f);
		Colour m_ShadowColour = Colour(1.0f, 1.0f, 1.0f, 0.0f);

		Colour m_ScrollBg = Colour(0.001f, 0.001f, 0.001f, 0.001f);

		bool m_DrawScroll = false;

		bool m_Scrollable = false;

		float m_ScrollAmount = 20.0f;

		float m_ScrollTargetY = m_VisibleOffset.y;

		Vector2f m_MaxScrollableArea = Vector2f(10000.0f, 100000.0f);

		ContextMenu* m_ContextMenu;

	};
}