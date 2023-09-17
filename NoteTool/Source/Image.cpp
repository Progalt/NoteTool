
#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <cassert>

Image::~Image()
{
	//printf("Image freed\n");
}

Image::Image(const std::string& path)
{
	LoadFromFile(path);
	m_Id = imageID++;
}

void Image::LoadFromFile(const std::string& path)
{

	int x, y, c;

	uint8_t* pixels = stbi_load(path.c_str(), &x, &y, &c, STBI_rgb_alpha);


	assert(pixels && "Failed to Load Image from memory");

	m_Width = static_cast<uint32_t>(x);
	m_Height = static_cast<uint32_t>(y);
	m_Channels = static_cast<uint32_t>(4);

	m_LoadedFromMemory = true;

	New(x, y, 4);

	memcpy(m_Pixels.data(), pixels, x * y * 4);

	stbi_image_free(pixels);
}

void Image::LoadFromMemory(std::vector<uint8_t> mem)
{

	LoadFromMemory(mem.data(), mem.size());
}

void Image::LoadFromMemory(uint8_t* mem, size_t size)
{
	int x, y, c;

	uint8_t* pixels = stbi_load_from_memory(mem, size, &x, &y, &c, STBI_rgb_alpha);



	assert(pixels && "Failed to Load Image from memory");

	m_Width = static_cast<uint32_t>(x);
	m_Height = static_cast<uint32_t>(y);
	m_Channels = static_cast<uint32_t>(4);

	New(x, y, 4);

	m_LoadedFromMemory = true;

	memcpy(m_Pixels.data(), pixels, x * y * 4);

	stbi_image_free(pixels);
}

void Image::CreateIcon(float* icon, uint32_t w, uint32_t h)
{
	New(w, h, 4);

	for (uint32_t x = 0; x < w; x++)
		for (uint32_t y = 0; y < h; y++)
		{
			float brightnessNormalised = icon[x * w + y];

			uint8_t alpha = static_cast<uint8_t>(brightnessNormalised * 255.0f);

			SetPixel(x, y, { 255, 255, 255, alpha });
		}
}

void Image::New(uint32_t w, uint32_t h, uint32_t channels)
{
	m_Pixels.resize(w * h * channels);

	m_LoadedFromMemory = false;

	m_Width = w;
	m_Height = h;
	m_Channels = channels;

}

void Image::Expand(uint32_t newWidth, uint32_t newHeight)
{
	// TODO: Implement
	// Copy across old image into new memory and delete old mem
}

void Image::SetPixel(uint32_t x, uint32_t y, Colour c)
{
	if (x > m_Width || y > m_Height)
		return;


	uint32_t pixelOffset = (y * m_Width + x) * m_Channels;
	m_Pixels[pixelOffset] = c.AsByte(0);
	m_Pixels[pixelOffset + 1] = c.AsByte(1);
	m_Pixels[pixelOffset + 2] = c.AsByte(2);

	if (m_Channels == 4)
		m_Pixels[pixelOffset + 3] = c.AsByte(3);
}

void Image::SetPixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	if (x > m_Width || y > m_Height)
		return;

	uint32_t pixelOffset = (y * m_Width + x) * m_Channels;

	m_Pixels[pixelOffset] = r;
	m_Pixels[pixelOffset + 1] = g;
	m_Pixels[pixelOffset + 2] = b;

	if (m_Channels == 4)
		m_Pixels[pixelOffset + 3] = a;
}

void Image::Copy(Image& img, uint32_t x, uint32_t y)
{
	
	for (uint32_t i = 0; i < img.m_Height; i++)
		for (uint32_t j = 0; j < img.m_Width; j++)
			SetPixel(x + j, y + i, img.GetPixel(j, i));
	

}

Colour Image::GetPixel(uint32_t x, uint32_t y)
{
	uint32_t pixelOffset = y * m_Width + x;

	return Colour(m_Pixels[pixelOffset],
		m_Pixels[pixelOffset + 1],
		m_Pixels[pixelOffset + 2],
		m_Pixels[pixelOffset + 3]);
}

void Image::Fill(Colour c, uint32_t yStart)
{
	if (c == Colour{ 0.0f, 0.0f, 0.0f, 0.0f })
	{
		// Alternative path to quickly 0 the memory

		if (yStart > m_Height)
			return;
		
		uint32_t offset = yStart * m_Width * m_Channels;
		uint32_t pixelCount = m_Width * m_Height * m_Channels;
		pixelCount -= offset;
		memset(m_Pixels.data() + offset, 0, pixelCount * sizeof(uint8_t));

	}
	else
	{

		uint32_t pixelCount = m_Width * m_Height;

		for (uint32_t i = 0; i < pixelCount; i++)
		{
			uint32_t offset = i * m_Channels;

			switch (m_Channels)
			{
			case 1:
				m_Pixels[offset + 0] = c.AsByte(0);
				break;
			case 2:
				m_Pixels[offset + 0] = c.AsByte(0);
				m_Pixels[offset + 1] = c.AsByte(1);
				break;
			case 3:
				m_Pixels[offset + 0] = c.AsByte(0);
				m_Pixels[offset + 1] = c.AsByte(1);
				m_Pixels[offset + 2] = c.AsByte(2);
				break;
			case 4:
				m_Pixels[offset + 0] = c.AsByte(0);
				m_Pixels[offset + 1] = c.AsByte(1);
				m_Pixels[offset + 2] = c.AsByte(2);
				m_Pixels[offset + 3] = c.AsByte(3);
				break;
			}
		}
	}
}