#include "fhpch.h"
#include "renderCommand.h"

#include "Platform/OpenGL/openGLRendererAPI.h"

RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;