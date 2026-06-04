#include "fhpch.h"
#include "openGLTexture.h"

#include "stb_image.h"

#include "glad/glad.h"

OpenGLTexture2D::OpenGLTexture2D(const std::string &path)
    : m_Path(path)
{
    int width, height, nrChannels;

    stbi_set_flip_vertically_on_load(1);
    stbi_uc* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    FH_CORE_ASSERT(data, "Failed to load image!");
    m_Width = width;
    m_Height = height;

    GLenum internalFormat = 0, dataFormat = 0;
    if (nrChannels == 4 )
    {
        internalFormat = GL_RGBA8;
        dataFormat = GL_RGBA;
    }
    else if (nrChannels == 3)
    {
        internalFormat = GL_RGB8;
        dataFormat = GL_RGB;
    }

    FH_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

    glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
    glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

    glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
}

OpenGLTexture2D::~OpenGLTexture2D()
{
    glDeleteTextures(1, &m_RendererID);
}

void OpenGLTexture2D::bind(uint32_t slot) const
{
    glBindTextureUnit(slot, m_RendererID);
}
