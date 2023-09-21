

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>


#include <SDL2/SDL_syswm.h>

#include "OS.h"

#include "Platform.h"

#include "UI/IconManager.h"


#include "Font.h"
#include "Theme.h"

#include "UI/Panel.h"
#include "UI/Utility.h"
#include "UI/Button.h"
#include "UI/Text.h"
#include "UI/FontManager.h"
#include "UI/DropDown.h"
#include "ModalPopup.h"
#include "UI/ContextMenu.h"

#include "Workspace.h"
#include "WorkspaceUI.h"

#include "Vendor/tinyfiledialogs.h"

#include "TextEdit.h"
#include "UserPrefs.h"
#include "Window.h"

int window_width = 1280;
int window_height = 720;

const std::string versionString = "v0.01d";

gui::Panel* windowPanel;
Renderer renderer;
Theme theme;
GPUTexture whiteTexture;
Matrix4x4f screen;

Window win;

Workspace currentWorkspace;
WorkspaceUI workspaceUI;


UserPrefs userPrefs;


gui::FontManager fontManager;
gui::FontManager codeFontManager;

// workspace gui 

gui::Panel* workspaceUIPanel;

gui::Panel* filelistArea;
gui::Panel* textArea;
gui::Panel* tabsArea;
gui::Panel* modal;

float borderSize = 1.0f;
float fileBrowserUISize = 300.0f;

const std::filesystem::path m_BaseWorkspacePath = GetDocumentsPath() /  "Workspaces";
const std::filesystem::path m_UserPrefsPath = GetAppDataPath() / "NotesTool";

void CreatePanelsForWorkspace()
{
	float padding = 0.0f;
	filelistArea->SetBounds({ padding , padding, fileBrowserUISize - padding * 2.0f, windowPanel->GetBounds().h - padding * 2.0f });
	filelistArea->SetColour(theme.panelBackground);
	filelistArea->SetHighlightColour(theme.panelHighlight);
	filelistArea->SetTransparency(1.0f);
	filelistArea->SetAnchor(gui::Anchor::CentreLeft);
	//filelistArea->SetVisible(false);
	filelistArea->SetFlags(gui::PanelFlags::DrawBorder);
	filelistArea->SetRounding(0.0f);

	float editableWidth = windowPanel->GetBounds().w - filelistArea->GetBounds().w - 1.0f;


	float rounding = 0.0f;
	textArea->SetBounds({ fileBrowserUISize + borderSize, padding, editableWidth - padding * 3.0f, windowPanel->GetBounds().h - padding * 2.0f });
	textArea->SetColour(theme.panelBackground);
	textArea->SetRounding(rounding);
	textArea->SetDummyPanel(true);
	//textArea->SetHighlightColour({ 0.4f, 0.4f, 0.4f, 1.0f });
	//textArea->SetVisible(false);


	workspaceUI.SetTextArea(textArea);
	workspaceUI.SetFontManager(&fontManager);
	workspaceUI.SetCodeFontManager(&codeFontManager);



}



void Render()
{
	

	//PROFILE_BEGIN(events);
	windowPanel->HandleEvents();
	//PROFILE_END(events, "Events");
	

	gui::DrawList drawList;

	//PROFILE_BEGIN(vertexLists);
	windowPanel->GenerateVertexList(drawList);
	//PROFILE_END(vertexLists, "Vertex List");

	//PROFILE_BEGIN(render);

	renderer.SetViewport(0, 0, window_width, window_height);
	renderer.SetScissor(0, 0, window_width, window_height);

	renderer.BeginRenderpass(theme.backgroundColour);

	renderer.CopyToBuffers(drawList.vertices, drawList.indices);
	
	for (auto& cmd : drawList.drawcalls)
	{
		if (cmd.scissor.IsNull())
			renderer.SetScissor(0, 0, window_width, window_height);
		else 
			renderer.SetScissor(cmd.scissor.x, window_height - cmd.scissor.y - cmd.scissor.h, cmd.scissor.w, cmd.scissor.h);
		//renderer.SubmitVertices(drawList.vertices, drawList.indices, screen, cmd.texture != nullptr ? cmd.texture : &whiteTexture, cmd.firstIndex, cmd.indexCount);

		renderer.Draw(screen, cmd.texture != nullptr ? cmd.texture : &whiteTexture, cmd.firstIndex, cmd.indexCount, 0);

	}


	renderer.EndRenderpass();

	//PROFILE_END(render, "Render");

	win.Swap();

	
}

void OpenWorkspace(const std::string path)
{
	currentWorkspace.OpenWorkspace(path);
	if (currentWorkspace.IsValid())
		modal->SetVisible(false);

	CreatePanelsForWorkspace();


	workspaceUI.Init(filelistArea, &currentWorkspace, fontManager.Get(gui::FontWeight::Regular, 12));

	workspaceUIPanel->SetVisible(true);

	std::string title = "Notes " + versionString + " - Editing " + currentWorkspace.GetRoot().name;
	
	win.SetTitle(title);

	userPrefs.AddRecentlyOpened(
		{
			currentWorkspace.GetRoot().name,
			path
		}
	);

	workspaceUI.Refresh();
}

int main(int argc, char* argv)
{

	userPrefs.LoadFromJSON(m_UserPrefsPath / "userprefs.json");


	window_width = userPrefs.lastOpenedWidth;
	window_height = userPrefs.lastOpenedHeight;

	// TODO: Move themes to appdata area
	theme.LoadFromThemeJSON("Themes/dark.json");
	workspaceUI.SetTheme(&theme);

	SDL_Init(SDL_INIT_EVERYTHING);

	std::string title = "Notes " + versionString;
	win.Create(title, window_width, window_height, WindowFlags::Resizable | WindowFlags::OpenGL);

	OS::GetInstance().InitCursors();

	// only enable dark mode for the hwnd if the theme wants it to be
	if (theme.useWindowsDarkTheme)
	{
		ToggleDarkModeForHwnd(win.GetWindow());
	}



	renderer.Initialise(win.GetWindow());


	// Init base content

	IconManager::GetInstance().LoadIcons();


	fontManager.SetInitialFont("DMSans");
	Font* fontRegular = fontManager.Get(gui::FontWeight::Regular, 12);
	Font* fontBold = fontManager.Get(gui::FontWeight::Bold, 12);


	codeFontManager.SetInitialFont("SourceCodePro");

	// Create a plain white texture
	// I just use a 2x2 image
	unsigned char pixels[16] = { 255, 255, 255, 255, 255, 255, 255, 255,255, 255, 255, 255,255, 255, 255, 255 };
	whiteTexture.CreateFromPixels(pixels, 2, 2, GPUFormat::RGBA8);


	// Setup screen matrix
	screen.Ortho(0.0f, (float)window_width, (float)window_height, 0.0f, -1.0f, 1.0f);

	// We use one big window panel for the GUI
	// Its a dummy panel so no rendering just events
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

	textArea = workspaceUIPanel->NewChild<gui::Panel>();
	filelistArea = workspaceUIPanel->NewChild<gui::Panel>();

	CreatePanelsForWorkspace();

	Vector2f modalSize = { 800.0f, 500.0f };

	// TODO: Move this start up modal somewhere else for init

	modal = windowPanel->NewChild<gui::Panel>();
	modal->SetDummyPanel(false);
	modal->SetBounds({ (float)window_width / 2.0f - modalSize.x / 2.0f, (float)window_height / 2.0f - modalSize.y / 2.0f, modalSize.x, modalSize.y });
	modal->SetColour({ theme.panelBackground });
	modal->SetRounding(theme.buttonRounding);
	modal->SetTransparency(1.0f);
	modal->SetAnchor(gui::Anchor::Centre);
	modal->SetTransparency(1.0f);
	modal->SetHighlightColour({ theme.panelHighlight });
	modal->SetFlags(gui::PanelFlags::DrawBorder);

	gui::Panel* oldWorkspaces = modal->NewChild<gui::Panel>();
	oldWorkspaces->SetBounds({ 0.0f, 0.0f, 300.0f, 500.0f });
	oldWorkspaces->SetColour(theme.backgroundColour);
	oldWorkspaces->SetHighlightColour(theme.panelHighlight);
	oldWorkspaces->SetFlags(gui::PanelFlags::DrawBorder);
	oldWorkspaces->SetRounding(theme.buttonRounding);

	float workspaceOffset = 5.0f;
	
	uint32_t i = userPrefs.recentlyOpenWorkspaces.size();
	for ( i > 0; i--;)
	{
		WorkspacePath& workspace = userPrefs.recentlyOpenWorkspaces[i];
		gui::Button* button = oldWorkspaces->NewChild<gui::Button>();
		button->SetBounds({ 5.0f, workspaceOffset, 290.0f, 70.0f });
		button->SetColour({ theme.backgroundColour });
		button->SetHoveredColour({ theme.panelBackground });
		button->SetRounding(6.0f);
		button->SetUserData(&workspace.path);
		button->SetOnClick([&](void* userData) { OpenWorkspace(*(std::string*)userData); });

		gui::Text* name = oldWorkspaces->NewChild<gui::Text>();
		name->SetString(workspace.name);
		name->SetColour(theme.textMain);
		name->SetFont(fontRegular);
		name->SetPosition({ 10.0f, workspaceOffset + 5.0f });

		gui::Text* path = oldWorkspaces->NewChild<gui::Text>();

		std::string pathStr = workspace.path;
		
		if (pathStr.size() > 35)
		{
			pathStr = pathStr.substr(0, 35);
			pathStr += "...";
		}

		path->SetString(pathStr);
		path->SetColour(theme.textSub);
		path->SetFont(fontRegular);
		path->SetPosition({ 10.0f, workspaceOffset + fontRegular->GetMaxHeight() + 9.0f });

		gui::Text* timestamp = oldWorkspaces->NewChild<gui::Text>();
		timestamp->SetString("Last Opened: " + workspace.time);
		timestamp->SetColour(theme.textSub);
		timestamp->SetFont(fontRegular);
		timestamp->SetPosition({ 10.0f, workspaceOffset + fontRegular->GetMaxHeight() * 2.0f + 13.0f });

		workspaceOffset += 70.0f;
	}

	gui::Panel* userArea = modal->NewChild<gui::Panel>();
	userArea->SetDummyPanel(true);
	userArea->SetBounds({ 300.0f, 0.0f, 500.0f, 500.0f });

	gui::Text* versionText = userArea->NewChild<gui::Text>();
	versionText->SetString("Version 0.01d");
	versionText->SetFont(fontRegular);
	float bounds = gui::GetTextLength(versionText->GetString(), fontRegular);
	versionText->SetPosition({ userArea->GetBounds().w / 2.0f - bounds / 2.0f, 160.0f });
	versionText->SetColour(theme.textSub);


	float createButtonY = 210.0f;
	float openButtonY = 280.0f;

	gui::Button* createButton = userArea->NewChild<gui::Button>();
	createButton->SetBounds({ 350.0f, createButtonY, 100.0f, 30.0f });
	createButton->SetOnClick([&](void*) { OS::GetInstance().DebugPrint("Click"); });
	createButton->SetColour(theme.accentColour);
	createButton->SetHighlightColour(theme.accentHighlight);
	createButton->SetHoveredColour(theme.accentColour + theme.hoverModifier);
	createButton->SetRounding(theme.buttonRounding);
	createButton->SetShadowColour(theme.accentColour);

	createButton->SetText("Create", fontRegular);

	gui::Text* createText = userArea->NewChild<gui::Text>();
	createText->SetString("Create new workspace");
	createText->SetFont(fontManager.Get(gui::FontWeight::Regular, 14));
	createText->SetPosition({ 60.0f, createButtonY });
	createText->SetColour(theme.textMain);

	gui::Text* createTextDesc = userArea->NewChild<gui::Text>();
	createTextDesc->SetString("Create a new folder and open as workspace");
	createTextDesc->SetFont(fontManager.Get(gui::FontWeight::Regular, 12));
	createTextDesc->SetPosition({ 60.0f, createButtonY + fontManager.Get(gui::FontWeight::Regular, 14)->GetLineSpacing() });
	createTextDesc->SetColour(theme.textSub);

	gui::Button* openButton = userArea->NewChild<gui::Button>();
	openButton->SetBounds({ 350.0f, openButtonY, 100.0f, 30.0f });
	openButton->SetOnClick([&](void*)
		{
			std::string documentsPath = GetDocumentsPath().string();
			const char* folder = tinyfd_selectFolderDialog("Open Workspace", documentsPath.c_str());

			if (folder != NULL)
			{
				OpenWorkspace(folder);
			}
		});
	openButton->SetColour(theme.accentColour);
	openButton->SetHighlightColour(theme.accentHighlight);
	openButton->SetHoveredColour(theme.accentColour + theme.hoverModifier);
	openButton->SetRounding(theme.buttonRounding);
	openButton->SetShadowColour(theme.accentColour);

	openButton->SetText("Open", fontRegular);

	gui::Text* openText = userArea->NewChild<gui::Text>();
	openText->SetString("Open folder as workspace");
	openText->SetFont(fontManager.Get(gui::FontWeight::Regular, 14));
	openText->SetPosition({ 60.0f, openButtonY });
	openText->SetColour(theme.textMain);

	gui::Text* openTextDesc = userArea->NewChild<gui::Text>();
	openTextDesc->SetString("Open an existing folder as a workspace");
	openTextDesc->SetFont(fontManager.Get(gui::FontWeight::Regular, 12));
	openTextDesc->SetPosition({ 60.0f, openButtonY + fontManager.Get(gui::FontWeight::Regular, 14)->GetLineSpacing() });
	openTextDesc->SetColour(theme.textSub);

	float settingsButtonSize = 24.0f;
	gui::Button* settingsButton = userArea->NewChild<gui::Button>();
	settingsButton->SetBounds({ userArea->GetBounds().w - settingsButtonSize - 5.0f,  userArea->GetBounds().h - settingsButtonSize - 5.0f, settingsButtonSize, settingsButtonSize });
	settingsButton->SetColour(theme.panelBackground);
	settingsButton->SetHighlightColour(theme.accentColour);
	settingsButton->SetRounding(12.0f);

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


	gui::ContextMenu* ctxMenu = filelistArea->GetContextMenu();
	ctxMenu->AddOption("New Note", [&]() { OS::GetInstance().DebugPrint("New Note"); 
		
			// Create a new file in the selected directory

			std::string basePath = currentWorkspace.GetRoot().path.generic_string();

			OS::GetInstance().DebugPrint("Attempt new note\n");

			//if (!workspaceUI.GetSelectedPath().empty())
			//	basePath = workspaceUI.GetSelectedPath();

			std::string path = basePath + "/NewFile";

			bool success = CreateNewFile(path, ".md");

			if (!success)
			{
				OS::GetInstance().DebugPrint("Failed to create new note\n");
			}

			workspaceUI.Refresh();

		
		});
	ctxMenu->AddOption("New Collection", [&]() { // Create a new folder in the selected directory

		std::string basePath = currentWorkspace.GetRoot().path.generic_string();

		if (!workspaceUI.GetSelectedPath().empty())
			basePath = workspaceUI.GetSelectedPath();

		std::string path = basePath + "/New Folder";

		CreateNewFolder(path);

		workspaceUI.Refresh(); });

	ctxMenu->SetFont(fontManager.Get(gui::FontWeight::Regular, 12));
	ctxMenu->SetColour(theme.panelBackground);
	ctxMenu->SetBorderColour(theme.panelHighlight);
	ctxMenu->SetPosition({ 100.0f, 100.0f });
	ctxMenu->SetRounding(theme.buttonRounding);
	


	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 LAST = 0;
	double deltaTime = 0;

	gui::EventHandler::initial_window_width = window_width;
	gui::EventHandler::initial_window_height = window_height;


	gui::EventHandler::mouseButton[gui::MOUSE_LEFT].down = false;
	gui::EventHandler::mouseButton[gui::MOUSE_RIGHT].down = false;
	
	

	SDL_Event evnt;
	while (win.IsOpen())
	{
		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();

		deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency()) * 0.001;

		gui::EventHandler::mouseButton[gui::MOUSE_LEFT].clicks = 0;
		gui::EventHandler::mouseButton[gui::MOUSE_RIGHT].clicks = 0;


		gui::EventHandler::deltaTime = (float)deltaTime;

		gui::EventHandler::resizeEvent = false;
		gui::EventHandler::verticalScroll = 0;
		gui::EventHandler::backspace = false;
		gui::EventHandler::enter = false;

		SDL_StartTextInput();

		OS::GetInstance().SetCursor(CursorType::Pointer);

		while (SDL_PollEvent(&evnt))
		{
			win.HandleWindowEvents(&evnt);


			switch (evnt.type)
			{
		
			case SDL_WINDOWEVENT:

				switch (evnt.window.event)
				{
				case SDL_WINDOWEVENT_SIZE_CHANGED:

					OS::GetInstance().DebugPrint("Triggering Resize\n");
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
				gui::EventHandler::mouseButton[button].down = true;
			}

			break; 
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

				gui::EventHandler::mouseButton[button].down = false;
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
					if (evnt.key.keysym.sym == SDLK_BACKSPACE)
					{
						gui::EventHandler::backspace = true;
					}
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

						if (gui::EventHandler::selecting)
						{
							textedit::Remove(gui::EventHandler::selectionOffset, gui::EventHandler::cursorOffset);
						}
						else 
							textedit::Remove();
					}

					

					if (evnt.key.keysym.sym == SDLK_RETURN)
					{
						textedit::Insert("\n");
						gui::EventHandler::enter = true;
					}
					
					if (evnt.key.keysym.sym == SDLK_TAB)
					{
						textedit::Insert("\t");

					}


					if (evnt.key.keysym.sym == SDLK_LEFT)
					{
						if (evnt.key.keysym.mod & KMOD_SHIFT)
						{
							if (!gui::EventHandler::selecting)
							{
								gui::EventHandler::selectionOffset = gui::EventHandler::cursorOffset;
								gui::EventHandler::selecting = true;
							}

							if (gui::EventHandler::cursorOffset > 0)
								gui::EventHandler::cursorOffset--;

							OS::GetInstance().DebugPrint("Selecting Offset: %d\n", gui::EventHandler::selectionOffset - gui::EventHandler::cursorOffset);
						}
						else
						{
	
							if (gui::EventHandler::cursorOffset > 0)
								gui::EventHandler::cursorOffset--;

							gui::EventHandler::selecting = false;
						}
						
					}

					 
					if (evnt.key.keysym.sym == SDLK_RIGHT)
					{
						uint32_t offset = 1;

						if (evnt.key.keysym.mod & KMOD_SHIFT)
						{
							if (!gui::EventHandler::selecting)
							{
								gui::EventHandler::selectionOffset = gui::EventHandler::cursorOffset;
								gui::EventHandler::selecting = true;
							}

							if (gui::EventHandler::cursorOffset + offset - 1 < gui::EventHandler::textInput->size())
								gui::EventHandler::cursorOffset += offset;

							OS::GetInstance().DebugPrint("Selecting Offset: %d\n", gui::EventHandler::selectionOffset - gui::EventHandler::cursorOffset);
						}
						else
						{
							if (gui::EventHandler::selecting && gui::EventHandler::selectionOffset > gui::EventHandler::cursorOffset)
								gui::EventHandler::cursorOffset = gui::EventHandler::selectionOffset;
							else if (gui::EventHandler::cursorOffset + offset - 1 < gui::EventHandler::textInput->size())
								gui::EventHandler::cursorOffset += offset;

							gui::EventHandler::selecting = false;
						}

					}

				}

				if (evnt.key.keysym.sym == SDLK_s && evnt.key.keysym.mod & KMOD_CTRL)
				{
					OS::GetInstance().DebugPrint("Saving...\n");
					workspaceUI.TriggerSave();
				}



				// Clipboard options
				if (evnt.key.keysym.sym == SDLK_c && evnt.key.keysym.mod & KMOD_CTRL)
				{
					std::string selectedText = textedit::GetSelection();
					//SDL_SetClipboardText(selectedText.c_str());

					OS::GetInstance().SetClipboardContents(selectedText);

					OS::GetInstance().DebugPrint("Added to clipboard: %s\n", selectedText.c_str());
				}


				if (evnt.key.keysym.sym == SDLK_v && evnt.key.keysym.mod & KMOD_CTRL)
				{
					std::string clipboard(SDL_GetClipboardText());

					textedit::Insert(clipboard);
				}

				
				

				break;
			} 
		}
		

		Render();
	}

	delete windowPanel;

	renderer.Shutdown();
	
	win.Close();

	OS::GetInstance().DestroyCursors();

	SDL_Quit();

	currentWorkspace.Close();

	userPrefs.lastOpenedWidth = window_width;
	userPrefs.lastOpenedHeight = window_height;

	// If the userprefs path doesn't exist we want to make it otherwise we can't save the json
	if (!std::filesystem::exists(m_UserPrefsPath))
		std::filesystem::create_directory(m_UserPrefsPath);

	userPrefs.SaveToJSON(m_UserPrefsPath / "userprefs.json");

	return 0;
}