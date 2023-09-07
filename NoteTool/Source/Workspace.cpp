
#include "Workspace.h"

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
			printf("Directory: %s\n", dir.name.c_str());
		}
		if (item.is_regular_file())
		{
			File file(item.path());

			files.push_back(file);
			printf("File: %s\n", file.name.c_str());
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

	
}

bool Workspace::Exists(const std::string& file)
{
	return true;
}

void Workspace::Refresh()
{
	OpenWorkspace(m_Root.path);
}