#include "fhpch.h"
#include "renderCommand.h"

#include "Platform/OpenGL/openGLRendererAPI.h"

// Creates the renderAPI object
Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::create();