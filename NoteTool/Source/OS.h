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

enum class CursorType
{
	Pointer,
	Hand,
	IBeam,

	Count
};

class OS
{
public:

	static OS& GetInstance()
	{
		static OS instance;

		return instance;
	}

	OS(OS const&) = delete;
	void operator=(OS const&) = delete;

	void InitCursors()
	{
		m_Cursors[(int)CursorType::Pointer] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
		m_Cursors[(int)CursorType::Hand] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
		m_Cursors[(int)CursorType::IBeam] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
	}

	void DestroyCursors()
	{
		for (uint32_t i = 0; i < (uint32_t)CursorType::Count; i++)
			SDL_FreeCursor(m_Cursors[i]);
	}

	void SetCursor(CursorType type)
	{
		if (m_Cursors[(int)type] != m_CurrentCursor)
		{
			SDL_SetCursor(m_Cursors[(int)type]);
			m_CurrentCursor = m_Cursors[(int)type];
		}
	}

	template<typename ... Args>
	void DebugPrint(const char* fmt, Args... args)
	{
#ifdef _DEBUG

		printf(fmt, args...);

#endif
	}

	void SetClipboardContents(const std::string& str)
	{
		SDL_SetClipboardText(str.c_str());
	}


private:
	OS() { }


	SDL_Cursor* m_Cursors[(int)CursorType::Count];
	SDL_Cursor* m_CurrentCursor = nullptr;

};

// On some Operating systems dark mode can be toggled for the window
// Toggle it if available
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
		OS::GetInstance().DebugPrint("Failed to set dark mode");


	// Hacky way to get it being drawn in dark mode without the window being redrawn due to input
	SDL_HideWindow(window);
	SDL_ShowWindow(window);
#endif
}

// Get the documents folder path for the current system
inline std::filesystem::path GetDocumentsPath()
{
#if 0
#ifdef _WIN32

	CHAR my_documents[MAX_PATH];
	HRESULT result = SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);


	return std::string(my_documents);
#endif
#else
	std::filesystem::path path;

#ifdef _WIN32


	PWSTR pathTemp;

	if (!SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &pathTemp)))
	{
		CoTaskMemFree(pathTemp);
		OS::GetInstance().DebugPrint("Failed to find documents path for system\n");
		return std::filesystem::path();
	}
	path = pathTemp;

	CoTaskMemFree(pathTemp);

#endif

	return path;
#endif
}


// This gets the next free filename index of the file name
// For instance if "new folder" exists it will go to "new folder1" and so on
inline uint32_t GetNextFreeIndexOfFileName(std::string path, uint32_t& currentIdx)
{
	if (std::filesystem::exists(path + std::to_string(currentIdx)))
	{
		currentIdx++;
		return GetNextFreeIndexOfFileName(path, currentIdx);
	}

	return currentIdx;
}

// Creates a new folder in the file system
// It first checks if the name of the file exists already and if it does it adds an index to 
// the end of the directory name
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

// Similar to the CreateNewFolder but creates a file instead
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

	OS::GetInstance().DebugPrint("Trying to create file: %s\n", path.c_str());

	// std::filesystem doesn't have the ability to create a new file so we use std::ofstream 
	// and write a blank file out
	std::ofstream file(path);
	
	if (!file.is_open())
	{
		OS::GetInstance().DebugPrint("Failed to create file: %s\n", path.c_str());
		return false;
	}

	file << "";
	file.close();

	return true;
}

inline void OpenURL(const std::string& url)
{
	SDL_OpenURL(url.c_str());
}

inline std::filesystem::path GetAppDataPath()
{
	std::filesystem::path path;

#ifdef _WIN32

	
	PWSTR pathTemp;

	if (!SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &pathTemp)))
	{
		CoTaskMemFree(pathTemp);
		OS::GetInstance().DebugPrint("Failed to find appdata path for system\n");
		return std::filesystem::path();
	}
	path = pathTemp;

	CoTaskMemFree(pathTemp);

#endif

	return path;
}

#if 0 
#ifdef _WIN32
static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{

	if (uMsg == BFFM_INITIALIZED)
	{
		std::string tmp = (const char*)lpData;
		printf("path: %s\n", tmp); 
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
	}

	return 0;
}
#endif

inline std::filesystem::path BrowseForFolder(const std::string& title, const std::filesystem::path& savedPath)
{
#ifdef _WIN32
	TCHAR path[MAX_PATH];

	const char* path_param = savedPath.generic_string().c_str();

	BROWSEINFOA bi = { 0 };
	bi.lpszTitle = title.c_str();
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	bi.lpfn = BrowseCallbackProc;
	bi.lParam = (LPARAM)path_param;

	LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);

	if (pidl != 0)
	{
		//get the name of the folder and put it in path
		SHGetPathFromIDList(pidl, path);

		//free memory used
		IMalloc* imalloc = 0;
		if (SUCCEEDED(SHGetMalloc(&imalloc)))
		{
			imalloc->Free(pidl);
			imalloc->Release();
		}

		return path;
	}

#endif 
	return "";
}
#endif

