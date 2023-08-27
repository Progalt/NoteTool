#pragma once

#include <string>
#include <vector>

struct WorkspacePath
{
	std::string name;
	std::string path;
};

class UserPrefs
{
public:


	std::vector<WorkspacePath> recentlyOpenWorkspaces;

private:
};