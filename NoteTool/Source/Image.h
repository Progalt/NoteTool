#pragma once

#include "Colour.h"
#include <cstdint>
#include <vector>
#include <string>

class Image
{
public:

	~Image();

	Image() { }

	Image(const std::string& path);

	void LoadFromFile(const std::string& path);

	void LoadFromMemory(std::vector<uint8_t> mem);

	void LoadFromMemory(uint8_t* mem, size_t size);

	void CreateIcon(float* icon, uint32_t w, uint32_t h);

	void New(uint32_t w, uint32_t h, uint32_t channels);

	void SetPixel(uint32_t x, uint32_t y, Colour c);

	Colour GetPixel(uint32_t x, uint32_t y);

	void Fill(Colour c);

	const uint32_t GetWidth() const { return m_Width; }

	const uint32_t GetHeight() const { return m_Height; }

	uint8_t* GetPixels() { return m_Pixels.data(); }

private:


	uint32_t m_Width, m_Height, m_Channels;
	bool m_LoadedFromMemory;

	std::vector<uint8_t> m_Pixels;
};