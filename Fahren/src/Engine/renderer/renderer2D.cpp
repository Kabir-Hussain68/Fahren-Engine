#include "fhpch.h"
#include "renderer2D.h"

#include "vertexArray.h"
#include "shader.h"
#include "renderCommand.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct QuadVertex
{
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 texCoord;
    float texIndex;   
    float tilingFactor;
};

struct Renderer2DData
{
    static const uint32_t maxQuads = 10000;
    static const uint32_t maxVertices = maxQuads * 4;
    static const uint32_t maxIndices = maxQuads * 6;
    static const uint32_t maxTextureSlots = 32;

    Ref<VertexArray> quadVertexArray;
    Ref<VertexBuffer> quadVertexBuffer;
    Ref<Shader> textureShader;
    Ref<Texture2D> whiteTexture;

    uint32_t quadIndexCount = 0;
    QuadVertex* quadVertexBufferBase = nullptr;
    QuadVertex* quadVertexBufferPtr = nullptr;

    std::array<Ref<Texture2D>, maxTextureSlots> textureSlots; 
    uint32_t textureSlotIndex = 1; // 0 = white texture

    glm::vec4 quadVertexPositions[4];

    Renderer2D::Statistics stats;
};

static Renderer2DData* s_Data;

void Renderer2D::startBatch()
{
    s_Data->quadIndexCount = 0;
	s_Data->quadVertexBufferPtr= s_Data->quadVertexBufferBase;

	s_Data->textureSlotIndex = 1;
}

void Renderer2D::Init()
{
    FH_PROFILE_FUNCTION();

    s_Data = new Renderer2DData();

    //VAO
    s_Data->quadVertexArray = VertexArray::create();

    s_Data->quadVertexBuffer = VertexBuffer::create(s_Data->maxVertices * sizeof(QuadVertex));
    s_Data->quadVertexBuffer->setLayout({
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float4, "a_Color" },
        { ShaderDataType::Float2, "a_TexCoord" },
        { ShaderDataType::Float, "a_TexIndex"},
        { ShaderDataType::Float, "a_TilingFactor"}
    });
    s_Data->quadVertexArray->addVertexBuffer(s_Data->quadVertexBuffer);

    s_Data->quadVertexBufferBase = new QuadVertex[s_Data->maxIndices];
    
    uint32_t* quadIndicies  = new uint32_t[s_Data->maxIndices];

    uint32_t offset = 0;
    for (uint32_t i = 0; i < s_Data->maxIndices; i += 6)
    {
        quadIndicies[i + 0] = offset + 0;
        quadIndicies[i + 1] = offset + 1;
        quadIndicies[i + 2] = offset + 2;
        
        quadIndicies[i + 3] = offset + 2;
        quadIndicies[i + 4] = offset + 3;
        quadIndicies[i + 5] = offset + 0;

        offset += 4;
    }

    Ref<IndexBuffer> quadIB;
    quadIB = IndexBuffer::create(quadIndicies, s_Data->maxIndices);
    s_Data->quadVertexArray->setIndexBuffer(quadIB);
    
    delete[] quadIndicies;

    s_Data->whiteTexture = Texture2D::create(1, 1);
    uint32_t whiteTextureData = 0xffffffff;
    s_Data->whiteTexture->setData(&whiteTextureData, sizeof(uint32_t));

    int32_t samplers[s_Data->maxTextureSlots];
    for (uint32_t i = 0; i < s_Data->maxTextureSlots; i++)
    {
        samplers[i] = i;
    }

    s_Data->textureShader = Shader::create("assets/shaders/texture.glsl");
    s_Data->textureShader->bind();
    s_Data->textureShader->setIntArray("u_Textures", samplers, s_Data->maxTextureSlots);

    //Setting first texture as the white texture    
    s_Data->textureSlots[0] = s_Data->whiteTexture;

    s_Data->quadVertexPositions[0] = {-0.5f, -0.5f, 0.0f, 1.0f};
    s_Data->quadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f};
    s_Data->quadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f};
    s_Data->quadVertexPositions[3] = {-0.5f,  0.5f, 0.0f, 1.0f};
}

void Renderer2D::shutdown()
{
    FH_PROFILE_FUNCTION();

    delete s_Data;
}

void Renderer2D::beginScene(const Camera &camera, const glm::mat4 &transform)
{
    FH_PROFILE_FUNCTION();

    glm::mat4 viewProj = camera.getProjection() * glm::inverse(transform);

    s_Data->textureShader->bind();
    s_Data->textureShader->setMat4("u_ViewProjection", viewProj);

    startBatch();
}

void Renderer2D::beginScene(const EditorCamera& camera)
{
    FH_PROFILE_FUNCTION();

    glm::mat4 viewProj = camera.getViewProjectionMatrix();

    s_Data->textureShader->bind();
    s_Data->textureShader->setMat4("u_ViewProjection", viewProj);

    startBatch();
}

void Renderer2D::beginScene(const OrthographicCamera &camera)
{
    FH_PROFILE_FUNCTION();

    s_Data->textureShader->bind();
    s_Data->textureShader->setMat4("u_ViewProjection", camera.getViewProjectionMatrix());

    startBatch();   
}

void Renderer2D::endScene()
{
    FH_PROFILE_FUNCTION();

    uint32_t dataSize = (uint32_t)((uint8_t*)s_Data->quadVertexBufferPtr - (uint8_t*)s_Data->quadVertexBufferBase);
    s_Data->quadVertexBuffer->setData(s_Data->quadVertexBufferBase, dataSize);

    flush();
}

void Renderer2D::flush()
{
    //Nothing to draw
    if (s_Data->quadIndexCount == 0)
        return;

    uint32_t dataSize = (uint32_t)((uint8_t*)s_Data->quadVertexBufferPtr - (uint8_t*)s_Data->quadVertexBufferBase);
	s_Data->quadVertexBuffer->setData(s_Data->quadVertexBufferBase, dataSize);

    //Bind textures
    for (uint32_t i = 0; i < s_Data->textureSlotIndex; i++)
    {
        s_Data->textureSlots[i]->bind(i);
    }
    RenderCommand::drawIndexed(s_Data->quadVertexArray, s_Data->quadIndexCount);
    s_Data->stats.drawCalls++;
}

void Renderer2D::nextBatch()
{
    flush();
    startBatch();
}

void Renderer2D::drawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color)
{
    drawQuad({position.x, position.y, 0.0f}, size, color);
}

void Renderer2D::drawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color)
{    
    FH_PROFILE_FUNCTION();

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                          glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f}); 

    drawQuad(transform, color);
}

void Renderer2D::drawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<Texture2D> &texture, float tilingFactor, const glm::vec4 tintColor)
{
    drawQuad({position.x, position.y, 0.0f}, size, texture, tilingFactor, tintColor);
}

void Renderer2D::drawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<Texture2D> &texture, float tilingFactor, const glm::vec4 tintColor)
{
    FH_PROFILE_FUNCTION();

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                          glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f}); 

    drawQuad(transform, texture, tilingFactor, tintColor);
}

void Renderer2D::drawQuad(const glm::mat4 transform, const glm::vec4 color)
{
    FH_PROFILE_FUNCTION();

    constexpr size_t quadVertexCount = 4; 
    const float textureIndex = 0.0f; //0 is white texture
    constexpr glm::vec2 texCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } }; 
    const float tilingFactor = 1.0f;

    if (s_Data->quadIndexCount >= Renderer2DData::maxIndices)
    {
        nextBatch();
    }

    for (size_t i = 0; i < quadVertexCount; i++)
    {
        s_Data->quadVertexBufferPtr->position = transform * s_Data->quadVertexPositions[i];
        s_Data->quadVertexBufferPtr->color = color;
        s_Data->quadVertexBufferPtr->texCoord = texCoords[i];
        s_Data->quadVertexBufferPtr->texIndex = textureIndex;
        s_Data->quadVertexBufferPtr->tilingFactor = tilingFactor;
        s_Data->quadVertexBufferPtr++;
    }

    s_Data->quadIndexCount += 6;

    s_Data->stats.quadCount++; 
}

void Renderer2D::drawQuad(const glm::mat4 transform, const Ref<Texture2D> &texture, float tilingFactor, const glm::vec4 tintColor)
{
    FH_PROFILE_FUNCTION();

	constexpr size_t quadVertexCount = 4;
	constexpr glm::vec2 texCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

    if (s_Data->quadIndexCount >= Renderer2DData::maxIndices)
    {
        nextBatch();
    }

    float textureIndex = 0.0f;

    for (uint32_t i = 1; i < s_Data->textureSlotIndex; i++)
    {
        if (*s_Data->textureSlots[i] == *texture)
        {
            textureIndex = (float)i;
            break; 
        }   
    }

    if (textureIndex == 0)
    {
        if (s_Data->textureSlotIndex >= Renderer2DData::maxTextureSlots)
        {
           nextBatch();
        }
            
        textureIndex = (float)s_Data->textureSlotIndex;
        s_Data->textureSlots[s_Data->textureSlotIndex] = texture;   
        s_Data->textureSlotIndex++;
    }

    for (size_t i = 0; i < quadVertexCount; i++)
    {
        s_Data->quadVertexBufferPtr->position = transform * s_Data->quadVertexPositions[i];
        s_Data->quadVertexBufferPtr->color = tintColor;
        s_Data->quadVertexBufferPtr->texCoord = texCoords[i];
        s_Data->quadVertexBufferPtr->texIndex = textureIndex;
        s_Data->quadVertexBufferPtr->tilingFactor = tilingFactor;
        s_Data->quadVertexBufferPtr++;
    }

    s_Data->quadIndexCount += 6;

    s_Data->stats.quadCount++; 
}

void Renderer2D::drawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, float rotation, const glm::vec4 &color)
{
    drawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, color);
}

void Renderer2D::drawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation, const glm::vec4 &color)
{
    FH_PROFILE_FUNCTION();

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(rotation), {0.0f, 0.0f, 1.0f}) *
                          glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f}); 

    drawQuad(transform, color);
}

void Renderer2D::drawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, float rotation, const Ref<Texture2D> &texture, float tilingFactor, const glm::vec4 tintColor)
{
    drawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, texture, tilingFactor, tintColor);
}

void Renderer2D::drawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation, const Ref<Texture2D> &texture, float tilingFactor, const glm::vec4 tintColor)
{
    FH_PROFILE_FUNCTION();  
 

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(rotation), {0.0f, 0.0f, 1.0f}) *
                          glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f}); 

    drawQuad(transform, texture, tilingFactor, tintColor);
}

void Renderer2D::resetStats()
{
    memset(&s_Data->stats, 0, sizeof(Statistics));
}

Renderer2D::Statistics Renderer2D::getStats()
{
    return s_Data->stats;
}
