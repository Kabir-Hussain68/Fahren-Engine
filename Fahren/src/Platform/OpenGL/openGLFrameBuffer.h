#pragma once

#include "Engine/renderer/frameBuffer.h"

class OpenGLFrameBuffer : public FrameBuffer
{
private:
    uint32_t m_RendererID = 0;
    uint32_t m_ColorAttachment = 0;
    uint32_t m_DepthAttachment = 0;
    FrameBufferSpecification m_Specification;

public:
    OpenGLFrameBuffer(const FrameBufferSpecification spec);
    virtual ~OpenGLFrameBuffer();

    void invalidate();

    virtual const FrameBufferSpecification& getSpecification() const override { return m_Specification; }

    virtual uint32_t getColorAttachmentRendererID() const override { return m_ColorAttachment; }

    virtual void bind() override;
    virtual void unBind() override;

    virtual void resize(uint32_t width, uint32_t height) override;
};