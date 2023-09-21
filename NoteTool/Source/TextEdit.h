#pragma once

#include "UI/EventHandler.h"


namespace textedit
{
	inline void Remove()
	{
		if (!gui::EventHandler::textInput)
			return;


		gui::EventHandler::textInput->erase(gui::EventHandler::cursorOffset - 1, 1);
		gui::EventHandler::cursorOffset--;

	}

	inline void Remove(uint32_t start, uint32_t end)
	{
		if (!gui::EventHandler::textInput)
			return;

		uint32_t minSelection = std::min(start, end);
		uint32_t maxSelection = std::max(start, end);

		gui::EventHandler::textInput->erase(minSelection, maxSelection - minSelection);
		gui::EventHandler::cursorOffset = minSelection;
	}

	inline void Insert(const std::string& text)
	{
		if (!gui::EventHandler::textInput)
			return;

		uint32_t oldSize = gui::EventHandler::textInput->size();
		gui::EventHandler::textInput->insert(gui::EventHandler::cursorOffset, text);
		uint32_t newSize = gui::EventHandler::textInput->size();
		gui::EventHandler::cursorOffset += newSize - oldSize;
	}

	inline std::string GetSelection()
	{
		uint32_t minSelection = std::min(gui::EventHandler::selectionOffset, gui::EventHandler::cursorOffset);
		uint32_t maxSelection = std::max(gui::EventHandler::selectionOffset, gui::EventHandler::cursorOffset);

		std::string str = gui::EventHandler::textInput->substr(minSelection, maxSelection - minSelection);

		return str;
	}
}