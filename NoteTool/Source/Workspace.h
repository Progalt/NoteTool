#pragma once
#include <filesystem>

enum class FileType
{
	PlainText, 
	Image
};

struct File
{
	FileType type;
	std::string name;
	std::string extension;

	std::filesystem::path path;		// includes name and file extension
};

struct Directory
{

	std::string name;

	std::vector<Directory> subdirectories;
	std::vector<File> files;

	std::filesystem::path path;
};

class Workspace
{
public:

	void OpenWorkspace(const std::filesystem::path& path);


private:

	Directory m_Root;

};