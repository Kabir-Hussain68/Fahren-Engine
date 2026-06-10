#pragma once

#include "Engine/renderer/buffer.h"

class OpenGLVertexBuffer : public VertexBuffer
{
private:
    uint32_t m_RendererID;
    BufferLayout m_Layout;

public: 
    OpenGLVertexBuffer(uint32_t size);
    OpenGLVertexBuffer(float* vertices, uint32_t size);
    virtual ~OpenGLVertexBuffer();

    virtual void bind() const override;
    virtual void unBind() const override;

    virtual void setData(const void* data, uint32_t size) override;

    virtual void setLayout(const BufferLayout& layout) override { m_Layout = layout; };
    virtual const BufferLayout& getLayout() const override { return m_Layout; };  

};

class OpenGLIndexBuffer : public IndexBuffer
{
private:
    uint32_t m_RendererID;
    uint32_t m_Count;

public: 
    OpenGLIndexBuffer(uint32_t* vertices, uint32_t count);
    virtual ~OpenGLIndexBuffer();

    virtual void bind() const override;
    virtual void unBind() const override;

    virtual uint32_t getCount() const { return m_Count; }

};