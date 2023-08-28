#pragma once
#include <string>


namespace gui
{
	enum MouseButton
	{
		MOUSE_LEFT, 
		MOUSE_RIGHT,

		MOUSE_BUTTON_COUNT
	};

	class EventHandler
	{
	public:



		static int x;
		static int y;
		static int xrel;
		static int yrel;

		static float deltaTime;

		static int window_width;
		static int window_height;

		static int initial_window_width;
		static int initial_window_height;

		static bool resizeEvent;

		static int verticalScroll;

		static std::string* textInput;
		static int cursorOffset;
		static int selectionStart;

		static bool selecting;

		struct MouseButton
		{
			unsigned char clicks = 0;
			bool down;
		};

		static MouseButton mouseButton[MOUSE_BUTTON_COUNT];

	private:
	};
}