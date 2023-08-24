
#include "Shader.h"
#include <string>
#include <iostream>

void ShaderGL::Bind()
{
    glUseProgram(m_Id);
}

const std::string baseVertexSrc =
R"(
#version 330 core

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec4 aColour;

uniform mat4 uMatrix;

out vec2 vTexCoord;
out vec4 vColour;

void main()
{
	gl_Position = uMatrix * vec4(aPosition, 0.0, 1.0);

	vTexCoord = aTexCoord;
	vColour = aColour;
}	

)";

const std::string baseFragmentSrc =
R"(
#version 330 core

out vec4 oColour;

in vec2 vTexCoord;

in vec4 vColour;

uniform sampler2D uTex0;

void main()
{
	oColour = texture(uTex0, vTexCoord) * vColour;
}	
)";

void ShaderGL::UseDefaultShaders()
{
    LoadFromSource(baseVertexSrc.c_str(), baseFragmentSrc.c_str());
}

void ShaderGL::LoadFromSource(const char* fragmentSrc)
{
    LoadFromSource(baseVertexSrc.c_str(), fragmentSrc);
}

void ShaderGL::LoadFromSource(const char* vertexSrc, const char* fragmentSrc)
{
    m_Id = glCreateProgram();

    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Load and compile vertex shader
    glShaderSource(vertexShaderID, 1, &vertexSrc, NULL);
    glCompileShader(vertexShaderID);
    if (!CheckShaderCompilation(vertexShaderID)) {
        return;
    }

    // Load and compile fragment shader
    glShaderSource(fragmentShaderID, 1, &fragmentSrc, NULL);
    glCompileShader(fragmentShaderID);
    if (!CheckShaderCompilation(fragmentShaderID)) {
        return;
    }

    // Attach shaders to the program
    glAttachShader(m_Id, vertexShaderID);
    glAttachShader(m_Id, fragmentShaderID);

    // Link the program
    glLinkProgram(m_Id);
    if (!CheckProgramLinking(m_Id)) {
        return;
    }

    // Clean up
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    SetShaderUniform("uTex0", 0);
    SetShaderUniform("uTex1", 1);
    SetShaderUniform("uTex2", 2);
    SetShaderUniform("uTex3", 3);

}

bool ShaderGL::CheckShaderCompilation(GLuint shaderID) {
    GLint compilationStatus;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compilationStatus);
    if (compilationStatus == GL_FALSE) {
        GLint logLength;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
        GLchar* infoLog = new GLchar[logLength];
        glGetShaderInfoLog(shaderID, logLength, NULL, infoLog);
        std::cerr << "Shader compilation error: " << infoLog << std::endl;
        delete[] infoLog;
        return false;
    }
    return true;
}

bool ShaderGL::CheckProgramLinking(GLuint programID) {
    GLint linkingStatus;
    glGetProgramiv(programID, GL_LINK_STATUS, &linkingStatus);
    if (linkingStatus == GL_FALSE) {
        GLint logLength;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);
        GLchar* infoLog = new GLchar[logLength];
        glGetProgramInfoLog(programID, logLength, NULL, infoLog);
        std::cerr << "Shader program linking error: " << infoLog << std::endl;
        delete[] infoLog;
        return false;
    }
    return true;
}

void ShaderGL::SetShaderUniform(const char* name, const Matrix4x4f& mat4x4)
{
    glUseProgram(m_Id);
    uint32_t loc = glGetUniformLocation(m_Id, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, mat4x4.m_flat);
}

void ShaderGL::SetShaderUniform(const char* name, const Vector2f& vec)
{
    glUseProgram(m_Id);
    uint32_t loc = glGetUniformLocation(m_Id, name);
    glUniform2fv(loc, 1, vec.v);
}

void ShaderGL::SetShaderUniform(const char* name, float f)
{
    glUseProgram(m_Id);
    uint32_t loc = glGetUniformLocation(m_Id, name);
    glUniform1f(loc, f);
}

void ShaderGL::SetShaderUniform(const char* name, int i)
{
    glUseProgram(m_Id);
    uint32_t loc = glGetUniformLocation(m_Id, name);
    glUniform1i(loc, i);
}