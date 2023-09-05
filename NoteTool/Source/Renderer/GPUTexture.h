#pragma once
#include "../Image.h"

enum class GPUFormat
{
	None,
	RGBA8,

	D24,

	Count,
};


class GPUTextureBase 
{
public:

	virtual void Destroy() = 0;

	virtual void CreateFromImage(Image& img) = 0;

	virtual void CreateFromPixels(void* pixels, uint32_t w, uint32_t h, GPUFormat format) = 0;

	const uint32_t GetWidth() const { return m_Width; }
	const uint32_t GetHeight() const { return m_Height; }

	bool IsDepthFormat()
	{
		if (m_Format == GPUFormat::D24)
			return true;

		return false;
	}

protected:

	uint32_t m_Width, m_Height;
	GPUFormat m_Format;
};