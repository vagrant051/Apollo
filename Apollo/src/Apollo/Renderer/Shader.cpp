#include "pch.h"

#include "Shader.h"
#include "Apollo/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include "glad/glad.h"

#include <glm/gtc/type_ptr.hpp>

namespace Apollo
{
	Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetRendererAPI())
		{
		case RendererAPI::API::None: APOLLO_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return new OpenGLShader(vertexSrc, fragmentSrc);
		}
		APOLLO_CORE_ASSERT(false, "Unidentified RendererAPI!");
		return nullptr;
	}
}