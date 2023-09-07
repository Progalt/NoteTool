#pragma once

#include <SDL2/SDL.h>
#include <string>
#include "Maths/Vector2.h"

enum class WindowFlags
{
	Resizable = 1 << 1, 
	NoTitleBar = 1 << 2,
	OpenGL = 1 << 3
};

inline WindowFlags operator|(WindowFlags lh, WindowFlags rh)
{
	return static_cast<WindowFlags>(
		static_cast<int>(lh ) |
		static_cast<int>(rh)
		);
}

inline bool operator&(WindowFlags lh, WindowFlags rh)
{
	return static_cast<int>(lh) &
		static_cast<int>(rh);
}

class Window
{
public:

	void Create(const std::string& title, const uint32_t initialWidth, const uint32_t intialHeight, WindowFlags flags);

	void Close();

	void SetAsCurrent();

	void HandleWindowEvents(SDL_Event* ev);

	bool IsOpen() const { return m_IsOpen; }

	void Swap();

	// Attribute changing 

	void SetTitle(const std::string& title);

	Vector2u GetSize();

	SDL_Window* GetWindow();

private:


	SDL_Window* m_Window;
	SDL_GLContext m_Context;


	uint32_t m_WindowID = 0;

	uint32_t m_Width = 0;
	uint32_t m_Height = 0;

	bool m_IsOpen;
};