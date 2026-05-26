#pragma once

#include "Engine/renderer/vertexArray.h"

class OpenGLVertexArray : public VertexArray
{
private:
    uint32_t m_RendererID;
    std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffer;
    std::shared_ptr<IndexBuffer> m_IndexBuffer;

public:
    OpenGLVertexArray();
    virtual ~OpenGLVertexArray();

    virtual void bind() const override;
    virtual void unBind() const override;

    virtual void addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;  
    virtual void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;  

    virtual const std::vector<std::shared_ptr<VertexBuffer>>& getVertexBuffers() const { return m_VertexBuffer; }
    virtual const std::shared_ptr<IndexBuffer>& getIndexBuffers() const { return m_IndexBuffer; }
};