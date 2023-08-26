
#include "PlainTextViewer.h"
#include <fstream>
#include <sstream>
#include <cassert>

void PlainTextViewer::LoadFileContents()
{
	std::ifstream file(m_File->path);

	assert(file.is_open());

	std::stringstream stream;
	stream << file.rdbuf();

	m_FileContents = stream.str();

}