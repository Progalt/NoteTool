#pragma once


#include "../GPUTexture.h"

class GPUTextureGL : public GPUTextureBase
{
public:

	~GPUTextureGL();

	void CreateFromImage(Image& img) override;

	void CreateFromPixels(void* pixels, uint32_t w, uint32_t h, GPUFormat format) override;

	const uint32_t GetGLId() const { return m_Id; }

private:


	uint32_t m_Id;

};