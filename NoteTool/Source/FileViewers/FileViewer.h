#pragma once

#include "../Workspace.h"
#include "../UI/Panel.h"
#include "../UI/FontManager.h"

class FileViewer
{
public:

	void SetFile(File* file) { m_File = file; LoadFileContents(); InitialiseGUIElements(); }

	void SetParentPanel(gui::Panel* panel) { m_Panel = panel; }

	void SetFontManager(gui::FontManager* fontManager) { m_FontManager = fontManager; }

	void SetCodeFontManager(gui::FontManager* fontManager) { m_CodeFontManager = fontManager; }

	virtual void Hide() = 0;

	virtual void Show() = 0;

	File* GetFile() { return m_File; }

	virtual void Save() = 0;

	virtual void Refresh() { }

protected:

	virtual void LoadFileContents() = 0;

	virtual void InitialiseGUIElements() = 0;

	File* m_File;

	gui::Panel* m_Panel;

	bool m_Editable = true;

	gui::FontManager* m_FontManager;
	gui::FontManager* m_CodeFontManager;

};