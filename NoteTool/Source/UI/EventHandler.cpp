
#include "EventHandler.h"

namespace gui
{
	int EventHandler::x = 0;
	int EventHandler::y = 0;
	int EventHandler::xrel = 0;
	int EventHandler::yrel = 0;

	EventHandler::MouseButton EventHandler::mouseButton[MOUSE_BUTTON_COUNT];
}