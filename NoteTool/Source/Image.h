#pragma once

#include "Colour.h"
#include <cstdint>
#include <vector>
#include <string>

static uint32_t imageID;

class Image
{
public:

	~Image();

	Image() { m_Id = imageID++; }

	Image(const std::string& path);

	void LoadFromFile(const std::string& path);

	void LoadFromMemory(std::vector<uint8_t> mem);

	void LoadFromMemory(uint8_t* mem, size_t size);

	void CreateIcon(float* icon, uint32_t w, uint32_t h);

	void New(uint32_t w, uint32_t h, uint32_t channels);

	void Expand(uint32_t newWidth, uint32_t newHeight);

	void Copy(Image& img, uint32_t x, uint32_t y);

	void SetPixel(uint32_t x, uint32_t y, Colour c);

	void SetPixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

	Colour GetPixel(uint32_t x, uint32_t y);

	void Fill(Colour c, uint32_t yStart = 0);

	const uint32_t GetWidth() const { return m_Width; }

	const uint32_t GetHeight() const { return m_Height; }

	uint8_t* GetPixels() { return m_Pixels.data(); }

	bool operator==(Image& img)
	{
		if (img.m_Id == m_Id)
			return true;

		return false;
	}

private:


	uint32_t m_Id;
	uint32_t m_Width, m_Height, m_Channels;
	bool m_LoadedFromMemory;

	std::vector<uint8_t> m_Pixels;
};