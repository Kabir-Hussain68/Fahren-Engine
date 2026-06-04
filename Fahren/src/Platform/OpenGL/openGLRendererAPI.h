#pragma once

#include "Engine/renderer/rendererAPI.h"

class OpenGLRendererAPI : public RendererAPI
{
public:
    virtual void Init() override;

    virtual void setClearColor(const glm::vec4& color) override;
    virtual void clear() override;

    virtual void drawIndexed(const Ref<VertexArray>& vertexArray) override;
};