
#include "Window.h"

void Window::Create(const std::string& title, const uint32_t initialWidth, const uint32_t intialHeight, WindowFlags flags)
{
	

	int winFlags = SDL_WINDOW_SHOWN;

	if (flags & WindowFlags::OpenGL)
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

		winFlags |= SDL_WINDOW_OPENGL;
	}

	if (flags & WindowFlags::Resizable)
		winFlags |= SDL_WINDOW_RESIZABLE;

	m_Window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, initialWidth, intialHeight, winFlags);

	if (!m_Window)
	{
		// Failed to create window

		printf("Failed to create window\n");

		return;
	}

	// Grab the window ID and store it so we can filter events later on
	m_WindowID = SDL_GetWindowID(m_Window);

	if (flags & WindowFlags::OpenGL)
	{
		m_Context = SDL_GL_CreateContext(m_Window);

		if (!m_Context)
		{
			// Failed to create OpenGL Context

			printf("Failed to create OpenGL Context\n");

			return;
		}
	}

	// Get the window size
	// Some flags might change the size when it is opened
	int w, h;
	SDL_GetWindowSizeInPixels(m_Window, &w, &h);

	m_Width = static_cast<uint32_t>(w);
	m_Height = static_cast<uint32_t>(h);

	m_IsOpen = true;

	// Set the current context as current
	SetAsCurrent();
}

void Window::Close()
{
	if (m_Window)
	{
		if (m_Context)
			SDL_GL_DeleteContext(m_Context);

		SDL_DestroyWindow(m_Window);
	}
}

void Window::SetAsCurrent()
{
	if (m_Context)
		SDL_GL_MakeCurrent(m_Window, m_Context);
}

void Window::HandleWindowEvents(SDL_Event* ev)
{
	if (ev->type == SDL_WINDOWEVENT)
	{
		// Filter out the events for other windows
		if (ev->window.windowID != m_WindowID)
			return;
	}
}

void Window::SetTitle(const std::string& title)
{
	SDL_SetWindowTitle(m_Window, title.c_str());

}

Vector2u Window::GetSize()
{
	return { m_Width, m_Height };
}

SDL_Window* Window::GetWindow()
{
	return m_Window;
}

void Window::Swap()
{
	SDL_GL_SwapWindow(m_Window);
}