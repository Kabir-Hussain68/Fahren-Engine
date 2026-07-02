#pragma once

#include "Engine/renderer/frameBuffer.h"

class OpenGLFrameBuffer : public FrameBuffer
{
private:
    uint32_t m_RendererID = 0;
    FrameBufferSpecification m_Specification;

    std::vector<FrameBufferTextureSpecification> m_ColorAttachmentSpecification;
    FrameBufferTextureSpecification m_DepthAttachmentSpecification = FrameBufferTextureFormat::None;

    std::vector<uint32_t> m_ColorAttachments;
    uint32_t m_DepthAttachment = 0;

public:
    OpenGLFrameBuffer(const FrameBufferSpecification spec);
    virtual ~OpenGLFrameBuffer();

    void invalidate();

    virtual const FrameBufferSpecification& getSpecification() const override { return m_Specification; }

    virtual uint32_t getColorAttachmentRendererID(uint32_t index = 0) const override { FH_CORE_ASSERT(index < m_ColorAttachments.size()); return m_ColorAttachments[index]; }

    virtual void bind() override;
    virtual void unBind() override;

    virtual void resize(uint32_t width, uint32_t height) override;
    virtual int readPixel(uint32_t index, int x, int y) override;

    virtual void clearAttachments(uint32_t index, int value); 
};