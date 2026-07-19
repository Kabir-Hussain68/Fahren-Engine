#pragma once

#include "Engine/renderer/rendererAPI.h"

class CommandBuffer;

class VulaknRendererAPI : public RendererAPI
{
private:
    Ref<CommandBuffer> m_ActiveCommandBuffer;
    glm::vec4 m_PendingClearColor;

public:
    virtual void Init() override;
    virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

    virtual void setClearColor(const glm::vec4& color) override;
    virtual void clear() override;

    virtual void drawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
    virtual void drawLines(const Ref<VertexArray> &vertexArray, uint32_t vertexCount) override;

    virtual void setLineThickness(float thickness) override;
};
