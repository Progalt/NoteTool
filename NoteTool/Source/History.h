#pragma once
#include <string>

// We want to have a max
// This is so if we are editing for hours on the same file it doesn't begin to eat lots of memory 
constexpr uint32_t MAX_HISTORY_ROLLBACK = 32;

struct HistoryEntry
{
	enum class Type
	{
		TextRemove,
		TextAdd
	};

	Type type;

	std::string str;
	uint32_t offset;

};

// This holds the history for stuff like undo or redo 
class History
{
public:


private:
};