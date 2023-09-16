

#include "NoteFile.h"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;


void NoteFile::LoadFromJSON(const std::filesystem::path& path)
{

	std::ifstream file(path);

	if (!file.is_open())
	{
		printf("Failed to open note for reading\n");
		return;
	}

	json note;
	file >> note;

	auto contains = [&](const std::string& name)
		{
			if (note.find(name) != note.end())
				return true;

			return false;
		};

	if (!contains("version"))
	{
		printf("note does not contain version. Cannot parse\n");
		return;
	}

	int versionNum = note["version"];
	
	if (contains("title"))
		title = note["title"];
	
	if (!contains("elements"))
	{
		printf("Note contains no elements\n");
		return;
	}

	// Loop through all elements
	// and add them
	NoteElement* last = nullptr;
	for (auto& element : note["elements"])
	{
		// Grab the current last element
		NoteElement* current = Insert(last);

		std::string type = element["type"];

		if (type == "header")
		{
			
			int size = element["size"];

			switch (size)
			{
			case 1:
				current->type = NoteElementType::Header1;
				break;
			case 2:
				current->type = NoteElementType::Header2;
				break;
			}

			current->header.text = element["text"];
		}
		else if (type == "paragraph")
		{
			current->type = NoteElementType::Paragraph;
			current->paragraph.text = element["text"];
		}
		else if (type == "dividor")
		{
			current->type = NoteElementType::Dividor;
		}
		else if (type == "bulletPoint")
		{
			current->type = NoteElementType::BulletPoint;
			current->paragraph.text = element["text"];
		}
		else if (type == "quote")
		{
			current->type = NoteElementType::Quote;
			current->quote.text = element["text"];
			current->quote.title = element["title"];
		}

		last = current;
	}



}

void NoteFile::WriteToJSON(const std::filesystem::path& path)
{
	// We want to write to a json

	std::ofstream file(path);

	json output;

	output["version"] = 1;
	output["title"] = title;

	json& elements = output["elements"];


	NoteElement* element = Start();
	uint32_t idx = 0;
	while (element != nullptr)
	{

		switch (element->type)
		{
		case NoteElementType::Header1:
			elements[idx] = {
				{ "type", "header" },
				{ "size", 1},
				{ "text", element->header.text}
			};
			break;
		case NoteElementType::Header2:
			elements[idx] = {
				{ "type", "header" },
				{ "size", 2},
				{ "text", element->header.text}
			};
			break;
		case NoteElementType::Paragraph:
			elements[idx] = {
				{ "type", "paragraph" },
				{ "text", element->paragraph.text }
			};
			break;
		case NoteElementType::Dividor:
			elements[idx] = {
				{ "type", "dividor" }
			};
			break;
		case NoteElementType::BulletPoint:
			elements[idx] = {
				{ "type", "bulletPoint"},
				{ "text", element->paragraph.text }
			};
			break;
		case NoteElementType::Quote:
			elements[idx] = {
				{ "type", "quote"},
				{ "text", element->quote.text },
				{ "title", element->quote.title }
			};
			break;
		}


		element = element->next;
		idx++;
	}



	// Finally write to the file

	// Use setw to make the json look nice
	file << std::setw(4) << output;
}


void NoteFile::Print()
{
	NoteElement* element = Start();

	printf("Note File: %s\n", title.c_str());

	while (element != nullptr)
	{

		std::string type = "";
		std::string text = "";

		switch (element->type)
		{
		case NoteElementType::Header1:
			type = "Header1";
			text = element->header.text;
			break;
		case NoteElementType::Header2:
			type = "Header2";
			text = element->header.text;
			break;
		case NoteElementType::Paragraph:
			type = "Paragraph";
			text = element->paragraph.text;
			break;
		}

		printf("%s: %s\n", type.c_str(), text.c_str());
		


		element = element->next;
	}
}