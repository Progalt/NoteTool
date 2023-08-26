
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>


#include <SDL2/SDL_syswm.h>


#ifdef _WIN32
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#endif

#include "Platform.h"
#include "Icons.h"


int window_width = 1280;
int window_height = 720;

bool open = true;

#include "Font.h"
#include "Theme.h"

#include "UI/Panel.h"
#include "UI/Dock.h"
#include "UI/Utility.h"
#include "UI/Button.h"
#include "UI/Text.h"
#include "UI/FontManager.h"
#include "UI/DropDown.h"

#include "Vendor/tinyfiledialogs.h"

void ToggleDarkModeForHwnd(SDL_Window* window)
{
	static bool darkMode = false;
	darkMode = !darkMode;

#ifdef _WIN32
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(window, &wmInfo);
	HWND hwnd = wmInfo.info.win.window;


	// https://stackoverflow.com/questions/39261826/change-the-color-of-the-title-bar-caption-of-a-win32-application/70693198#70693198
	// Set the window to a dark theme mode.. Windows 11 allows for more customisation on this but... I have windows 10... so
	// Anyway
	// TODO: Add support for windows 11 border colours
	// Surely this can come from the theme

	BOOL USE_DARK_MODE = darkMode;
	BOOL SET_IMMERSIVE_DARK_MODE_SUCCESS = SUCCEEDED(DwmSetWindowAttribute(
		hwnd, DWMWINDOWATTRIBUTE::DWMWA_USE_IMMERSIVE_DARK_MODE,
		&USE_DARK_MODE, sizeof(USE_DARK_MODE)));

	if (!SET_IMMERSIVE_DARK_MODE_SUCCESS)
		printf("Failed to set dark mode");


	// Hacky way to get it being drawn in dark mode without the window being redrawn due to input
	SDL_HideWindow(window);
	SDL_ShowWindow(window);
#endif
}

gui::Panel* windowPanel;
Renderer renderer;
Theme theme;
SDL_Window* win;
GPUTexture whiteTexture;
Matrix4x4f screen;

void Render()
{

	windowPanel->HandleEvents();

	renderer.SetViewport(0, 0, window_width, window_height);
	renderer.SetScissor(0, 0, window_width, window_height);

	renderer.BeginRenderpass(theme.backgroundColour);



	// Draw the titleBar

	gui::DrawList drawList;

	windowPanel->GenerateVertexList(drawList);

	//gui::RenderText(drawList, "Hello World", &fontRegular, { 400, 200 }, 0.0f, { 1.0f, 1.0f, 1.0f, 1.0f });

	//gui::RenderText(drawList, "BOLD", &fontBold, { 400, 300 }, 0.0f, { 1.0f, 1.0f, 1.0f, 1.0f });

	for (auto& cmd : drawList.drawcalls)
	{
		if (cmd.scissor.IsNull())
			renderer.SetScissor(0, 0, window_width, window_height);
		else 
			renderer.SetScissor(cmd.scissor.x, window_height - cmd.scissor.y - cmd.scissor.h, cmd.scissor.w, cmd.scissor.h);
		renderer.SubmitVertices(drawList.vertices, drawList.indices, screen, cmd.texture != nullptr ? cmd.texture : &whiteTexture, cmd.firstIndex, cmd.indexCount);
	}


	renderer.EndRenderpass();

	SDL_GL_SwapWindow(win);
}

int main(int argc, char* argv)
{

	theme.LoadFromThemeJSON("Themes/dark.json");


	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	win = SDL_CreateWindow("Notes - Workspace/", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

	SDL_SetWindowResizable(win, SDL_TRUE);

	// only enable dark mode for the hwnd if the theme wants it to be
	if (theme.useWindowsDarkTheme)
	{
		ToggleDarkModeForHwnd(win);
	}

	// Init context

	SDL_GLContext ctx;

	ctx = SDL_GL_CreateContext(win);

	// Initialise the rendering

	renderer.Initialise(win);

	// Init base content


	gui::FontManager fontManager;
	fontManager.SetInitialFont("DMSans");
	Font* fontRegular = fontManager.Get(gui::FontWeight::Regular, 12);
	Font* fontBold = fontManager.Get(gui::FontWeight::Bold, 12);


	unsigned char pixels[16] = { 255, 255, 255, 255, 255, 255, 255, 255,255, 255, 255, 255,255, 255, 255, 255 };
	whiteTexture.CreateFromPixels(pixels, 2, 2, GPUFormat::RGBA8);

	Image exitImg;
	exitImg.CreateIcon(exitIcon, 10, 10);

	GPUTexture exitIcon;
	exitIcon.CreateFromImage(exitImg);

	screen.Ortho(0.0f, (float)window_width, (float)window_height, 0.0f, -1.0f, 1.0f);


	FloatRect fullWindowBounds;
	fullWindowBounds.size = { (float)window_width, (float)window_height };
	windowPanel = new gui::Panel();
	windowPanel->SetBounds(fullWindowBounds);
	windowPanel->SetDummyPanel(true);

	Vector2f modalSize = { 500.0f, 450.0f };

	gui::Panel* modal = windowPanel->NewChild<gui::Panel>();
	modal->SetDummyPanel(false);
	modal->SetBounds({ (float)window_width / 2.0f - modalSize.x / 2.0f, (float)window_height / 2.0f - modalSize.y / 2.0f, modalSize.x, modalSize.y});
	modal->SetColour({ 0.03f, 0.03f, 0.03f, 1.0f });
	modal->SetRounding(theme.buttonRounding);
	modal->SetTransparency(1.0f);
	modal->SetAnchor(gui::Anchor::Centre);
	modal->SetTransparency(1.0f);
	

	gui::Text* versionText = modal->NewChild<gui::Text>();
	versionText->SetString("Version 0.01a");
	versionText->SetFont(fontRegular);
	float bounds = gui::GetTextLength(versionText->GetString(), fontRegular);
	versionText->SetPosition({ modal->GetBounds().w / 2.0f - bounds / 2.0f, 160.0f });
	versionText->SetColour({ 0.4f, 0.4f, 0.4f, 1.0f });


	float createButtonY = 210.0f;
	float openButtonY = 280.0f;

	gui::Button* createButton = modal->NewChild<gui::Button>();
	createButton->SetBounds({ 350.0f, createButtonY, 100.0f, 30.0f });
	createButton->SetOnClick([&](void*) { printf("Click"); });
	createButton->SetColour(theme.accentColour);
	createButton->SetHighlightColour(theme.accentHighlight);
	createButton->SetHoveredColour(theme.accentColour + theme.hoverModifier);
	createButton->SetRounding(theme.buttonRounding);
	createButton->SetShadowColour(theme.accentColour);

	createButton->SetText("Create", fontRegular);

	gui::Text* createText = modal->NewChild<gui::Text>();
	createText->SetString("Create new workspace");
	createText->SetFont(fontManager.Get(gui::FontWeight::Regular, 14));
	createText->SetPosition({ 60.0f, createButtonY + 10.0f });
	createText->SetColour({ 0.65f, 0.65f, 0.65f, 1.0f });

	gui::Text* createTextDesc = modal->NewChild<gui::Text>();
	createTextDesc->SetString("Create a new folder and open as workspace");
	createTextDesc->SetFont(fontManager.Get(gui::FontWeight::Regular, 12));
	createTextDesc->SetPosition({ 60.0f, createButtonY + 10.0f + fontManager.Get(gui::FontWeight::Regular, 14)->GetLineSpacing() });
	createTextDesc->SetColour({ 0.4f, 0.4f, 0.4f, 1.0f });

	gui::Button* openButton = modal->NewChild<gui::Button>();
	openButton->SetBounds({ 350.0f, openButtonY, 100.0f, 30.0f });
	openButton->SetOnClick([&](void*) {});
	openButton->SetColour(theme.accentColour);
	openButton->SetHighlightColour(theme.accentHighlight);
	openButton->SetHoveredColour(theme.accentColour + theme.hoverModifier);
	openButton->SetRounding(theme.buttonRounding);
	openButton->SetShadowColour(theme.accentColour);

	openButton->SetText("Open", fontRegular);

	gui::Text* openText = modal->NewChild<gui::Text>();
	openText->SetString("Open folder as workspace");
	openText->SetFont(fontManager.Get(gui::FontWeight::Regular, 14));
	openText->SetPosition({ 60.0f, openButtonY + 10.0f });
	openText->SetColour({ 0.65f, 0.65f, 0.65f, 1.0f });

	gui::Text* openTextDesc = modal->NewChild<gui::Text>();
	openTextDesc->SetString("Open an existing folder as a workspace");
	openTextDesc->SetFont(fontManager.Get(gui::FontWeight::Regular, 12));
	openTextDesc->SetPosition({ 60.0f, openButtonY + 10.0f + fontManager.Get(gui::FontWeight::Regular, 14)->GetLineSpacing() });
	openTextDesc->SetColour({ 0.4f, 0.4f, 0.4f, 1.0f });

	//gui::DropDown* dropDown = modal->NewChild<gui::DropDown>();
	//dropDown->SetBounds({ 60.0f, 350.0f, 350.0f + 100.0f - 60.0f, 30.0f});
	//dropDown->SetColour(theme.accentColour);
	//dropDown->SetHighlightColour(theme.accentHighlight);
	//dropDown->SetShadowColour(theme.accentColour);
	//dropDown->SetHoveredColour(theme.accentColour + theme.hoverModifier);
	//dropDown->SetRounding(theme.buttonRounding);
	//dropDown->SetFont(fontManager.Get(gui::FontWeight::Regular, 12));
	//dropDown->SetList({ "English", "French", "German", "Swedish", "Russian", "Japanese", "Korean", "Chinese", "Spanish", "Dutch", "Norwegian", "Finnish"});

	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 LAST = 0;
	double deltaTime = 0;

	gui::EventHandler::initial_window_width = window_width;
	gui::EventHandler::initial_window_height = window_height;

	SDL_Event evnt;
	while (open)
	{
		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();

		deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency()) * 0.001;

		gui::EventHandler::mouseButton[gui::MOUSE_LEFT].clicks = 0;
		gui::EventHandler::mouseButton[gui::MOUSE_RIGHT].clicks = 0;

		gui::EventHandler::deltaTime = (float)deltaTime;

		gui::EventHandler::resizeEvent = false;
		gui::EventHandler::verticalScroll = 0;

		while (SDL_PollEvent(&evnt))
		{
			switch (evnt.type)
			{
			case SDL_QUIT:
			{
				open = false;
				SDL_Event pushEvent;
				pushEvent.type = SDL_QUIT;
				SDL_PushEvent(&pushEvent);
			}
			break;
			case SDL_WINDOWEVENT:

				switch (evnt.window.event)
				{
				case SDL_WINDOWEVENT_RESIZED:

					gui::EventHandler::resizeEvent = true;

					window_width = evnt.window.data1;
					window_height = evnt.window.data2;

					gui::EventHandler::window_width = window_width;
					gui::EventHandler::window_height = window_height;
					fullWindowBounds.size = { (float)window_width, (float)window_height };

					screen.Ortho(0.0f, (float)window_width, (float)window_height, 0.0f, -1.0f, 1.0f);
					windowPanel->SetBounds(fullWindowBounds);


					break;
				}

				break;
			case SDL_MOUSEMOTION:

				gui::EventHandler::x = evnt.motion.x;
				gui::EventHandler::y = evnt.motion.y;

				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			{
				gui::MouseButton button;
				switch (evnt.button.button)
				{
				case SDL_BUTTON_LEFT:
					button = gui::MOUSE_LEFT;
					break;
				case SDL_BUTTON_RIGHT:
					button = gui::MOUSE_RIGHT;
					break;
				default:
					button = gui::MOUSE_LEFT;
					break;
				}

				gui::EventHandler::mouseButton[button].clicks = (evnt.type == SDL_MOUSEBUTTONDOWN) ? evnt.button.clicks : 0;
				gui::EventHandler::mouseButton[button].down = (evnt.button.state == SDL_PRESSED) ? true : false;
			}
			break;
			case SDL_MOUSEWHEEL:
				gui::EventHandler::verticalScroll = evnt.wheel.y;
				break;
			} 
		}

		Render();
	}

	delete windowPanel;

	renderer.Shutdown();
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}