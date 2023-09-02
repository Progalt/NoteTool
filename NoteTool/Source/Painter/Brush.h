#pragma once

enum LineMode
{
	None,
	Antialiasing
};

enum class BrushMode
{
	None, 
	Solid
};

class Brush
{
public:

	Brush() : m_Mode(BrushMode::Solid) { }

	void SetMode(BrushMode mode) { m_Mode = mode; }
	const BrushMode GetMode() { return m_Mode; }

	void SetColour(const Colour& col) { m_Colour = col; }
	Colour GetColour() const { return m_Colour; }

private:

	BrushMode m_Mode;

	Colour m_Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
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