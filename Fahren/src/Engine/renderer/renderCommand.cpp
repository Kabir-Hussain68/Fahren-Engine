#include "fhpch.h"
#include "renderCommand.h"

#include "Platform/OpenGL/openGLRendererAPI.h"

Scope<RendererAPI> RenderCommand::s_RendererAPI = createScope<OpenGLRendererAPI>();