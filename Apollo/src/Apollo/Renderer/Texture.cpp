#include "pch.h"

#include "Texture.h"

#include "Apollo/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Apollo
{
	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetRendererAPI())
		{
		case RendererAPI::API::None: APOLLO_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(width, height);
		}
		APOLLO_CORE_ASSERT(false, "Unidentified RendererAPI!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetRendererAPI())
		{
		case RendererAPI::API::None: APOLLO_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(path);
		}
		APOLLO_CORE_ASSERT(false, "Unidentified RendererAPI!");
		return nullptr;
	}
}