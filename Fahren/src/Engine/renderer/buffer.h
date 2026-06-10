#pragma once

enum class ShaderDataType
{
    None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
};

static uint32_t ShaderDataTypeSize(ShaderDataType type)
{
    switch (type)
    {
        case ShaderDataType::Float  :  return 4;
        case ShaderDataType::Float2 :  return 4 * 2;
        case ShaderDataType::Float3 :  return 4 * 3;
        case ShaderDataType::Float4 :  return 4 * 4;
        case ShaderDataType::Mat3   :  return 4 * 3 * 3;
        case ShaderDataType::Mat4   :  return 4 * 4 * 4;
        case ShaderDataType::Int    :  return 4;
        case ShaderDataType::Int2   :  return 4 * 2;
        case ShaderDataType::Int3   :  return 4 * 3;
        case ShaderDataType::Int4   :  return 4 * 4;
        case ShaderDataType::Bool   :  return 1;
    }

    FH_CORE_ASSERT(false, "Unknown ShaderDataType");
    return 0;
}

struct BufferElements
{
    BufferElements() {}

    std::string name;
    ShaderDataType type;
    uint32_t offset;
    uint32_t size; 
    bool normalized;

    BufferElements(ShaderDataType type, const std::string& name, bool normalized = false)
        : name(name), type(type), size(ShaderDataTypeSize(type)), offset(0), normalized(normalized)
        {}

    uint32_t getComponentCount() const
    {
        switch(type)
        {
            case ShaderDataType::Float  :  return 1;
            case ShaderDataType::Float2 :  return 2;
            case ShaderDataType::Float3 :  return 3;
            case ShaderDataType::Float4 :  return 4;
            case ShaderDataType::Mat3   :  return 3 * 3;
            case ShaderDataType::Mat4   :  return 4 * 4;
            case ShaderDataType::Int    :  return 1;
            case ShaderDataType::Int2   :  return 2;
            case ShaderDataType::Int3   :  return 3;
            case ShaderDataType::Int4   :  return 4;
            case ShaderDataType::Bool   :  return 1;
        }

        FH_CORE_ASSERT(false, "Unknwon Shader Data Type");
        return 0;
    }
};

class BufferLayout
{
private:
    std::vector<BufferElements> m_Elements;
    uint32_t m_Stride = 0;

    void calculateOffsetsAndStride()
    {
        uint32_t offset = 0;
        m_Stride = 0;
        for (auto& elements : m_Elements)
        {
            elements.offset = offset;
            offset += elements.size;
            m_Stride += elements.size;
        }
    }

public:
    BufferLayout() {}

    inline uint32_t getStride() const { return m_Stride; }
    inline const std::vector<BufferElements>& getElements() const { return m_Elements; } 

    BufferLayout(const std::initializer_list<BufferElements>& elements)
        : m_Elements(elements) 
    {
        calculateOffsetsAndStride();
    }

    std::vector<BufferElements>::iterator begin() { return m_Elements.begin(); }
    std::vector<BufferElements>::iterator end() { return m_Elements.end(); }
    std::vector<BufferElements>::const_iterator begin() const { return m_Elements.begin(); }
    std::vector<BufferElements>::const_iterator end() const { return m_Elements.end(); }
};

class VertexBuffer
{
public:
    virtual ~VertexBuffer() {}

    virtual void bind() const = 0;
    virtual void unBind() const = 0;

    virtual void setData(const void* data, uint32_t size) = 0;

    virtual void setLayout(const BufferLayout& layout) = 0;
    virtual const BufferLayout& getLayout() const = 0;  
    
    static Ref<VertexBuffer> create(uint32_t size);
    static Ref<VertexBuffer> create(float* vertices, uint32_t size);
};

class IndexBuffer
{
public:
    virtual ~IndexBuffer() {}

    virtual void bind() const = 0;
    virtual void unBind() const = 0;

    virtual uint32_t getCount() const = 0;

    static Ref<IndexBuffer> create(uint32_t* indices, uint32_t count);
};