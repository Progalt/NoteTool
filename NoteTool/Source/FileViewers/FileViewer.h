#pragma once

#include "../Workspace.h"
#include "../UI/Panel.h"

class FileViewer
{
public:

	void SetFile(File* file) { m_File = file; LoadFileContents(); }

	void SetParentPanel(gui::Panel* panel) { m_Panel = panel; }

protected:

	virtual void LoadFileContents() = 0;

	File* m_File;

	gui::Panel* m_Panel;

};