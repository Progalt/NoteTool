#pragma once

#include "Panel.h"
#include "Button.h"
#include "EventHandler.h"
#include "../Icons.h"
#include "../Image.h"

namespace gui
{

	struct ListEntry
	{
		ListEntry() { }
		ListEntry(const std::string& text, std::function<void(void*)> callback, void* userData = nullptr, bool directory = false) : text(text), callback(callback), userData(userData), directory(directory) { textData.rerender = true; }

		void AddChild(ListEntry entry) { children.push_back(entry); }

		std::string text;
		std::function<void(void*)> callback;
		void* userData = nullptr;

		std::vector<ListEntry> children;

		bool directory = false;

		FloatRect boundingBox;
		bool hovered = false;
		bool expanded = true;
		bool selected = false;

		struct TextEntry
		{
			GPUTexture texture;
			Image image;
			bool rerender = true;
			FloatRect textBounds;


			void RasterizeText(const std::string& str, Font* font)
			{
				textBounds.w = gui::GetTextLength(str, font) + gui::TextPadding;
				textBounds.h = font->GetMaxHeight() + gui::TextPadding;

				image.New((int)textBounds.w, textBounds.h, 4);
				image.Fill({ 0.0f, 0.0f, 0.0f, 0.0f });

				float baseLine = (float)font->GetAscent();

				gui::RenderTextSoftware(image, str, font, {}, textBounds.w, { 1.0f, 1.0f, 1.0f, 1.0f }, textBounds, baseLine);

				texture.CreateFromImage(image);
			}
		} textData;
	};

	class List : public Widget
	{
	public:

		void GenerateVertexList(DrawList& drawList) override
		{
			if (!m_Visible)
				return;

			m_CurrentYOffset = m_GlobalBounds.y + 5.0f + m_Font->GetLineSpacing();
			uint32_t indents = 0;

			for (auto& baseEntries : m_Entries)
			{
				RenderEntry(drawList, baseEntries, indents);
			}

			GenerateChildVertexLists(drawList);
		}

		void OnEvent()
		{


			for (auto& entry : m_Entries)
				HandleEventsForEntry(entry);
		}

		void AddEntry(ListEntry entry)
		{
			m_Entries.push_back(entry);
		}

		void SetFont(Font* font) { m_Font = font; }

		void SetHoveredColour(Colour col) { m_HoveredColour = col; }
		void SetTextColour(Colour col) { m_TextColour = col; }

		void Clear()
		{
			m_Entries.clear();
		}

		ListEntry* GetSelected()
		{
			return m_Selected;
		}

	private:

		void HandleEventsForEntry(ListEntry& entry)
		{
			if (!m_Visible)
				return;

			entry.hovered = false;
			if (!entry.boundingBox.IsNull())
			{
				if (entry.boundingBox.Contains((float)EventHandler::x, (float)EventHandler::y))
				{
					entry.hovered = true;
				}
			}

			if (entry.hovered)
			{
				if (EventHandler::mouseButton[MouseButton::MOUSE_LEFT].clicks > 0)
				{
					if (!entry.directory)
					{
						if (m_Selected)
							m_Selected->selected = false;
						entry.selected = true;

						if (entry.callback)
							entry.callback(entry.userData);

						m_Selected = &entry;
					}
					else
					{
						if (m_Selected)
							m_Selected->selected = false;
						entry.selected = true;

						entry.expanded = !entry.expanded;

						m_Selected = &entry;
					}
				}
			}

			for (auto& child : entry.children)
				HandleEventsForEntry(child);
		}

		void RenderEntry(DrawList& list, ListEntry& entry,  uint32_t& indents)
		{
			float xPosNoIndents = m_GlobalBounds.position.x + 24.0f;
			float xPos = xPosNoIndents + (float)indents * m_IndentSize;
			float yPos = m_GlobalBounds.position.y + m_CurrentYOffset;

			entry.boundingBox = { m_GlobalBounds.position.x + 2.0f, yPos,  m_GlobalBounds.w - 10.0f, m_EntrySize };

			if (entry.hovered || entry.selected)
			{
				Shape shape = gui::GenerateRoundedQuad(entry.boundingBox.position, entry.boundingBox.position + entry.boundingBox.size, m_HoveredColour, 4.0f);

				list.Add(shape.vertices, shape.indices);
			}

			for (uint32_t i = 0; i < indents; i++)
			{
				float xPosI = xPosNoIndents + ((float)i -1.0f) * m_IndentSize;
				float dist = (18.0f - 9.0f) / 2.0f; 
				Shape line = gui::GenerateQuad({ xPosI - dist + 3.0f, yPos }, { xPosI - dist + 3.5f, yPos + m_EntrySize }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, m_TextColour);

				list.Add(line.vertices, line.indices);
			}

			float textYPos = yPos + (m_EntrySize / 2.0f) - (float)m_Font->GetMaxHeight() / 2.0f;// + ((float)m_Font->GetPixelSize() / 3.0f);
			//gui::RenderText(list, entry.text, m_Font, { (int)xPos,  (int)textYPos   }, 0.0f, m_TextColour, m_GlobalBounds);

			// Render the text
			if (entry.textData.rerender)
			{
				entry.textData.RasterizeText(entry.text, m_Font);
				entry.textData.rerender = false;
			}
			
			Vector2f position = { xPos,  textYPos };
			Shape quad = gui::GenerateQuad(position, position + entry.textData.textBounds.size, { 0.0f, 0.0f }, { 1.0f, 1.0f }, m_TextColour);

			list.Add(quad.vertices, quad.indices, &entry.textData.texture);

			m_CurrentYOffset = m_CurrentYOffset + m_EntrySize;

			if (entry.directory && entry.children.size() > 0)
			{
				float yCentre = yPos + m_EntrySize / 2.0f;


				Shape tri;

				if (entry.expanded)
				{
					tri = gui::GenerateAlignedEqualTri({ xPos - 18.0f, yCentre - 3.0f }, { xPos - 9.0f, yCentre + 3.0f }, m_TextColour, 2);
				}
				else
				{
					float triHeight = (18.0f - 9.0f) / 2.0f + 0.5f;
					tri = gui::GenerateAlignedEqualTri({ xPos - 18.0f + 4.5f, yCentre - triHeight }, { xPos - 8.0f + 4.5f, yCentre + triHeight }, m_TextColour, 0);
				}
				list.Add(tri.vertices, tri.indices);

			}

			if (entry.expanded && entry.children.size() > 0)
			{

				indents++;
				for (auto& entry : entry.children)
					RenderEntry(list, entry, indents);

				indents--;
			}

			

			
		}

		float m_CurrentYOffset = 0.0f;

		float m_EntrySize = 25.0f;
		float m_IndentSize = 12.0f;
		Font* m_Font;

		Colour m_HoveredColour;
		Colour m_TextColour;

		ListEntry* m_Selected;

		

		std::vector<ListEntry> m_Entries;
	};
}