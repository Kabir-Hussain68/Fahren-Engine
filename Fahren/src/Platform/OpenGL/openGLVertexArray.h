#pragma once

#include "Engine/renderer/vertexArray.h"

class OpenGLVertexArray : public VertexArray
{
private:
    uint32_t m_RendererID;
    std::vector<Ref<VertexBuffer>> m_VertexBuffer;
    Ref<IndexBuffer> m_IndexBuffer;

public:
    OpenGLVertexArray();
    virtual ~OpenGLVertexArray();

    virtual void bind() const override;
    virtual void unBind() const override;

    virtual void addVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;  
    virtual void setIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;  

    virtual const std::vector<Ref<VertexBuffer>>& getVertexBuffers() const { return m_VertexBuffer; }
    virtual const Ref<IndexBuffer>& getIndexBuffers() const { return m_IndexBuffer; }
};