#pragma once

#include "Widget.h"

namespace gui
{
	class Dividor : public Widget
	{
	public:

		void GenerateVertexList(DrawList& drawList) override
		{
			

			GenerateChildVertexLists(drawList);
		}

	private:

		float m_DivPadding = 10.0f;
		float m_DivThickness = 1.0f;
	};
}