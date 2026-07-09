#include "fhpch.h"
#include "uniformBuffer.h"

#include "Engine/renderer/renderer.h"
#include "Platform/OpenGL/openGLUniformBuffer.h"

// Checks the API for the correct implementation
Ref<UniformBuffer> UniformBuffer::create(uint32_t size, uint32_t binding)
{
    switch (Renderer::getAPI())
		{
			case RendererAPI::API::None:    FH_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return createRef<OpenGLUniformBuffer>(size, binding);
		}

		FH_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
}