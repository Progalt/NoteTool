#pragma once

#include "UI/EventHandler.h"


namespace textedit
{
	inline void Remove()
	{ 
		if (gui::EventHandler::selecting)
		{
			uint32_t minSelect = std::min(gui::EventHandler::cursorOffset, gui::EventHandler::selectionStart);
			uint32_t maxSelect = std::max(gui::EventHandler::cursorOffset, gui::EventHandler::selectionStart);
			gui::EventHandler::textInput->erase(minSelect, maxSelect - minSelect);
			gui::EventHandler::cursorOffset = minSelect;
			gui::EventHandler::selectionStart = gui::EventHandler::cursorOffset;
		}
		else
		{
			gui::EventHandler::textInput->erase(gui::EventHandler::cursorOffset - 1, 1);
			gui::EventHandler::cursorOffset--;
			gui::EventHandler::selectionStart = gui::EventHandler::cursorOffset;
		}
	}

	inline void Insert(const std::string& text)
	{
		uint32_t oldSize = gui::EventHandler::textInput->size();
		gui::EventHandler::textInput->insert(gui::EventHandler::cursorOffset, text);
		uint32_t newSize = gui::EventHandler::textInput->size();
		gui::EventHandler::cursorOffset += newSize - oldSize;
		gui::EventHandler::selectionStart = gui::EventHandler::cursorOffset;
	}

	inline std::string GetSelection()
	{
		uint32_t minSelect = std::min(gui::EventHandler::cursorOffset, gui::EventHandler::selectionStart);
		uint32_t maxSelect = std::max(gui::EventHandler::cursorOffset, gui::EventHandler::selectionStart);

		std::string output(gui::EventHandler::textInput->begin() + minSelect, gui::EventHandler::textInput->begin() + maxSelect);

		return output;
	}
}