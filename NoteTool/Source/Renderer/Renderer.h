#pragma once

#include "../Colour.h"
#include <SDL2/SDL.h>
#include "../Maths/Vector2.h"
#include <vector>
#include "../Maths/Matrix.h"


#include "GLImpl/GPUTextureGL.h"

typedef GPUTextureGL GPUTexture;

struct Vertex
{
	Vector2f position;
	Vector2f texCoord;
	Colour colour;
};

class RendererBase
{
public:

	virtual void Initialise(SDL_Window* win) = 0;

	virtual void Shutdown() = 0;

	virtual void BeginRenderpass(const Colour& clear) = 0;

	virtual void EndRenderpass() = 0;

	virtual void SetViewport(int x, int y, int w, int h) = 0;

	virtual void SetScissor(int x, int y, int w, int h) = 0;

	virtual void SubmitVertices(std::vector<Vertex> vertices, std::vector<uint32_t> indices, const Matrix4x4f& mat, GPUTexture* tex, uint32_t indexOffset, uint32_t indexCount) = 0;
};
