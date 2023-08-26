#pragma once
#include <filesystem>
#include <cassert>

enum class FileType
{
	Unrecognised,
	PlainText, 
	Image
};

struct File
{
	FileType type;
	std::string name;
	std::string extension;

	std::filesystem::path path;		// includes name and file extension

	void TypeFromExtension()
	{
		type = FileType::Unrecognised;

		// TODO: Add more extensions 
		if (extension == "txt")
		{
			type = FileType::PlainText;
		}

		if (extension == "png" ||
			extension == "jpg" ||
			extension == "bmp" ||
			extension == "tga")
		{
			type = FileType::Image;
		}

		
	}
};

struct Directory
{

	std::string name;

	std::vector<Directory> subdirectories;
	std::vector<File> files;

	std::filesystem::path path;

	void ParseDirectory(const std::filesystem::path& path);

	Directory& GetDirectory(size_t idx) 
	{ 
		assert(idx < subdirectories.size());
		return subdirectories[idx];
	}

	File& GetFile(size_t idx)
	{
		assert(idx < files.size());
		return files[idx];
	}

	size_t DirectoryCount() { return subdirectories.size(); }

	size_t FileCount() { return files.size(); }

};

class Workspace
{
public:

	void OpenWorkspace(const std::filesystem::path& path);

	bool IsValid() { return !m_Root.name.empty(); }

	bool Exists(const std::string& file);

	Directory& GetRoot() { return m_Root; }

private:

	Directory m_Root;


};