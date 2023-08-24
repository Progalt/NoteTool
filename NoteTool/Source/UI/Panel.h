#pragma once

#include "Widget.h"
#include "EventHandler.h"

namespace gui
{
	class Panel : public Widget
	{
	public:


		void GenerateVertexList(DrawList& drawList) override 
		{ 
			if (!m_DummyPanel)
			{

			}

			GenerateChildVertexLists(drawList);
		}

		// A dummy panel is a panel purely used for formatting reasons and not to actually be rendered
		void SetDummyPanel(bool dummy) { m_DummyPanel = dummy; }

	private:

		bool m_DummyPanel;
	};
}