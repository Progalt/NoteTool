#pragma once

#include <cstdint>
#include <cstdio>

// Really dead simple allocator
// Does not free any data until destructor
// But will expand if needed

template<typename _Ty>
class LinearAllocator
{
public:

	LinearAllocator()
	{
		Reallocate(64);
	}

	~LinearAllocator()
	{
		if (m_Memory)
			delete[] m_Memory;
	}

	void FreeAll()
	{
		m_Pointer = 0;
	}

	_Ty* Allocate()
	{
		if (m_Pointer + 1 > m_AllocatedSize)
		{
			Reallocate(m_AllocatedSize + 1);
		}

		_Ty* ptr = &m_Memory[m_Pointer];

		m_Pointer++;

		return ptr;
	}



private:

	void Reallocate(size_t size)
	{

		_Ty* temp = new _Ty[size];

		if (m_Memory)
		{
			printf("Reallocating Memory...\n");
			for (uint32_t i = 0; i < m_AllocatedSize; i++)
				temp[i] = m_Memory[i];

			delete[] m_Memory;
		}

		m_Memory = temp;
		m_AllocatedSize = size;
	}

	size_t m_AllocatedSize = 0;
	size_t m_Pointer = 0;

	_Ty* m_Memory;
};