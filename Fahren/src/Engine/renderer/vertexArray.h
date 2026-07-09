#pragma once

#include <memory>
#include "Engine/renderer/buffer.h"

// Virtual functions
class VertexArray
{
public:
    virtual ~VertexArray() = default;

    virtual void bind() const = 0;
    virtual void unBind() const = 0;

    virtual void addVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;  
    virtual void setIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;  

    virtual const std::vector<Ref<VertexBuffer>>& getVertexBuffers() const = 0;
    virtual const Ref<IndexBuffer>& getIndexBuffers() const = 0;

    static Ref<VertexArray> create();
};