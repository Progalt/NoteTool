#pragma once

#include "../Renderer.h"
#include "Shader.h"

constexpr size_t VertexSize = 8192 * 2;
constexpr size_t IndexSize = VertexSize * 3ui64;


class RendererGL : public RendererBase
{
public:

	void Initialise(SDL_Window* win) override;

	void Shutdown()  override;

	void BeginRenderpass(const Colour& clear)  override;

	void EndRenderpass() override;

	void SetViewport(int x, int y, int w, int h) override;

	void SetScissor(int x, int y, int w, int h) override;


	void CopyToBuffers(std::vector<Vertex> vertices, std::vector<uint32_t> indices) override;

	void Draw(const Matrix4x4f& mat, GPUTexture* tex, uint32_t indexOffset, uint32_t indexCount, uint32_t vertexOffset) override;

	void SubmitVertices(std::vector<Vertex> vertices, std::vector<uint32_t> indices, const Matrix4x4f& mat, GPUTexture* tex, uint32_t indexOffset, uint32_t indexCount) override;

private:

	ShaderGL m_DefaultShader;

	struct
	{
		size_t vboOffset = 0, iboOffset = 0;
		uint32_t vao = 0, vbo = 0, ibo = 0;

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		uint32_t currentVertexSize, currentIndexSize;
	} m_Buffer;

	void CreateBuffers(uint32_t vertexSize, uint32_t indexSize);
	void CleanupBuffers();

	void ResizeBuffers(uint32_t newVertexSize, uint32_t newIndexSize);

};