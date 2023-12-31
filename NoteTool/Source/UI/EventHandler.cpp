
#include "EventHandler.h"

namespace gui
{
	int EventHandler::x = 0;
	int EventHandler::y = 0;
	int EventHandler::xrel = 0;
	int EventHandler::yrel = 0;

	int EventHandler::window_width = 0;
	int EventHandler::window_height = 0;

	int EventHandler::initial_window_width = 0;
	int EventHandler::initial_window_height = 0;

	float EventHandler::deltaTime = 0.0f;

	bool EventHandler::resizeEvent = false;

	int EventHandler::verticalScroll = 0;

	int EventHandler::cursorOffset = 0;
	int EventHandler::selectionOffset = 0;
	bool EventHandler::selecting = false;
	std::string* EventHandler::textInput = nullptr;

	bool EventHandler::backspace = false;
	bool EventHandler::enter = false;

	EventHandler::MouseButton EventHandler::mouseButton[MOUSE_BUTTON_COUNT];
}