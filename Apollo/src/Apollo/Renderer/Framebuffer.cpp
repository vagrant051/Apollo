#include "pch.h"

#include "Framebuffer.h"

#include "Apollo/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Apollo
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetRendererAPI())
		{
		case RendererAPI::API::None: APOLLO_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLFramebuffer>(spec);
		}
		APOLLO_CORE_ASSERT(false, "Unidentified RendererAPI!");
		return nullptr;
	}
}
