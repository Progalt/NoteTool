
#include "UserPrefs.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <ctime>
#include <sstream>

using json = nlohmann::json;


void UserPrefs::LoadFromJSON(const std::filesystem::path& path)
{
	std::ifstream userPrefsFile(path);

	if (!userPrefsFile)
	{
		printf("Failed to open userprefs file for reading\n");
		return;
	}

	json input;

	userPrefsFile >> input;

	for (auto& workspace : input["recentWorkspaces"])
	{
		WorkspacePath path;
		path.name = workspace["name"];
		path.path = workspace["path"];
		path.time = workspace["time"];

		// If it doesn't exist we don't want to add it as recently open
		// User could've deleted it out of application
		if (std::filesystem::exists(path.path))
		{

			recentlyOpenWorkspaces.push_back(path);

			printf("Recently Open: %s\n", path.name.c_str());
		}
	}

	if (input.find("width") != input.end())
		lastOpenedWidth = input["width"];

	if (input.find("height") != input.end())
		lastOpenedHeight = input["height"];
}

void UserPrefs::SaveToJSON(const std::filesystem::path& path)
{
	std::ofstream outputFile(path);

	if (!outputFile.is_open())
	{
		printf("Failed to open userprefs file for writing\n");
		return;
	}

	json outputJson;

	json openWorkspaces;// = outputJson["recentWorkspaces"];

	uint32_t idx = 0;
	for (auto& workspace : recentlyOpenWorkspaces)
	{
		openWorkspaces[idx]["name"] = workspace.name;
		openWorkspaces[idx]["path"] = workspace.path;
		openWorkspaces[idx]["time"] = workspace.time;

		idx++;
	}

	outputJson["recentWorkspaces"] = openWorkspaces;

	outputJson["theme"] = theme;

	outputJson["width"] = lastOpenedWidth;
	outputJson["height"] = lastOpenedHeight;

	
	outputFile << std::setw(4) << outputJson;
}

void UserPrefs::AddRecentlyOpened(WorkspacePath path)
{
	uint32_t idx = UINT32_MAX;

	uint32_t i = 0;
	for (auto& recent : recentlyOpenWorkspaces)
	{
		if (recent.name == path.name && recent.path == path.path)
		{
			idx = i;
		}

		i++;
	}

	if (idx != UINT32_MAX)
	{
		recentlyOpenWorkspaces.erase(recentlyOpenWorkspaces.begin() + idx);
	}

	time_t t = std::time(nullptr);
	tm tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%H:%M:%S %d/%m/%Y");
	recentlyOpenWorkspaces.push_back({ path.name, path.path, oss.str()});
}