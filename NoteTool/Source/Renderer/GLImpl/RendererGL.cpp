
#include "RendererGL.h"
#include <glad/glad.h>
#include <cassert>
#include "../../Platform.h"

#define glCheck(func) func; if (GL_NO_ERROR != glGetError()) printf("GL Error: %d, %s\n", __LINE__, __FILE__);

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar * message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

void RendererGL::Initialise(SDL_Window* win)
{
	assert(gladLoadGL());

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_SCISSOR_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	glEnable(GL_FRAMEBUFFER_SRGB);

	glEnable(GL_MULTISAMPLE);


	m_DefaultShader.UseDefaultShaders();

	InitBuffers();
}

void RendererGL::Shutdown()
{
	CleanupBuffers();
}

void RendererGL::BeginRenderpass(const Colour& clear)
{
	glClearColor(clear.r, clear.g, clear.b, clear.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_Buffer.vboOffset = 0;
	m_Buffer.iboOffset = 0;

}

void RendererGL::EndRenderpass()
{

}

void RendererGL::SetViewport(int x, int y, int w, int h)
{
	glViewport(x, y, w, h);
}

void RendererGL::SetScissor(int x, int y, int w, int h)
{
	glScissor(x, y, w, h);

}

void RendererGL::SubmitVertices(std::vector<Vertex> vertices, std::vector<uint32_t> indices, const Matrix4x4f& mat, GPUTexture* tex, uint32_t indexOffset, uint32_t indexCount)
{
	m_DefaultShader.Bind();
	m_DefaultShader.SetShaderUniform("uMatrix", mat);

	size_t offset = m_Buffer.vboOffset;

	std::vector<uint32_t> offsetIndices;

	for (auto& idx : indices)
		offsetIndices.push_back(idx + offset);


	glBindBuffer(GL_ARRAY_BUFFER, m_Buffer.vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertices.size(), vertices.data());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer.ibo);

	uint32_t count = indexCount;
	if (count + indexOffset > indices.size())
		count = indices.size() - indexOffset;

	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint32_t) * indices.size(), indices.data());

	glBindVertexArray(m_Buffer.vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer.ibo);

	if (tex)
	{
		glBindTexture(GL_TEXTURE_2D, tex->GetGLId());
	}


	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (const void*)(indexOffset * sizeof(uint32_t)));

	m_Buffer.vboOffset += vertices.size();
	m_Buffer.iboOffset += indices.size();

}


void RendererGL::InitBuffers()
{
	glCheck(glGenBuffers(1, &m_Buffer.vbo));
	glCheck(glGenBuffers(1, &m_Buffer.ibo));
	glCheck(glGenVertexArrays(1, &m_Buffer.vao));

	glCheck(glBindVertexArray(m_Buffer.vao));

	glCheck(glBindBuffer(GL_ARRAY_BUFFER, m_Buffer.vbo));
	glCheck(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * VertexSize, nullptr, GL_DYNAMIC_DRAW));

	glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer.ibo));
	glCheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * IndexSize, nullptr, GL_DYNAMIC_DRAW));

	glCheck(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0));
	glCheck(glEnableVertexAttribArray(0));

	glCheck(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord)));
	glCheck(glEnableVertexAttribArray(1));

	glCheck(glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, colour)));
	glCheck(glEnableVertexAttribArray(2));

	
}

void RendererGL::CleanupBuffers()
{

}