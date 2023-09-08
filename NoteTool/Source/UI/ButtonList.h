#pragma once

#include "Widget.h"
#include <functional>

namespace gui
{
	class ButtonList : public Widget
	{
	public:

		void GenerateVertexList(DrawList& drawList) override;

		void AddButton(const std::string& text, std::function<void(void*)> callback, void* userData)
		{
			m_Buttons.push_back({
					text, 
					callback,
					userData,
					false
				});
		}

	private:

		struct Button
		{
			std::string text;

			std::function<void(void*)> callback;
			void* userData;

			bool hovered;
		};

		std::vector<Button> m_Buttons;
		
	};
}