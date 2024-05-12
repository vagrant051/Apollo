#include "pch.h"

#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Apollo
{
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}