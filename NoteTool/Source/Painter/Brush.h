#pragma once

enum LineMode
{
	None,
	Antialiasing
};

class Brush
{
public:

private:
};


class Pen
{
public:

	void SetLineMode(LineMode lineMode) { m_LineMode = lineMode; }
	LineMode GetLineMode() const { return m_LineMode; }

	void SetColour(const Colour& col) { m_Colour = col; }
	Colour GetColour() const { return m_Colour; }

private:

	LineMode m_LineMode = LineMode::None;

	Colour m_Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
};