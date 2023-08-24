#pragma once

#include <cstdint>
#include <glad/glad.h>
#include "../../Maths/Vector2.h"
#include "../../Maths/Matrix.h"

class ShaderGL
{
public:

	~ShaderGL()
	{
		if (m_Id)
			glDeleteProgram(m_Id);
	}

	void Bind();

	void LoadFromSource(const char* vertexSrc, const char* fragmentSrc);

	void LoadFromSource(const char* fragmentSrc);

	void UseDefaultShaders();

	void SetShaderUniform(const char* name, const Matrix4x4f& mat4x4);

	void SetShaderUniform(const char* name, const Vector2f& vec);

	void SetShaderUniform(const char* name, float f);

	void SetShaderUniform(const char* name, int i);

private:

	uint32_t m_Id;

	bool CheckShaderCompilation(GLuint shaderID);
	bool CheckProgramLinking(GLuint programID);
};