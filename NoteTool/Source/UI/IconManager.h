#pragma once
#include<unordered_map>
#include "../Renderer/Renderer.h"

enum class IconType
{
	None,

	Folder_Empty,
	Folder_Populated,

	Folder_Add,

	Pin, 

	BasicFile,

	ThreeDots,

	FileDropArrow,

	Cross,
	Plus, 

	Bin
};


class IconManager
{
public:

	static IconManager& GetInstance()
	{
		static IconManager instance;

		return instance;
	}

	IconManager(IconManager const&) = delete;
	void operator=(IconManager const&) = delete;

	void LoadIcons()
	{

		// Load the icons into GPU textures

		Image folderEmpty("Icons/Folder24.png");
		m_Icons[IconType::Folder_Empty].CreateFromImage(folderEmpty);
		
		Image folderPopulated("Icons/FolderPopulated24.png");
		m_Icons[IconType::Folder_Populated].CreateFromImage(folderPopulated);

		Image folderAdd("Icons/FolderAdd24.png");
		m_Icons[IconType::Folder_Add].CreateFromImage(folderAdd);

		Image pin("Icons/Pin24.png");
		m_Icons[IconType::Pin].CreateFromImage(pin);

		Image dots("Icons/ThreeDots.png");
		m_Icons[IconType::ThreeDots].CreateFromImage(dots);


		Image basicFile("Icons/BasicFile24.png");
		m_Icons[IconType::BasicFile].CreateFromImage(basicFile);

		Image dropArrow("Icons/FileDropArrow24.png");
		m_Icons[IconType::FileDropArrow].CreateFromImage(dropArrow);

		Image cross("Icons/X.png");
		m_Icons[IconType::Cross].CreateFromImage(cross);

		Image bin("Icons/Bin.png");
		m_Icons[IconType::Bin].CreateFromImage(bin);

		Image plus("Icons/Plus.png");
		m_Icons[IconType::Plus].CreateFromImage(plus);
	}

	void DestroyIcons()
	{

	}

	GPUTexture* operator[](IconType type)
	{
		if (m_Icons.find(type) != m_Icons.end())
			return &m_Icons[type];
	}

private:
	IconManager() { }

	

	std::unordered_map<IconType, GPUTexture> m_Icons;

};