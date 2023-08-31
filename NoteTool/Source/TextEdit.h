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

	inline void Insert(const std::string& text)
	{
		if (!gui::EventHandler::textInput)
			return;

		uint32_t oldSize = gui::EventHandler::textInput->size();
		gui::EventHandler::textInput->insert(gui::EventHandler::cursorOffset, text);
		uint32_t newSize = gui::EventHandler::textInput->size();
		gui::EventHandler::cursorOffset += newSize - oldSize;
	}
}