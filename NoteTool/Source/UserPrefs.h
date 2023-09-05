#pragma once

#include <string>
#include <vector>
#include <filesystem>

struct WorkspacePath
{
	std::string name;
	std::string path;
	std::string time;
};

class UserPrefs
{
public:

	void LoadFromJSON(const std::filesystem::path& path);
	void SaveToJSON(const std::filesystem::path& path);

	void AddRecentlyOpened(WorkspacePath path);

	// data in userprefs

	std::vector<WorkspacePath> recentlyOpenWorkspaces;

	std::filesystem::path theme;

	int lastOpenedWidth = 1280, lastOpenedHeight = 720;

private:
};