#include "ButtonList.h"
#include "Utility.h"
#include "IconManager.h"

namespace gui
{
	void ButtonList::GenerateVertexList(DrawList& drawList)
	{
		if (!m_Visible)
			return;

		float yOffset = 0.0f;

		auto rasteriseText = [&](GPUTexture* texture, const std::string& str, float* textLen)
			{
				Image img;

				float length = gui::GetTextLength(str, m_Font);


				uint32_t height = m_Font->GetMaxHeight();
				img.New((uint32_t)length + gui::TextPadding, height + gui::TextPadding, 4);
				img.Fill({ 0.0f, 0.0f, 0.0f, 0.0f });

				float baseLine = (float)m_Font->GetAscent();

				gui::RenderTextSoftware(img, str, m_Font, {}, length, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, length + (float)gui::TextPadding, (float)height + (float)gui::TextPadding }, baseLine);

				texture->CreateFromImage(img);

				*textLen = length + gui::TextPadding;
			};


		for (uint32_t i = 0; i < m_Collections.size(); i++)
		{
			Collection& currentCollection = m_Collections[i];

			// First render the collection as a normal button


			Vector2f min = m_GlobalBounds.position;
			min.y += yOffset;
			Vector2f max = min;
			max.y += m_ButtonSize;
			max.x += m_GlobalBounds.size.x;

			currentCollection.bounds.position = min;
			currentCollection.bounds.size = max - min;

			Colour iconCol = { 1.0f, 1.0f, 1.0f, 1.0f };

			if (currentCollection.hovered)
			{
				// We only render the bg if hovered


				iconCol = { 0.5f, 0.5f, 1.0f, 1.0f };




			}

			// Render Icon

			IconType iconType = IconType::Folder_Empty;

			if (currentCollection.iconOverride != IconType::None)
				iconType = currentCollection.iconOverride;

			GPUTexture* icon = nullptr;
			icon = IconManager::GetInstance()[iconType];
			if (icon != nullptr)
			{
				Vector2f iconMin = m_GlobalBounds.position;
				iconMin.y += yOffset;
				Vector2f iconMax = iconMin;
				iconMax.x += m_IconSize;
				iconMax.y += m_IconSize;
				gui::Shape iconQuad = gui::GenerateQuad(iconMin, iconMax, { 0.0f, 0.0f }, { 1.0f, 1.0f }, iconCol);

				drawList.Add(iconQuad.vertices, iconQuad.indices, icon);
			}

			// Render the text

			// On first pass we want to rasterise the text 
			if (!currentCollection.rasterisedText)
			{
				rasteriseText(&currentCollection.texture, currentCollection.name, &currentCollection.textLength);
				currentCollection.rasterisedText = true;
			}

			min = m_GlobalBounds.position;
			min.x += m_HorizontalPadding;
			min.y += yOffset;
			min.y += (float)(m_Font->GetAscent() / 2) - 3.0f;
			max = min;
			max.y += m_Font->GetMaxHeight() + gui::TextPadding;
			max.x += currentCollection.textLength;
			Shape textQuad = gui::GenerateQuad(min, max, { 0.0f, 0.0f }, { 1.0f, 1.0f }, iconCol);

			drawList.Add(textQuad.vertices, textQuad.indices, &currentCollection.texture);

			// Lets draw a side button
			if (currentCollection.hovered)
			{
				for (uint32_t j = 0; j < currentCollection.sidebuttons.size(); j++)
				{
					SideButton& sideButton = currentCollection.sidebuttons[j];

					if (sideButton.icon == IconType::None)
						continue;

					min = m_GlobalBounds.position;
					min.x += m_GlobalBounds.size.x - m_IconSize * (float)(j + 1);
					min.y += yOffset;
					max = min;
					max.y += m_IconSize;
					max.x += m_IconSize;


					Colour sideCol = m_HoveredColour;

					if (sideButton.hovered)
					{
						sideCol = sideButton.hoverColour;
					}

					Shape sideButtonQuad = gui::GenerateQuad(min, max, { 0.0f, 0.0f }, { 1.0f, 1.0f }, sideCol);
					GPUTexture* sideIcon = nullptr;
					sideIcon = IconManager::GetInstance()[sideButton.icon];

					drawList.Add(sideButtonQuad.vertices, sideButtonQuad.indices, sideIcon);

					sideButton.bounds.position = min;
					sideButton.bounds.size = max - min;

				}
			}

			// Offset the Y for the next element
			yOffset += m_ButtonSize + m_ElementPadding;

			// If its a list we want to then render the contents
			if (currentCollection.expanded)
			{
				float fileTreeHorzPos = m_GlobalBounds.x + m_IconSize / 2.0f - 1.0f;

				// Render a file tree like shape
				if (currentCollection.m_Buttons.size() > 0)
				{
					float treeLength = (float)(currentCollection.m_Buttons.size()) * (m_ButtonSize + m_ElementPadding) - m_ButtonSize / 1.5f;

					{
						min = m_GlobalBounds.position;
						min.y += yOffset;
						min.x = fileTreeHorzPos;
						max = min;
						max.y += treeLength;
						max.x += 2.0f;
						Shape shape = gui::GenerateQuad(min, max, {}, {}, m_HoveredColour);

						drawList.Add(shape.vertices, shape.indices);
					}
				}

				for (auto& entry : currentCollection.m_Buttons)
				{
					// Rendered similar to the collections but with an offset 

					min = m_GlobalBounds.position;
					min.y += yOffset;
					max = min;
					max.y += m_ButtonSize;
					max.x += m_GlobalBounds.size.x;

					entry.bounds.position = min;
					entry.bounds.size = max - min;

					Colour col = { 1.0f, 1.0f, 1.0f, 1.0f };
						 
					if (entry.hovered)
					{
						col = { 0.5f, 0.5f, 1.0f, 1.0f };
					}

					IconType iconType = IconType::BasicFile;

				
					GPUTexture* icon = nullptr;
					icon = IconManager::GetInstance()[iconType];

					if (icon != nullptr)
					{
						Vector2f iconMin = m_GlobalBounds.position;
						iconMin.y += yOffset;
						iconMin.x += m_HorizontalPadding;
						Vector2f iconMax = iconMin;
						iconMax.x += m_IconSize;
						iconMax.y += m_IconSize;
						gui::Shape iconQuad = gui::GenerateQuad(iconMin, iconMax, { 0.0f, 0.0f }, { 1.0f, 1.0f }, col);

						drawList.Add(iconQuad.vertices, iconQuad.indices, icon);
					}

					if (!entry.rasterisedText)
					{
						rasteriseText(&entry.texture, entry.text, &entry.textLength);
						entry.rasterisedText = true;
					}

					min = m_GlobalBounds.position;
					min.x += m_HorizontalPadding + 28.0f;
					min.y += yOffset;
					min.y += (float)(m_Font->GetAscent() / 2) - 2.0f;
					max = min;
					max.y += m_Font->GetMaxHeight() + gui::TextPadding;
					max.x += entry.textLength;

					Shape textQuad = gui::GenerateQuad(min, max, { 0.0f, 0.0f }, { 1.0f, 1.0f }, col);

					drawList.Add(textQuad.vertices, textQuad.indices, &entry.texture);

					// Lets draw a side button
					if (entry.hovered)
					{
						for (uint32_t j = 0; j < entry.sidebuttons.size(); j++)
						{
							SideButton& sideButton = entry.sidebuttons[j];

							if (sideButton.icon == IconType::None)
								continue;

							min = m_GlobalBounds.position;
							min.x += m_GlobalBounds.size.x - m_IconSize * (float)(j + 1);
							min.y += yOffset;
							max = min;
							max.y += m_IconSize;
							max.x += m_IconSize;
							

							Colour sideCol = m_HoveredColour;

							if (sideButton.hovered)
							{
								sideCol = sideButton.hoverColour;
							}

							Shape sideButtonQuad = gui::GenerateQuad(min, max, { 0.0f, 0.0f }, { 1.0f, 1.0f }, sideCol);
							GPUTexture* sideIcon = nullptr;
							sideIcon = IconManager::GetInstance()[sideButton.icon];

							drawList.Add(sideButtonQuad.vertices, sideButtonQuad.indices, sideIcon);

							sideButton.bounds.position = min;
							sideButton.bounds.size = max - min;

						}
					}

					// Draw the file tree connector

					min = m_GlobalBounds.position;
					min.x = fileTreeHorzPos;
					min.y += yOffset + m_ButtonSize / 2.0f;
					max = min;
					max.y += 2.0f;
					max.x = m_HorizontalPadding + 4.0f;


					Shape connector = gui::GenerateQuad(min, max, {}, {}, m_HoveredColour);

					drawList.Add(connector.vertices, connector.indices);

					

					yOffset += m_ButtonSize + m_ElementPadding;




				}

			



			}

			if (currentCollection.paddingGap > 0.0f && currentCollection.renderDivLine)
			{
				Vector2f div = { m_GlobalBounds.x, m_GlobalBounds.y + yOffset + currentCollection.paddingGap / 2.0f };
				div.y -= 1.0f;
				Vector2f divEnd = div;
				divEnd.y += 2.0f;
				divEnd.x += m_GlobalBounds.w;

				Shape shape = gui::GenerateQuad(div, divEnd, {}, {}, m_HoveredColour);

				drawList.Add(shape.vertices, shape.indices);
			}

			yOffset += currentCollection.paddingGap;
		}

		Vector2f div = { m_GlobalBounds.x, m_GlobalBounds.y + yOffset + 8.0f };
		div.y -= 1.0f;
		Vector2f divEnd = div;
		divEnd.y += 2.0f;
		divEnd.x += m_GlobalBounds.w;

		Shape shape = gui::GenerateQuad(div, divEnd, {}, {}, m_HoveredColour);

		drawList.Add(shape.vertices, shape.indices);


		GenerateChildVertexLists(drawList);
	}

	void ButtonList::OnEvent()
	{


		for (uint32_t i = 0; i < m_Collections.size(); i++)
		{
			Collection& currentCollection = m_Collections[i];

			if (currentCollection.bounds.Contains((float)gui::EventHandler::x, (float)gui::EventHandler::y))
			{
				currentCollection.hovered = true;

				for (auto& sidebutton : currentCollection.sidebuttons)
				{
					if (sidebutton.bounds.Contains((float)gui::EventHandler::x, (float)gui::EventHandler::y))
					{
						sidebutton.hovered = true;
					}
					else
					{
						sidebutton.hovered = false;
					}

					if (sidebutton.hovered && gui::EventHandler::mouseButton[gui::MOUSE_LEFT].clicks >= 1)
					{
						if (sidebutton.callback)
							sidebutton.callback(sidebutton.userData);
					}
				}
			}
			else
			{
				currentCollection.hovered = false;
			}


			if (currentCollection.hovered && gui::EventHandler::mouseButton[gui::MOUSE_LEFT].clicks >= 1)
			{
				float maxX = currentCollection.bounds.x + currentCollection.bounds.w - (m_IconSize * (float)currentCollection.sidebuttons.size());
				if ((float)gui::EventHandler::x < maxX)
					currentCollection.expanded = !currentCollection.expanded;
			}

			if (currentCollection.expanded)
			{
				for (auto& entry : currentCollection.m_Buttons)
				{
					if (entry.bounds.Contains((float)gui::EventHandler::x, (float)gui::EventHandler::y))
					{
						entry.hovered = true;
					}
					else
					{
						entry.hovered = false;
					}

					if (entry.hovered && gui::EventHandler::mouseButton[gui::MOUSE_LEFT].clicks >= 1)
					{
						float maxX = entry.bounds.x + entry.bounds.w - (m_IconSize * (float)entry.sidebuttons.size());
						if ((float)gui::EventHandler::x < maxX)
						{

							if (entry.callback)
								entry.callback(entry.userData);
						}
					}

					for (auto& sidebutton : entry.sidebuttons)
					{
						if (sidebutton.bounds.Contains((float)gui::EventHandler::x, (float)gui::EventHandler::y))
						{
							sidebutton.hovered = true;
						}
						else
						{
							sidebutton.hovered = false;
						}

						if (sidebutton.hovered && gui::EventHandler::mouseButton[gui::MOUSE_LEFT].clicks >= 1)
						{
							if (sidebutton.callback)
								sidebutton.callback(sidebutton.userData);
						}
					}
				}
			}

		}



	}
}