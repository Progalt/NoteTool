#pragma once
#include <string>

// We want to have a max
// This is so if we are editing for hours on the same file it doesn't begin to eat lots of memory 
constexpr uint32_t MAX_HISTORY_ROLLBACK = 32;

struct HistoryEntry
{
	enum class Type
	{
		None,
		TextRemove,
		TextAdd
	};

	Type type = Type::None;

	std::string str;
	uint32_t offset;

};

// This holds the history for stuff like undo or redo 
class History
{
public:

	void PushBack(const HistoryEntry& entry)
	{
		uint32_t truePosition = m_Offset % MAX_HISTORY_ROLLBACK;
		m_History[truePosition] = entry;
		m_Offset++;
		
		
	}

	HistoryEntry PopBack()
	{
		HistoryEntry output = HistoryEntry();

		if (m_Offset > 0)
			m_Offset--;

		uint32_t truePosition = m_Offset % MAX_HISTORY_ROLLBACK;
		output = m_History[m_Offset--];
		

		return output;
	}

private:

	HistoryEntry m_History[MAX_HISTORY_ROLLBACK];
	uint32_t m_Offset = 0;
};