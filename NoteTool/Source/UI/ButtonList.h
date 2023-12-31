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

		virtual ~ButtonList()
		{
			Clear();
		}

		struct SideButton
		{
			IconType icon = IconType::None;

			std::function<void(void*)> callback;
			void* userData;

			Colour hoverColour = { 0.5f, 0.5f, 1.0f, 1.0f };

			FloatRect bounds;
			bool hovered = false;
		};

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

			std::vector<SideButton> sidebuttons;

			SideButton& AddSideButton(IconType icon, std::function<void(void*)> callback, void* userData)
			{
				sidebuttons.push_back({ icon, callback, userData });

				return sidebuttons[sidebuttons.size() - 1];
			}
		};

		struct Collection
		{
			std::string name;

			std::vector<Button> m_Buttons;
			std::vector<SideButton> sidebuttons;

			IconType iconOverride = IconType::None;
		

			bool hovered = true;
			bool expanded = true;
			bool excludeAutoClose = false;
			
			float paddingGap = 0.0f;
			bool renderDivLine = false;

			Button& AddButton(const std::string& text, std::function<void(void*)> callback, void* userData)
			{
				m_Buttons.push_back({
						text,
						callback,
						userData,
						false
					});

				return m_Buttons[m_Buttons.size() - 1];
			}


			

			SideButton& AddSideButton(IconType icon, std::function<void(void*)> callback, void* userData)
			{
				sidebuttons.push_back({ icon, callback, userData });

				return sidebuttons[sidebuttons.size() - 1];
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
				{
					for (auto& buttons : collection.m_Buttons)
						buttons.texture.Destroy();

					collection.texture.Destroy();
				}

				m_Collections.clear();
			}
		}

		Collection& GetCollection(uint32_t i) { return m_Collections[i]; }

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