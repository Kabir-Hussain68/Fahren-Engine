#include "fhpch.h"
#include "openGLFrameBuffer.h"

#include <glad/glad.h>

static const uint32_t s_MaxFrameBufferSize = 8192;

namespace Utils
{
    static GLenum textureTargets(bool multisampled)
    {
    return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    }

    static void createTextures(bool multisampled, uint32_t* outID, uint32_t count)
    {
        glCreateTextures(textureTargets(multisampled), count, outID);
    }

    static void bindTextures(bool multisampled, uint32_t ID)
    {
        glBindTexture(textureTargets(multisampled), ID);
    }

    static void attachColorTexture(uint32_t ID, int samples, GLenum internalformat, GLenum format, uint32_t width, uint32_t height, int index)
    {
        bool multisampled = samples > 1;
        if (multisampled)
        {
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalformat, width, height, GL_FALSE);
        }
        else
        {
            GLenum dataType = (format == GL_RED_INTEGER) ? GL_INT : GL_UNSIGNED_BYTE;
            glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, dataType, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, textureTargets(multisampled), ID, 0);
    }

    static void attachDepthTexture(uint32_t ID, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
    {
        bool multisampled = samples > 1;
        if (multisampled)
        {
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
        }
        else
        {
            glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, textureTargets(multisampled), ID, 0);
    }

    static bool isDepthFormat(FrameBufferTextureFormat format)
    {
        switch (format)
        {
            case FrameBufferTextureFormat::DEPTH24STENCIL8:
            return true;    
        }
        
        return false;
    }

    static GLenum textureFormatToGL(FrameBufferTextureFormat format)
    {
        switch (format)
        {
            case FrameBufferTextureFormat::RGBA8:
                return GL_RGBA8;
            case FrameBufferTextureFormat::RED_INTEGER:
                return GL_RED_INTEGER;
        }

        FH_CORE_ASSERT(false);
        return 0;
    }
}

OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification spec)
    : m_Specification(spec)
{
    for (auto format : m_Specification.attachments.attachments)
    {
        if(!Utils::isDepthFormat(format.textureFormat))
            m_ColorAttachmentSpecification.emplace_back(format);
        else
            m_DepthAttachmentSpecification = format;
    }
    invalidate();
}

OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
    glDeleteFramebuffers(1, &m_RendererID);
    glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
    glDeleteTextures(1, &m_DepthAttachment);
}

void OpenGLFrameBuffer::invalidate()
{
    if (m_Specification.width == 0 || m_Specification.height == 0)
        return;

    if(m_RendererID)
    {
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
        glDeleteTextures(1, &m_DepthAttachment);

        m_ColorAttachments.clear();
        m_DepthAttachment = 0;
    }

    glCreateFramebuffers(1, &m_RendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

    bool multiSample = m_Specification.samples > 1;

    //Attachments
    if (m_ColorAttachmentSpecification.size())
    {
        m_ColorAttachments.resize(m_ColorAttachmentSpecification.size());
        Utils::createTextures(multiSample, m_ColorAttachments.data(), m_ColorAttachments.size());

        for (size_t i = 0; i < m_ColorAttachmentSpecification.size(); i++)
        {
            Utils::bindTextures(multiSample, m_ColorAttachments[i]);
            switch (m_ColorAttachmentSpecification[i].textureFormat)
            {
                case FrameBufferTextureFormat::RGBA8:
                    Utils::attachColorTexture(m_ColorAttachments[i], m_Specification.samples, GL_RGBA8, GL_RGBA, m_Specification.width, m_Specification.height, i);
                    break;
                case FrameBufferTextureFormat::RED_INTEGER:
                    Utils::attachColorTexture(m_ColorAttachments[i], m_Specification.samples, GL_R32I, GL_RED_INTEGER, m_Specification.width, m_Specification.height, i);
                    break;
            }
        }
    }

    if (m_DepthAttachmentSpecification.textureFormat != FrameBufferTextureFormat::None)
    {
        Utils::createTextures(multiSample, &m_DepthAttachment, 1);
        Utils::bindTextures(multiSample, m_DepthAttachment);
        switch (m_DepthAttachmentSpecification.textureFormat)
            {
                case FrameBufferTextureFormat::DEPTH24STENCIL8:
                    Utils::attachDepthTexture(m_DepthAttachment, m_Specification.samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.width, m_Specification.height);
                    break;
            }
    }

    if (m_ColorAttachments.size() > 1)
    {
        FH_CORE_ASSERT(m_ColorAttachments.size() <= 4);
        GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3}; 
        glDrawBuffers(m_ColorAttachments.size(), buffers);
    }
    else if (m_ColorAttachments.empty())
    {
        //Only depth pass
        glDrawBuffer(GL_NONE);
    }

    FH_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFrameBuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
    glViewport(0, 0, m_Specification.width, m_Specification.height);

    int value = -1;
    glClearTexImage(m_ColorAttachments[1], 0, GL_RED_INTEGER, GL_INT, &value);
}

void OpenGLFrameBuffer::unBind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFrameBuffer::resize(uint32_t width, uint32_t height)
{
    if (width == 0 || height == 0 || width > s_MaxFrameBufferSize || height > s_MaxFrameBufferSize)
    {
        FH_CORE_WARN("Attempted to resize framebuffer to {0}, {1}", width, height);
        return;
    }

    m_Specification.width = width;
    m_Specification.height = height;

    invalidate();
}

int OpenGLFrameBuffer::readPixel(uint32_t index, int x, int y)
{
    FH_CORE_ASSERT(index < m_ColorAttachments.size());

    glReadBuffer(GL_COLOR_ATTACHMENT0 + index);
    int pixelData;
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
    return pixelData;
}

void OpenGLFrameBuffer::clearAttachments(uint32_t index, int value)
{
    FH_CORE_ASSERT(index < m_ColorAttachments.size());

    auto& spec = m_ColorAttachmentSpecification[index];
    spec.textureFormat;

    glClearTexImage(m_ColorAttachments[index], 0, Utils::textureFormatToGL(spec.textureFormat), GL_INT, &value);
}
