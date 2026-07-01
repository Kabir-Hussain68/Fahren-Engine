#pragma once

#include "Engine/core/base.h"

struct FrameBufferSpecification
{
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t samples = 1;

    bool swapChainTarget = false;
};

class FrameBuffer
{
private:

public:
    virtual ~FrameBuffer() = default;

    virtual const FrameBufferSpecification& getSpecification() const = 0;

    virtual uint32_t getColorAttachmentRendererID() const = 0;

    virtual void bind() = 0;
    virtual void unBind() = 0;

    virtual void resize(uint32_t width, uint32_t height) = 0;

    static Ref<FrameBuffer> create(const FrameBufferSpecification& spec);
};