#pragma once
#include "../Allocator/LinearAllocator.h"
#include "../Maths/Rect.h"
#include <string>
#include <filesystem>

enum class NoteElementType
{
	Header1, 
	Header2,
	Paragraph
};

// Note Elements

struct Header
{
	std::string text;

};

struct Paragraph
{
	std::string text;
};

struct NoteElement
{
	NoteElementType type;

	Vector2i position;
	FloatRect bounds;

	Header header;
	Paragraph paragraph;

	NoteElement* next = nullptr;

	void* userData;
};



class NoteFile
{
public:

	NoteElement* End()
	{
		NoteElement* element = m_Root;

		if (!element)
		{
			m_Root = m_Elements.Allocate();

			return m_Root;
		}

		while (element->next != nullptr)
		{
			element = element->next;
		}

		return element;
	}

	NoteElement* Start()
	{
		if (m_Root)
			return m_Root;

		m_Root = m_Elements.Allocate();

		return m_Root;
	}

	NoteElement* Insert(NoteElement* parent)
	{

	
		if (parent)
		{
			NoteElement* element = m_Elements.Allocate();

			if (parent->next)
			{
				NoteElement* temp = parent->next;

				parent->next = element;
				element->next = temp;
			}
			else
			{
				parent->next = element;
			}

			return element;
		}
		else
		{
			return Start();
		}

	
		return nullptr;
	}

	const std::string& GetTitle() const { return title; }

	void LoadFromJSON(const std::filesystem::path& path);

	void WriteToJSON(const std::filesystem::path& path);

	void Print();

private:

	// Note properties

	std::string title;
	
	
	// Elements

	NoteElement* m_Root;

	LinearAllocator<NoteElement> m_Elements;
};