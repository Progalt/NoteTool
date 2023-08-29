#pragma once
#include <filesystem>
#include <cassert>

enum class FileType
{
	Unrecognised,
	PlainText, 
	Image,
	Markdown
};

struct File
{
	FileType type;
	std::string name;
	std::string extension;
	
	bool isCodeFile = false;

	std::filesystem::path path;		// includes name and file extension

	bool NameHasFileExtension()
	{
		if (name.find(extension))
		{
			return true;
		}

		return false;
	}

	std::string NameWithoutExtension()
	{
		if (!NameHasFileExtension())
			return name;


		uint32_t offset = name.find(extension);

		std::string output = name.substr(0, offset);

		return output;
	}

	void TypeFromExtension()
	{
		type = FileType::Unrecognised;

		// TODO: Add more extensions 
		if (extension == ".txt")
		{
			type = FileType::PlainText;
		}

		if (extension == ".md")
		{
			type = FileType::Markdown;
		}

		if (extension == ".cpp" || extension == ".hpp" ||
			extension == ".c" || extension == "h" ||
			extension == ".rs" || extension == ".java" ||
			extension == ".js" || extension == ".py" ||
			extension == ".dart")
		{
			type = FileType::PlainText;
			isCodeFile = true;
		}

		if (extension == ".png" ||
			extension == ".jpg" ||
			extension == ".bmp" ||
			extension == ".tga")
		{
			type = FileType::Image;
		}

		
	}

	bool operator==(const File& f2)
	{
		if (type == f2.type && name == f2.name && extension == f2.extension)
			return true;

		return false;
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

	void Refresh();

	bool IsValid() { return !m_Root.name.empty(); }

	bool Exists(const std::string& file);

	Directory& GetRoot() { return m_Root; }

private:

	Directory m_Root;


};