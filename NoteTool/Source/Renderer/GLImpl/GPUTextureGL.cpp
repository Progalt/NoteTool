
#include "GPUTextureGL.h"
#include <glad/glad.h>
#include "../../OS.h"

#define glCheck(func) func; if (GL_NO_ERROR != glGetError()) OS::GetInstance().DebugPrint("GL Error: %d, %s\n", __LINE__, __FILE__);


int formatTable[(int)GPUFormat::Count] =
{
	0,
	GL_RGBA
};

void GPUTextureGL::Destroy()
{
	OS::GetInstance().DebugPrint("Deleted texture\n");
	glDeleteTextures(1, &m_Id);
}

void GPUTextureGL::CreateFromImage(Image& img)
{

	if (m_Width != img.GetWidth() || m_Height != img.GetHeight())
	{
		if (m_Id)
		{
			glDeleteTextures(1, &m_Id);
		}

		glCheck(glGenTextures(1, &m_Id));
		glCheck(glBindTexture(GL_TEXTURE_2D, m_Id));

		glCheck(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.GetWidth(), img.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.GetPixels()));

		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

		glCheck(glBindTexture(GL_TEXTURE_2D, 0));

		this->m_Width = img.GetWidth();
		this->m_Height = img.GetHeight();

		OS::GetInstance().DebugPrint("Creating New GPU Texture\n");
	}
	else
	{
		glCheck(glBindTexture(GL_TEXTURE_2D, m_Id));
		glCheck(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img.GetWidth(), img.GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, img.GetPixels()));
		glCheck(glBindTexture(GL_TEXTURE_2D, 0));

	
	}
}

void GPUTextureGL::CreateFromPixels(void* pixels, uint32_t w, uint32_t h, GPUFormat format)
{
	if (m_Id)
	{
		glDeleteTextures(1, &m_Id);
	}


	glCheck(glGenTextures(1, &m_Id));
	glCheck(glBindTexture(GL_TEXTURE_2D, m_Id));

	glCheck(glTexImage2D(GL_TEXTURE_2D, 0, formatTable[(int)format], w, h, 0, formatTable[(int)format], GL_UNSIGNED_BYTE, pixels));

	glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

	glCheck(glBindTexture(GL_TEXTURE_2D, 0));

	this->m_Width = w;
	this->m_Height = h;
}