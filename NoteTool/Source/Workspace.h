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
struct Directory;

const std::string WorkspaceJSONFile = "workspace-config.json";

struct File
{
	File () { }

	File(const std::filesystem::path& path)
	{
		this->path = path;

		if (path.has_extension())
			extension = path.extension().generic_string();

		if (path.has_filename())
			name = path.filename().generic_string();

		TypeFromExtension();
	}

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

	std::string PathWithoutName()
	{
		return path.remove_filename().generic_string();
	}

	std::string NameWithoutExtension()
	{
		if (!NameHasFileExtension())
			return name;


		size_t offset = name.find(extension);

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

	Directory* parentDirectory = nullptr;
};

struct Directory
{

	std::string name;

	std::vector<Directory> subdirectories;
	std::vector<File> files;

	std::filesystem::path path;

	void ParseDirectory(const std::filesystem::path& path);

	Directory* GetDirectoryOfFile(File* file)
	{
		for (auto& f : files)
			if (f == *file)
				return this;

		Directory* dir = nullptr;
		for (auto& d : subdirectories)
		{
			dir = d.GetDirectoryOfFile(file);
		}

		return dir;
	}

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

	File* GetFile(const std::string& name)
	{
		for (auto& file : files)
			if (file.name == name)
				return &file;

		File* file = nullptr;

		for (auto& d : subdirectories)
		{
			file = d.GetFile(name);
		}

		return file;
	}

	File* GetFilePtr(size_t idx)
	{
		return &files[idx];
	}

	uint32_t GetFileIndex(File* file)
	{
		uint32_t i = 0;
		for (auto& f : files)
		{
			if (*file == f)
				return i;

			i++;
		}

		return -1;
	}

	size_t DirectoryCount() { return subdirectories.size(); }

	size_t FileCount() { return files.size(); }

};

class Workspace
{
public:

	void OpenWorkspace(const std::filesystem::path& path);

	void Close();

	void Refresh();

	bool IsValid() { return !m_Root.name.empty(); }

	bool Exists(const std::string& file);

	Directory& GetRoot() { return m_Root; }

	std::vector<File*> pins;

private:

	Directory m_Root;

	


};