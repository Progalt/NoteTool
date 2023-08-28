#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <fstream>
#include <cassert>
#include <filesystem>

#ifdef _WIN32
#include <dwmapi.h>
#include <shlobj.h>
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "shell32.lib")
#endif

#include <string>

inline void ToggleDarkModeForHwnd(SDL_Window* window)
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

inline std::string GetDocumentsPath()
{
#ifdef _WIN32

	CHAR my_documents[MAX_PATH];
	HRESULT result = SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);


	return std::string(my_documents);
#endif

}


inline uint32_t GetNextFreeIndexOfFileName(std::string path, uint32_t& currentIdx)
{
	if (std::filesystem::exists(path + std::to_string(currentIdx)))
	{
		currentIdx++;
		return GetNextFreeIndexOfFileName(path, currentIdx);
	}

	return currentIdx;
}

inline bool CreateNewFolder(const std::string in_path)
{
	std::string path = in_path;

	if (std::filesystem::exists(path))
	{
		uint32_t startIdx = 1;
		uint32_t idx = GetNextFreeIndexOfFileName(path, startIdx);

		path += std::to_string(idx);
	}

	std::filesystem::create_directory(path);

	return true;
}

inline bool CreateNewFile(const std::string in_path, const std::string ext)
{
	std::string path = in_path;

	if (std::filesystem::exists(path))
	{
		uint32_t startIdx = 1;
		uint32_t idx = GetNextFreeIndexOfFileName(path, startIdx);

		path += std::to_string(idx);
	}

	path += ext;


	std::ofstream file(path);
	
	if (!file.is_open())
		return false;

	file << "";
	file.close();

	return true;
}