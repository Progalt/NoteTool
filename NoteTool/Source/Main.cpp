
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>


#include <SDL2/SDL_syswm.h>

#include "OS.h"

#include "Platform.h"
#include "Icons.h"


int window_width = 1280;
int window_height = 720;

bool open = true;

#include "Font.h"
#include "Theme.h"

#include "UI/Panel.h"
#include "UI/Utility.h"
#include "UI/Button.h"
#include "UI/Text.h"
#include "UI/FontManager.h"
#include "UI/DropDown.h"
#include "ModalPopup.h"

#include "Workspace.h"
#include "WorkspaceUI.h"

#include "Vendor/tinyfiledialogs.h"

#include "TextEdit.h"



gui::Panel* windowPanel;
Renderer renderer;
Theme theme;
SDL_Window* win;
GPUTexture whiteTexture;
Matrix4x4f screen;

Workspace currentWorkspace;
WorkspaceUI workspaceUI;

ModalPopup modalPopup;


gui::FontManager fontManager;
gui::FontManager codeFontManager;

// workspace gui 

gui::Panel* workspaceUIPanel;

gui::Panel* filelistArea;
gui::Panel* textArea;
gui::Panel* tabsArea;
gui::Panel* dockArea;

float dockSize = 40.0f;
float borderSize = 1.0f;

void CreatePanelsForWorkspace()
{
	filelistArea->SetBounds({ dockSize , 0.0f, 250.0f, windowPanel->GetBounds().h, });
	filelistArea->SetColour({ 0.02f, 0.02f, 0.02f, 1.0f });
	filelistArea->SetHighlightColour({ 0.04f, 0.04f, 0.04f, 1.0f });
	filelistArea->SetTransparency(1.0f);
	filelistArea->SetAnchor(gui::Anchor::CentreLeft);
	//filelistArea->SetVisible(false);
	filelistArea->SetFlags(gui::PanelFlags::DrawBorder);

	float editableWidth = windowPanel->GetBounds().w - 250.0f;


	textArea->SetBounds({ 250.0f + borderSize + dockSize, 30.0f, editableWidth, windowPanel->GetBounds().h - 30.0f });
	textArea->SetColour(theme.backgroundColour);
	//textArea->SetVisible(false);

	tabsArea->SetBounds({ 250.0f + borderSize + dockSize, 0.0f, editableWidth, 30.0f });
	tabsArea->SetColour({ 0.02f, 0.02f, 0.02f, 1.0f });
	//tabsArea->SetVisible(false);
	tabsArea->SetAnchor(gui::Anchor::TopRight);
	tabsArea->SetLockPosition(true);
	tabsArea->SetHighlightColour({ 0.04f, 0.04f, 0.04f, 1.0f });
	tabsArea->SetFlags(gui::PanelFlags::DrawBorder);

	dockArea->SetBounds({ 0.0f, 0.0f, dockSize,  windowPanel->GetBounds().h });
	dockArea->SetColour({ 0.02f, 0.02f, 0.02f, 1.0f });
	dockArea->SetHighlightColour({ 0.04f, 0.04f, 0.04f, 1.0f });
	//dockArea->SetVisible(false);
	dockArea->SetAnchor(gui::Anchor::BottomLeft);
	dockArea->SetFlags(gui::PanelFlags::DrawBorder);
	dockArea->SetLockPosition(true);

	workspaceUI.SetTextArea(textArea);
	workspaceUI.SetFontManager(&fontManager);
	workspaceUI.SetCodeFontManager(&codeFontManager);



}


void InitDock()
{
	float dockButtonSize = dockSize - 6.0f;

	gui::Button* createNewDirectory = dockArea->NewChild<gui::Button>();

	createNewDirectory->SetAnchor(gui::Anchor::TopLeft);
	createNewDirectory->SetBounds({ 3.0f, dockButtonSize, dockButtonSize, dockButtonSize });
	createNewDirectory->SetOnClick([&](void*) 
		{ 
			// Create a new folder in the selected directory

			std::string basePath = currentWorkspace.GetRoot().path.generic_string();

			if (!workspaceUI.GetSelectedPath().empty())
				basePath = workspaceUI.GetSelectedPath();

			std::string path = basePath + "/New Folder";

			assert(CreateNewFolder(path));

			workspaceUI.Refresh();

		});
	createNewDirectory->SetColour(theme.accentColour);
	createNewDirectory->SetHighlightColour(theme.accentHighlight);
	createNewDirectory->SetHoveredColour(theme.accentColour + theme.hoverModifier);
	createNewDirectory->SetRounding(theme.buttonRounding);
	createNewDirectory->SetShadowColour(theme.accentColour);
	createNewDirectory->SetLockPosition(true);
	createNewDirectory->SetLockSize(true);
	createNewDirectory->SetAnchor(gui::Anchor::TopLeft);

	gui::Button* createNewFile = dockArea->NewChild<gui::Button>();

	
	createNewFile->SetBounds({ 3.0f, dockButtonSize * 2.0f + 3.0f, dockButtonSize, dockButtonSize });
	createNewFile->SetOnClick([&](void*)
		{
			// Create a new file in the selected directory

			std::string basePath = currentWorkspace.GetRoot().path.generic_string();

			if (!workspaceUI.GetSelectedPath().empty())
				basePath = workspaceUI.GetSelectedPath();

			std::string path = basePath + "/NewFile";

			assert(CreateNewFile(path, ".txt"));

			workspaceUI.Refresh();
		});
	createNewFile->SetColour(theme.accentColour);
	createNewFile->SetHighlightColour(theme.accentHighlight);
	createNewFile->SetHoveredColour(theme.accentColour + theme.hoverModifier);
	createNewFile->SetRounding(theme.buttonRounding);
	createNewFile->SetShadowColour(theme.accentColour);
	createNewFile->SetLockPosition(true);
	createNewFile->SetLockSize(true);
	createNewFile->SetAnchor(gui::Anchor::TopLeft);

	gui::Button* settings = dockArea->NewChild<gui::Button>();

	
	settings->SetBounds({ 3.0f, dockArea->GetBounds().h - dockButtonSize - 10.0f, dockButtonSize, dockButtonSize});
	settings->SetOnClick([&](void*)
		{

		});
	settings->SetColour(theme.accentColour);
	settings->SetHighlightColour(theme.accentHighlight);
	settings->SetHoveredColour(theme.accentColour + theme.hoverModifier);
	settings->SetRounding(theme.buttonRounding);
	settings->SetShadowColour(theme.accentColour);
	settings->SetLockSize(true);
	settings->SetAnchor(gui::Anchor::BottomLeft);
}

enum Icons
{
	ICON_FILESYSTEM_ARROW,

	ICON_COUNT
};

GPUTexture icons[ICON_COUNT];

void InitIcons()
{
	Image img;
	img.LoadFromMemory(filesystemarrow, filesystemarrowcnt);

	icons[ICON_FILESYSTEM_ARROW].CreateFromImage(img);
}

void Render()
{

	windowPanel->HandleEvents();

	renderer.SetViewport(0, 0, window_width, window_height);
	renderer.SetScissor(0, 0, window_width, window_height);

	renderer.BeginRenderpass(theme.backgroundColour);

	gui::DrawList drawList;

	windowPanel->GenerateVertexList(drawList);

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

	InitIcons();

	// Init base content



	fontManager.SetInitialFont("DMSans");
	Font* fontRegular = fontManager.Get(gui::FontWeight::Regular, 12);
	Font* fontBold = fontManager.Get(gui::FontWeight::Bold, 12);


	codeFontManager.SetInitialFont("Hack");

	unsigned char pixels[16] = { 255, 255, 255, 255, 255, 255, 255, 255,255, 255, 255, 255,255, 255, 255, 255 };
	whiteTexture.CreateFromPixels(pixels, 2, 2, GPUFormat::RGBA8);



	screen.Ortho(0.0f, (float)window_width, (float)window_height, 0.0f, -1.0f, 1.0f);


	FloatRect fullWindowBounds;
	fullWindowBounds.size = { (float)window_width, (float)window_height };
	windowPanel = new gui::Panel();
	windowPanel->SetBounds(fullWindowBounds);
	windowPanel->SetDummyPanel(true);

	workspaceUIPanel = windowPanel->NewChild<gui::Panel>();

	workspaceUIPanel->SetBounds(fullWindowBounds);
	workspaceUIPanel->SetDummyPanel(true);
	workspaceUIPanel->SetVisible(false);
	//workspaceUIPanel->SetAnchor(gui::Anchor::TopLeft);

	filelistArea = workspaceUIPanel->NewChild<gui::Panel>();
	textArea = workspaceUIPanel->NewChild<gui::Panel>();
	tabsArea = workspaceUIPanel->NewChild<gui::Panel>();
	dockArea = workspaceUIPanel->NewChild<gui::Panel>();

	CreatePanelsForWorkspace();

	Vector2f modalSize = { 500.0f, 450.0f };

	gui::Panel* modal = windowPanel->NewChild<gui::Panel>();
	modal->SetDummyPanel(false);
	modal->SetBounds({ (float)window_width / 2.0f - modalSize.x / 2.0f, (float)window_height / 2.0f - modalSize.y / 2.0f, modalSize.x, modalSize.y});
	modal->SetColour({ 0.02f, 0.02f, 0.02f, 1.0f });
	modal->SetRounding(theme.buttonRounding);
	modal->SetTransparency(1.0f);
	modal->SetAnchor(gui::Anchor::Centre);
	modal->SetTransparency(1.0f);
	modal->SetHighlightColour({ 0.04f, 0.04f, 0.04f, 1.0f });
	modal->SetFlags(gui::PanelFlags::DrawBorder);

	gui::Text* versionText = modal->NewChild<gui::Text>();
	versionText->SetString("Version 0.01d");
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
	createText->SetPosition({ 60.0f, createButtonY });
	createText->SetColour({ 0.65f, 0.65f, 0.65f, 1.0f });

	gui::Text* createTextDesc = modal->NewChild<gui::Text>();
	createTextDesc->SetString("Create a new folder and open as workspace");
	createTextDesc->SetFont(fontManager.Get(gui::FontWeight::Regular, 12));
	createTextDesc->SetPosition({ 60.0f, createButtonY + fontManager.Get(gui::FontWeight::Regular, 14)->GetLineSpacing() });
	createTextDesc->SetColour({ 0.4f, 0.4f, 0.4f, 1.0f });

	gui::Button* openButton = modal->NewChild<gui::Button>();
	openButton->SetBounds({ 350.0f, openButtonY, 100.0f, 30.0f });
	openButton->SetOnClick([&](void*) 
		{ 
			const char* folder = tinyfd_selectFolderDialog("Open Workspace", GetDocumentsPath().c_str());

			if (folder != NULL)
			{
				currentWorkspace.OpenWorkspace(folder);
				if (currentWorkspace.IsValid())
					modal->SetVisible(false);

				CreatePanelsForWorkspace(); 
				InitDock();
				

				workspaceUI.Init(filelistArea, &currentWorkspace, fontRegular);
				/*filelistArea->SetVisible(true);
				textArea->SetVisible(true);
				tabsArea->SetVisible(true);
				dockArea->SetVisible(true);*/

				workspaceUIPanel->SetVisible(true);

				std::string title = "Notes - Workspace/" + currentWorkspace.GetRoot().name;
				SDL_SetWindowTitle(win, title.c_str());
			}
		});
	openButton->SetColour(theme.accentColour);
	openButton->SetHighlightColour(theme.accentHighlight);
	openButton->SetHoveredColour(theme.accentColour + theme.hoverModifier);
	openButton->SetRounding(theme.buttonRounding);
	openButton->SetShadowColour(theme.accentColour);

	openButton->SetText("Open", fontRegular);

	gui::Text* openText = modal->NewChild<gui::Text>();
	openText->SetString("Open folder as workspace");
	openText->SetFont(fontManager.Get(gui::FontWeight::Regular, 14));
	openText->SetPosition({ 60.0f, openButtonY });
	openText->SetColour({ 0.65f, 0.65f, 0.65f, 1.0f });

	gui::Text* openTextDesc = modal->NewChild<gui::Text>();
	openTextDesc->SetString("Open an existing folder as a workspace");
	openTextDesc->SetFont(fontManager.Get(gui::FontWeight::Regular, 12));
	openTextDesc->SetPosition({ 60.0f, openButtonY + fontManager.Get(gui::FontWeight::Regular, 14)->GetLineSpacing() });
	openTextDesc->SetColour({ 0.4f, 0.4f, 0.4f, 1.0f });

	//modal->SetVisible(false);

	/*gui::DropDown* dropDown = modal->NewChild<gui::DropDown>();
	dropDown->SetBounds({ 60.0f, 350.0f, 350.0f + 100.0f - 60.0f, 30.0f});
	dropDown->SetColour(theme.accentColour);
	dropDown->SetHighlightColour(theme.accentHighlight);
	dropDown->SetShadowColour(theme.accentColour);
	dropDown->SetHoveredColour(theme.accentColour + theme.hoverModifier);
	dropDown->SetRounding(theme.buttonRounding);
	dropDown->SetFont(fontManager.Get(gui::FontWeight::Regular, 12));
	dropDown->SetList({ "Plain Text", "Code" });*/


	

	modalPopup.Initialise(windowPanel, &fontManager, (float)window_width / 2.0f);



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

		SDL_StartTextInput();

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
				case SDL_WINDOWEVENT_SIZE_CHANGED:

					printf("Triggering Resize\n");
					gui::EventHandler::resizeEvent = true;

					window_width = evnt.window.data1;
					window_height = evnt.window.data2;

					windowPanel->SetOldBounds(fullWindowBounds);

					gui::EventHandler::window_width = window_width;
					gui::EventHandler::window_height = window_height;
					fullWindowBounds.size = { (float)window_width, (float)window_height };

					screen.Ortho(0.0f, (float)window_width, (float)window_height, 0.0f, -1.0f, 1.0f);
					windowPanel->SetBounds(fullWindowBounds);
					workspaceUIPanel->SetBounds(fullWindowBounds);

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
			case SDL_TEXTINPUT:
				if (gui::EventHandler::textInput)
				{
					/*uint32_t oldSize = gui::EventHandler::textInput->size();
					gui::EventHandler::textInput->insert(gui::EventHandler::cursorOffset, evnt.text.text);
					uint32_t newSize = gui::EventHandler::textInput->size();
					gui::EventHandler::cursorOffset += newSize - oldSize;
					gui::EventHandler::selectionStart = gui::EventHandler::cursorOffset;*/

					textedit::Insert(evnt.text.text);
				}
				break;
			case SDL_KEYDOWN:

				if (gui::EventHandler::textInput)
				{
					if (evnt.key.keysym.sym == SDLK_BACKSPACE && gui::EventHandler::cursorOffset > 0)
					{
						/*if (gui::EventHandler::selecting)
						{
							uint32_t minSelect = std::min(gui::EventHandler::cursorOffset, gui::EventHandler::selectionStart);
							uint32_t maxSelect = std::max(gui::EventHandler::cursorOffset, gui::EventHandler::selectionStart);
							gui::EventHandler::textInput->erase(minSelect, maxSelect - minSelect);
							gui::EventHandler::cursorOffset = minSelect;
						}
						else
						{
							gui::EventHandler::textInput->erase(gui::EventHandler::cursorOffset - 1, 1);
							gui::EventHandler::cursorOffset--;
						}*/

						textedit::Remove();
					}

					if (evnt.key.keysym.sym == SDLK_RETURN)
					{
						/*gui::EventHandler::textInput->insert(gui::EventHandler::cursorOffset, "\n");
						gui::EventHandler::cursorOffset++;*/

						textedit::Insert("\n");
					}

					if (evnt.key.keysym.sym == SDLK_TAB)
					{
						/*gui::EventHandler::textInput->insert(gui::EventHandler::cursorOffset, "\t");
						gui::EventHandler::cursorOffset++;*/

						textedit::Insert("\t");
					}


					if (evnt.key.keysym.sym == SDLK_RIGHT && !evnt.key.keysym.mod & KMOD_LSHIFT)
					{
						if (gui::EventHandler::cursorOffset < gui::EventHandler::textInput->size())
							gui::EventHandler::cursorOffset++;

						gui::EventHandler::selectionStart = gui::EventHandler::cursorOffset;

						gui::EventHandler::selecting = false;
					}


					if (evnt.key.keysym.sym == SDLK_LEFT && evnt.key.keysym.mod & KMOD_LSHIFT)
					{
						if (gui::EventHandler::cursorOffset > 0)
							gui::EventHandler::cursorOffset--;

						gui::EventHandler::selecting = true;
					}
					else if (evnt.key.keysym.sym == SDLK_LEFT)
					{
						if (gui::EventHandler::cursorOffset > 0)
							gui::EventHandler::cursorOffset--;

						gui::EventHandler::selectionStart = gui::EventHandler::cursorOffset;

						gui::EventHandler::selecting = false;
					}


					if (evnt.key.keysym.sym == SDLK_RIGHT && evnt.key.keysym.mod & KMOD_LSHIFT)
					{
						if (gui::EventHandler::cursorOffset < gui::EventHandler::textInput->size())
							gui::EventHandler::cursorOffset++;

						gui::EventHandler::selecting = true;
					}
					else if (evnt.key.keysym.sym == SDLK_RIGHT)
					{
						if (gui::EventHandler::cursorOffset < gui::EventHandler::textInput->size())
							gui::EventHandler::cursorOffset++;

						gui::EventHandler::selectionStart = gui::EventHandler::cursorOffset;

						gui::EventHandler::selecting = false;
					}

				}

				if (evnt.key.keysym.sym == SDLK_s && evnt.key.keysym.mod & KMOD_CTRL)
				{
					printf("Saving...\n");
					workspaceUI.TriggerSave();
					modalPopup.DisplayModal(ModalType::Success, "Successfully saved", (float)window_width / 2.0f);
				}

				if (evnt.key.keysym.sym == SDLK_a && evnt.key.keysym.mod & KMOD_CTRL)
				{
					gui::EventHandler::selectionStart = 0;
					gui::EventHandler::cursorOffset = gui::EventHandler::textInput->size();
					gui::EventHandler::selecting = true;
				}

				// Clipboard options
				if (evnt.key.keysym.sym == SDLK_c && evnt.key.keysym.mod & KMOD_CTRL)
				{
					std::string selectedText = textedit::GetSelection();
					SDL_SetClipboardText(selectedText.c_str());

					printf("Added to clipboard: %s\n", selectedText.c_str());
				}

				if (evnt.key.keysym.sym == SDLK_v && evnt.key.keysym.mod & KMOD_CTRL)
				{
					std::string clipboard(SDL_GetClipboardText());

					textedit::Insert(clipboard);
				}

				

				break;
			} 
		}
		
		modalPopup.ModalUpdate();

		Render();
	}

	delete windowPanel;

	renderer.Shutdown();
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}