
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

void PlainTextViewer::InitialiseGUIElements()
{
	m_TextBox = m_Panel->NewChild<gui::TextBox>();
	m_TextBox->SetFont(m_FontManager->Get(gui::FontWeight::Regular, 16));

	m_TextBox->SetBounds({ 10.0f, 30.0f, m_Panel->GetBounds().w, m_Panel->GetBounds().h });
	m_TextBox->string = m_FileContents;
}

void PlainTextViewer::Save()
{
	m_FileContents = m_TextBox->string;

	std::ofstream file(m_File->path);

	assert(file.is_open());

	file << m_FileContents;

	file.close();
}