#include "fhpch.h"
#include "graphicsContext.h"

#include "renderer.h"
#include "Platform/OpenGL/openGLContext.h"

Scope<GraphicsContext> GraphicsContext::create(void* window)
{
	switch (Renderer::getAPI())
	{
		case RendererAPI::API::None:    FH_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return createScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
	}

	FH_CORE_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}