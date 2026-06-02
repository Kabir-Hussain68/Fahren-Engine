#include "fhpch.h"
#include "openGLVertexArray.h"

#include <glad/glad.h>

static GLenum ShaderDataTypeToGLBaseType(ShaderDataType type)
{
    switch (type)
    {
        case ShaderDataType::Float  :  return GL_FLOAT;
        case ShaderDataType::Float2 :  return GL_FLOAT;
        case ShaderDataType::Float3 :  return GL_FLOAT;
        case ShaderDataType::Float4 :  return GL_FLOAT;
        case ShaderDataType::Mat3   :  return GL_FLOAT;
        case ShaderDataType::Mat4   :  return GL_FLOAT;
        case ShaderDataType::Int    :  return GL_INT;
        case ShaderDataType::Int2   :  return GL_INT;
        case ShaderDataType::Int3   :  return GL_INT;
        case ShaderDataType::Int4   :  return GL_INT;
        case ShaderDataType::Bool   :  return GL_BOOL;
    }

    FH_CORE_ASSERT(false, "Unknown shader data type");
    return 0;
}

OpenGLVertexArray::OpenGLVertexArray()
{
    glCreateVertexArrays(1, &m_RendererID);
}

OpenGLVertexArray::~OpenGLVertexArray()
{
    glDeleteVertexArrays(1, &m_RendererID);
}

void OpenGLVertexArray::bind() const
{
    glBindVertexArray(m_RendererID);
}

void OpenGLVertexArray::unBind() const
{
    glBindVertexArray(0);
}

void OpenGLVertexArray::addVertexBuffer(const Ref<VertexBuffer> &vertexBuffer)
{
    FH_CORE_ASSERT(vertexBuffer->getLayout().getElements().size(), "Vertex buffer has no layout");

    glBindVertexArray(m_RendererID);
    vertexBuffer->bind();


    uint32_t index = 0;
    const auto& layout = vertexBuffer->getLayout();
    for (const auto& element : layout)
    {
        //Vertex Layout
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, 
            element.getComponentCount(), 
            ShaderDataTypeToGLBaseType(element.type), 
            element.normalized ? GL_TRUE : GL_FALSE, 
            layout.getStride(), 
            (const void*)(uintptr_t)element.offset);
        index++;
    }

    m_VertexBuffer.push_back(vertexBuffer);
}

void OpenGLVertexArray::setIndexBuffer(const Ref<IndexBuffer> &indexBuffer)
{
    glBindVertexArray(m_RendererID);
    indexBuffer->bind();

    m_IndexBuffer = indexBuffer;
}
