#pragma once

#include "Panel.h"
#include "Button.h"
#include "EventHandler.h"
#include "../Image.h"

namespace gui
{

	struct ListEntry
	{
		ListEntry() { }
		ListEntry(const std::string& text, std::function<void(void*)> callback, void* userData = nullptr, bool directory = false) : text(text), callback(callback), userData(userData), directory(directory) { textData.rerender = true; }

		void AddChild(ListEntry* entry) { 
			children.push_back(entry); 

			if (entry->parent)
				entry->parent->RemoveChild(entry);

			children[children.size() - 1]->parent = this;
		}

		void RemoveChild(ListEntry* entry)
		{
			for (uint32_t i = 0; i < children.size(); i++)
				if (children[i] == entry)
					children.erase(children.begin() + i);
		}

		std::string text;
		std::function<void(void*)> callback;
		void* userData = nullptr;

		ListEntry* parent = nullptr;
		std::vector<ListEntry*> children;

		bool directory = false;

		FloatRect boundingBox;
		bool hovered = false;
		bool expanded = true;
		bool selected = false;

		float expandedAmount = 0.0f;

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


		~List()
		{
			for (auto& a : m_AllocatedListEntries)
				delete a;
		}

		ListEntry* NewListEntry(const std::string& text, std::function<void(void*)> callback, void* userData = nullptr, bool directory = false)
		{
			m_AllocatedListEntries.push_back(new ListEntry(text, callback, userData, directory));

			return m_AllocatedListEntries[m_AllocatedListEntries.size() - 1];
		}


		void AddEntry(ListEntry* entry)
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

		void GenerateVertexList(DrawList& drawList) override;

		void OnEvent() override;

	private:

		void HandleEventsForEntry(ListEntry* entry);

		void RenderEntry(DrawList& list, ListEntry* entry, uint32_t& indents);

		float GetCurrentSize(ListEntry* list);


		float m_CurrentYOffset = 0.0f;

		float m_EntrySize = 25.0f;
		float m_IndentSize = 12.0f;
		Font* m_Font;

		Colour m_HoveredColour;
		Colour m_TextColour;

		ListEntry* m_Selected;

		bool m_DraggingEntry = false;
		ListEntry* m_Payload;

		std::vector<ListEntry*> m_AllocatedListEntries;

		std::vector<ListEntry*> m_Entries;
	};
}