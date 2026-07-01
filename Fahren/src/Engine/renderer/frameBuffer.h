#pragma once

#include "Engine/core/base.h"

enum class FrameBufferTextureFormat
{
    None = 0,

    //Color
    RGBA8,

    //Depth/Stencil
    DEPTH24STENCIL8,

    //Defaults
    Depth = DEPTH24STENCIL8
};

struct FrameBufferTextureSpecification
{
    FrameBufferTextureSpecification() = default;
    FrameBufferTextureSpecification(FrameBufferTextureFormat format)
        : textureFormat(format) {}

    FrameBufferTextureFormat textureFormat = FrameBufferTextureFormat::None;
};

struct FrameBufferAttachmentSpecification
{
    FrameBufferAttachmentSpecification() = default;
    FrameBufferAttachmentSpecification(std::initializer_list<FrameBufferTextureSpecification> attachment)
        : attachments(attachment) {}
    
    std::vector<FrameBufferTextureSpecification> attachments;
};

struct FrameBufferSpecification
{
    uint32_t width = 0;
    uint32_t height = 0;
    FrameBufferAttachmentSpecification attachments;
    uint32_t samples = 1;

    bool swapChainTarget = false;
};

class FrameBuffer
{
private:

public:
    virtual ~FrameBuffer() = default;

    virtual const FrameBufferSpecification& getSpecification() const = 0;

    virtual uint32_t getColorAttachmentRendererID(uint32_t index = 0) const = 0;

    virtual void bind() = 0;
    virtual void unBind() = 0;

    virtual void resize(uint32_t width, uint32_t height) = 0;

    static Ref<FrameBuffer> create(const FrameBufferSpecification& spec);
};