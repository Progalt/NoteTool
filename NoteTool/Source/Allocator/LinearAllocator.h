#pragma once

#include <cstdint>
#include <cstdio>
#include "../OS.h"



// Really dead simple allocator
// Does not free any data until destructor
// But will expand if needed

template<typename _Ty>
class LinearAllocator
{
public:

	LinearAllocator()
	{
		AllocateBlock(nullptr);
	}

	~LinearAllocator()
	{
		Block* block = m_Root;

		while (block != nullptr)
		{
			delete[] block->memPtr;
			block->memPtr = nullptr;

			block = block->next;
		}
	}


	void FreeAll()
	{
		m_Pointer = 0;
	}

	_Ty* Allocate()
	{
		return GetNext();
	}



private:

	const uint32_t m_BlockSize = 32;

	struct Block
	{
		_Ty* memPtr;
		Block* next;
	};

	Block* m_Root;
	Block* m_Current;

	void AllocateBlock(Block* parent)
	{
		if (parent)
		{
			parent->next = new Block();
			parent->next->memPtr = new _Ty[m_BlockSize];

			m_Current = parent->next;
		}
		else
		{
			m_Root = new Block();
			m_Root->memPtr = new _Ty[m_BlockSize];

			m_Current = m_Root;
		}

		OS::GetInstance().DebugPrint("Allocated New Block\n");
	}

	_Ty* GetNext()
	{
		if (m_Pointer >= m_BlockSize)
		{
			AllocateBlock(m_Current);
			m_Pointer = 0;
		}

		_Ty* ptr = &m_Current->memPtr[m_Pointer];

		m_Pointer++;

		return ptr;
	}


	size_t m_AllocatedSize = 0;
	size_t m_Pointer = 0;

	_Ty* m_Memory;

	
};