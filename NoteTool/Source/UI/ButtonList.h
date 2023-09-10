#pragma once

#include "Widget.h"
#include <functional>
#include "../Font.h"
#include "IconManager.h"

namespace gui
{


	class ButtonList : public Widget
	{
	public:

		~ButtonList()
		{
			for (auto& collection : m_Collections)
				collection.texture.Destroy();
		}

		struct Button
		{
			std::string text;

			std::function<void(void*)> callback;
			void* userData;

			bool hovered = true;


			GPUTexture texture;
			float textLength = 0.0f;
			bool rasterisedText = false;

			

			FloatRect bounds;
		};

		struct Collection
		{
			std::string name;

			std::vector<Button> m_Buttons;

			IconType iconOverride = IconType::None;
		

			bool hovered = true;
			bool expanded = true;
			bool excludeAutoClose = false;
			
			float paddingGap = 0.0f;
			bool renderDivLine = false;

			Collection& AddButton(const std::string& text, std::function<void(void*)> callback, void* userData)
			{
				m_Buttons.push_back({
						text,
						callback,
						userData,
						false
					});

				return *this;
			}

			GPUTexture texture;
			float textLength = 0.0f;
			bool rasterisedText = false;

			FloatRect bounds;
		};

		Collection& NewCollection(const std::string& name)
		{
			m_Collections.push_back({ name });
			return m_Collections[m_Collections.size() - 1];
		}

		void GenerateVertexList(DrawList& drawList) override;

		void OnEvent() override;

		void SetHoveredColour(Colour col) { m_HoveredColour = col; }

		void SetFont(Font* font) { m_Font = font; };

		void Clear()
		{
			if (m_Collections.size() > 0)
			{
				for (auto& collection : m_Collections)
					collection.texture.Destroy();

				m_Collections.clear();
			}
		}

	private:

		std::vector<Collection> m_Collections;
		Collection* m_ExpandedCollection;

		Colour m_HoveredColour;

		float m_ButtonSize = 24.0f;
		float m_ElementPadding = 6.0f;

		float m_HorizontalPadding = 30.0f;

		float m_IconSize = 24.0f;

		Font* m_Font;

		struct
		{
			bool active = false;
			FloatRect bounds;
		} m_BackButton;

		
		
	};
}