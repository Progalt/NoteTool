
#include "RendererGL.h"
#include <glad/glad.h>
#include <cassert>
#include "../../Platform.h"
#include "../../OS.h"


#define glCheck(func) func; if (GL_NO_ERROR != glGetError()) OS::GetInstance().DebugPrint("GL Error: %d, %s\n", __LINE__, __FILE__);

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
	if (!gladLoadGL())
	{
		OS::GetInstance().DebugPrint("Failed to initialise GLAD\n");
	}


	glEnable(GL_SCISSOR_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
//#endif

	//glEnable(GL_FRAMEBUFFER_SRGB);

	glEnable(GL_MULTISAMPLE);


	m_DefaultShader.UseDefaultShaders();

	CreateBuffers(VertexSize, IndexSize);
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
	glCheck(glViewport(x, y, w, h));
}

void RendererGL::SetScissor(int x, int y, int w, int h)
{
	glCheck(glScissor(x, y, w, h));

}

void RendererGL::CopyToBuffers(std::vector<Vertex> vertices, std::vector<uint32_t> indices)
{
	if (vertices.size() > m_Buffer.currentVertexSize)
	{
		ResizeBuffers(m_Buffer.currentVertexSize + VertexSize, m_Buffer.currentIndexSize + IndexSize);
	}

	assert(vertices.size() < m_Buffer.currentVertexSize);
	assert(indices.size() < m_Buffer.currentIndexSize);


	//glCheck(glBindBuffer(GL_ARRAY_BUFFER, m_Buffer.vbo));
	//glCheck(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertices.size(), vertices.data()));
	//glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer.ibo));
	//glCheck(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint32_t) * indices.size(), indices.data()));

	memcpy(m_Buffer.mappedVBO, vertices.data(), sizeof(Vertex) * vertices.size());
	memcpy(m_Buffer.mappedIBO, indices.data(), sizeof(uint32_t) * indices.size());
}

void RendererGL::Draw(const Matrix4x4f& mat, GPUTexture* tex, uint32_t indexOffset, uint32_t indexCount, uint32_t vertexOffset)
{
	m_DefaultShader.Bind();
	m_DefaultShader.SetShaderUniform("uMatrix", mat);

	glCheck(glBindVertexArray(m_Buffer.vao));
	glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer.ibo));

	if (tex)
	{
		glCheck((glBindTexture(GL_TEXTURE_2D, tex->GetGLId())));
	}


	glCheck(glDrawElementsBaseVertex(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (const void*)(indexOffset * sizeof(uint32_t)), vertexOffset));
}

void RendererGL::SubmitVertices(std::vector<Vertex> vertices, std::vector<uint32_t> indices, const Matrix4x4f& mat, GPUTexture* tex, uint32_t indexOffset, uint32_t indexCount)
{
	m_DefaultShader.Bind();
	m_DefaultShader.SetShaderUniform("uMatrix", mat);

	size_t offset = m_Buffer.vboOffset;

	//std::vector<uint32_t> offsetIndices;

	//for (auto& idx : indices)
	//	offsetIndices.push_back(idx + offset);

	if (vertices.size() > m_Buffer.currentVertexSize)
	{
		ResizeBuffers(m_Buffer.currentVertexSize + VertexSize, m_Buffer.currentIndexSize + IndexSize);
	}

	assert(vertices.size() < m_Buffer.currentVertexSize);
	assert(indices.size() < m_Buffer.currentIndexSize);

	glCheck(glBindBuffer(GL_ARRAY_BUFFER, m_Buffer.vbo));
	glCheck(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertices.size(), vertices.data()));
	glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer.ibo));


	glCheck(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint32_t) * indices.size(), indices.data()));

	glCheck(glBindVertexArray(m_Buffer.vao));
	glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer.ibo));

	if (tex)
	{
		glCheck((glBindTexture(GL_TEXTURE_2D, tex->GetGLId())));
	}


	glCheck(glDrawElementsBaseVertex(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (const void*)(indexOffset * sizeof(uint32_t)), 0));

	m_Buffer.vboOffset += vertices.size();
	m_Buffer.iboOffset += indices.size();

}


void RendererGL::CreateBuffers(uint32_t vertexSize, uint32_t indexSize)
{
	glCheck(glGenBuffers(1, &m_Buffer.vbo));
	glCheck(glGenBuffers(1, &m_Buffer.ibo));
	glCheck(glGenVertexArrays(1, &m_Buffer.vao));

	glCheck(glBindVertexArray(m_Buffer.vao));

	uint32_t flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

	glCheck(glBindBuffer(GL_ARRAY_BUFFER, m_Buffer.vbo));
	//glCheck(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexSize, nullptr, GL_DYNAMIC_DRAW));
	glCheck(glBufferStorage(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexSize, nullptr, flags));
	m_Buffer.mappedVBO = glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertexSize, flags);

	glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer.ibo));
	//glCheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indexSize, nullptr, GL_DYNAMIC_DRAW));
	glCheck(glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indexSize, nullptr, flags));
	m_Buffer.mappedIBO = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint32_t) * indexSize, flags);

	glCheck(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0));
	glCheck(glEnableVertexAttribArray(0));

	glCheck(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord)));
	glCheck(glEnableVertexAttribArray(1));

	glCheck(glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, colour)));
	glCheck(glEnableVertexAttribArray(2));


	m_Buffer.currentVertexSize = vertexSize;
	m_Buffer.currentIndexSize = indexSize;
}

void RendererGL::ResizeBuffers(uint32_t newVertexSize, uint32_t newIndexSize)
{
	OS::GetInstance().DebugPrint("Resizing Buffer\n");
	CleanupBuffers();
	CreateBuffers(newVertexSize, newIndexSize);
}

void RendererGL::CleanupBuffers()
{
	uint32_t buffers[2] = { m_Buffer.vbo, m_Buffer.ibo };
	glDeleteBuffers(2, buffers);

	glDeleteVertexArrays(1, &m_Buffer.vao);
}