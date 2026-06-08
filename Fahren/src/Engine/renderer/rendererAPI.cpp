#include "fhpch.h"
#include "rendererAPI.h"

#include "Platform/OpenGL/openGLRendererAPI.h"

RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

Scope<RendererAPI> RendererAPI::create()
{
	switch (s_API)
	{
		case RendererAPI::API::None:    FH_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return createScope<OpenGLRendererAPI>();
	}

	FH_CORE_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}