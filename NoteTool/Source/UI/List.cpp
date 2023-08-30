
#include "List.h"

namespace gui
{
	void List::GenerateVertexList(DrawList& drawList)
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

	void List::OnEvent()
	{

		for (auto& entry : m_Entries)
			HandleEventsForEntry(entry);
	}



	void List::HandleEventsForEntry(ListEntry* entry)
	{
		if (!m_Visible)
			return;

		entry->hovered = false;
		if (!entry->boundingBox.IsNull())
		{
			if (entry->boundingBox.Contains((float)EventHandler::x, (float)EventHandler::y))
			{
				entry->hovered = true;
			}
		}

		if (entry->hovered)
		{


			if (EventHandler::mouseButton[MouseButton::MOUSE_LEFT].clicks >= 1)
			{
				if (!entry->directory)
				{
					if (m_Selected)
						m_Selected->selected = false;
					entry->selected = true;

					if (entry->callback)
						entry->callback(entry->userData);

					m_Selected = entry;
				}
				else
				{
					if (m_Selected)
						m_Selected->selected = false;
					entry->selected = true;

					entry->expanded = !entry->expanded;

					m_Selected = entry;
				}
			}

			if (EventHandler::mouseButton[MouseButton::MOUSE_LEFT].down && !m_DraggingEntry)
			{
			}

		}

		if (entry->expanded)
			for (auto& child : entry->children)
				HandleEventsForEntry(child);

	}

	void List::RenderEntry(DrawList& list, ListEntry* entry, uint32_t& indents)
	{
		float xPosNoIndents = m_GlobalBounds.position.x + 24.0f;
		float xPos = xPosNoIndents + (float)indents * m_IndentSize;
		float yPos = m_GlobalBounds.position.y + m_CurrentYOffset;

		entry->boundingBox = { m_GlobalBounds.position.x + 2.0f, yPos,  m_GlobalBounds.w - 10.0f, m_EntrySize };

		if (entry->hovered || entry->selected)
		{
			Shape shape = gui::GenerateRoundedQuad(entry->boundingBox.position, entry->boundingBox.position + entry->boundingBox.size, m_HoveredColour, 4.0f);

			list.Add(shape.vertices, shape.indices);
		}

		for (uint32_t i = 0; i < indents; i++)
		{
			float xPosI = xPosNoIndents + ((float)i - 1.0f) * m_IndentSize;
			float dist = (18.0f - 9.0f) / 2.0f;
			Shape line = gui::GenerateQuad({ xPosI - dist + 3.0f, yPos }, { xPosI - dist + 3.5f, yPos + m_EntrySize }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, m_TextColour);

			list.Add(line.vertices, line.indices);
		}

		float textYPos = yPos + (m_EntrySize / 2.0f) - (float)m_Font->GetMaxHeight() / 2.0f;// + ((float)m_Font->GetPixelSize() / 3.0f);
		//gui::RenderText(list, entry->text, m_Font, { (int)xPos,  (int)textYPos   }, 0.0f, m_TextColour, m_GlobalBounds);

		// Render the text
		if (entry->textData.rerender)
		{
			entry->textData.RasterizeText(entry->text, m_Font);
			entry->textData.rerender = false;
		}

		Vector2f position = { xPos,  textYPos };
		Shape quad = gui::GenerateQuad(position, position + entry->textData.textBounds.size, { 0.0f, 0.0f }, { 1.0f, 1.0f }, m_TextColour);

		list.Add(quad.vertices, quad.indices, &entry->textData.texture);

		m_CurrentYOffset = m_CurrentYOffset + m_EntrySize;

		if (entry->directory && entry->children.size() > 0)
		{
			float yCentre = yPos + m_EntrySize / 2.0f;


			Shape tri;

			if (entry->expanded)
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


		if (entry->children.size() > 0)
		{

			float maxExpansion = GetCurrentSize(entry);

			/*float expansionSpeed = 10.0f;
			if (entry->expanded)
			{
				entry->expandedAmount = gui::Lerp(entry->expandedAmount, maxExpansion, EventHandler::deltaTime * expansionSpeed);
			}
			else
			{
				entry->expandedAmount = gui::Lerp(entry->expandedAmount, 0.0f, EventHandler::deltaTime * expansionSpeed);
				if (std::fabs(entry->expandedAmount) < 2.5f)
					entry->expandedAmount = 0.0f;
			}

			if (entry->expandedAmount > maxExpansion)
				entry->expandedAmount = maxExpansion;*/

				// TODO: This is still a bit glitchy with sub folders of folders so fix that

			if (entry->expanded)
				entry->expandedAmount = maxExpansion;
			else
				entry->expandedAmount = 0.0f;

			indents++;

			IntRect oldScissor = list.GetCurrentScissor();


			IntRect newScissor;
			newScissor.size.x = entry->boundingBox.size.x;
			newScissor.size.y = (int)entry->expandedAmount;
			newScissor.position = entry->boundingBox.position;
			newScissor.position.y = yPos + m_EntrySize;

			list.SetScissor(newScissor);


			if (entry->expandedAmount > 0.0f)
			{
				for (auto& entry : entry->children)
					RenderEntry(list, entry, indents);

				m_CurrentYOffset -= maxExpansion - entry->expandedAmount;
			}

			indents--;

			list.SetScissor(oldScissor);


		}
	}


	float List::GetCurrentSize(ListEntry* list)
	{
		float size = 0.0f;


		size += (float)list->children.size() * m_EntrySize;
		//size -= list.expandedAmount;


		for (auto& child : list->children)
			if (list->expanded)
				size += GetCurrentSize(child);

		return size;
	}

}