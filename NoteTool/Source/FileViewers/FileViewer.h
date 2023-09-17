#pragma once

#include "../Workspace.h"
#include "../UI/Panel.h"
#include "../UI/FontManager.h"
#include "../UI/TextBox.h"
#include "../UI/Text.h"
#include "../Theme.h"
#include "../UI/Shape.h"

class WorkspaceUI;

class FileViewer
{
public:

	void ShowExtension(bool ext) { m_ShowExtension = ext; }

	void SetFile(File* file) { m_File = file; LoadFileContents(); InitialiseGUIElements(); }

	void SetParentPanel(gui::Panel* panel) { m_Panel = panel; }

	void SetFontManager(gui::FontManager* fontManager) { m_FontManager = fontManager; }

	void SetCodeFontManager(gui::FontManager* fontManager) { m_CodeFontManager = fontManager; }

	virtual void Hide() = 0;

	virtual void Show() = 0;

	File* GetFile() { return m_File; }

	virtual void Save() = 0;

	virtual void Refresh() { }

	WorkspaceUI* parent;
	Theme* theme;

protected:

	


	virtual void LoadFileContents() = 0;

	virtual void InitialiseGUIElements() = 0;

	void SetupTitleAndExt();

	File* m_File;

	gui::Panel* m_Panel;

	bool m_Editable = true;

	gui::FontManager* m_FontManager;
	gui::FontManager* m_CodeFontManager;

	gui::TextBox* m_Title;
	gui::Text* m_FileExt;
	gui::ShapeWidget* m_SavedCircle;

	bool m_Saved = true;
	bool m_ShowExtension = true;

	float m_TitlePadding = 30.0f;
};