
#include "Workspace.h"
#include <fstream>

#include <nlohmann/json.hpp>
#include "OS.h"

using json = nlohmann::json;

void Directory::ParseDirectory(const std::filesystem::path& path)
{
	files.clear();
	subdirectories.clear();

	name = path.filename().generic_string();
	this->path = path;

	for (auto& item : std::filesystem::directory_iterator(path))
	{
		
		if (item.is_directory())
		{
			Directory dir;
			dir.ParseDirectory(item.path());
			subdirectories.push_back(dir);
			OS::GetInstance().DebugPrint("Directory: %s\n", dir.name.c_str());
		}
		if (item.is_regular_file())
		{


			File file(item.path());

			// We don't want to add the json config file
			if (file.name == WorkspaceJSONFile)
				continue;

			files.push_back(file);
			OS::GetInstance().DebugPrint("File: %s\n", file.name.c_str());
		}
	}
}

void Workspace::OpenWorkspace(const std::filesystem::path& path)
{
	if (path.empty())
		return;

	if (!std::filesystem::exists(path))
	{

	}

	m_Root.ParseDirectory(path);

	std::filesystem::path configPath = path / WorkspaceJSONFile;

	if (std::filesystem::exists(configPath))
	{
		// Lets load the config file

		std::ifstream configFile(configPath);

		if (!configFile.is_open())
		{
			OS::GetInstance().DebugPrint("Failed to open workspace config file for reading\n");
			return;
		}

		json input;

		configFile >> input;

		// If no pins exist in workspace it doesn't get written to config so we need 
		// to test and see if it exists
		if (input.find("pins") != input.end())
		{
			json storedPins = input["pins"];

			pins.clear();

			// Loop through and load pins
			for (auto& pin : storedPins)
			{
				std::string name = pin;

				File* file = m_Root.GetFile(name);

				if (file)
				{
					OS::GetInstance().DebugPrint("Found pin: %s\n", file->name.c_str());
					pins.push_back(file);
				}
			}
		}
	}
}

bool Workspace::Exists(const std::string& file)
{
	return true;
}

void Workspace::Refresh()
{
	OpenWorkspace(m_Root.path);
}

void Workspace::Close()
{
	std::filesystem::path configPath = m_Root.path / WorkspaceJSONFile;
	std::ofstream config(configPath);

	if (!config.is_open())
	{
		OS::GetInstance().DebugPrint("Failed to open workspace config file for writing\n");
		return;
	}

	json output;

	if (pins.size() > 0)
	{
		uint32_t i = 0;
		for (auto& pin : pins)
		{
			output["pins"][i] = pin->name;

			i++;
		}
	}

	config << std::setw(4) << output;
}