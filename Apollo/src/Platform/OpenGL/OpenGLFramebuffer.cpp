#include "pch.h"

#include "OpenGLFramebuffer.h"

#include "Apollo/Application.h"

#include "glad/glad.h"

namespace Apollo
{
	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{
		Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(1, &m_ColorAttachment);
		glDeleteRenderbuffers(1, &m_DepthAttachment);
	}

	void OpenGLFramebuffer::Invalidate()
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(1, &m_ColorAttachment);
			glDeleteRenderbuffers(1, &m_DepthAttachment);
			m_RendererID = 0;
		}

		glCreateFramebuffers(1, &m_RendererID);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
		glTextureStorage2D(m_ColorAttachment, 1, GL_RGBA8, m_Specification.Width, m_Specification.Height);
		glTextureParameteri(m_ColorAttachment, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_ColorAttachment, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glNamedFramebufferTexture(m_RendererID, GL_COLOR_ATTACHMENT0, m_ColorAttachment, 0);

		glCreateRenderbuffers(1, &m_DepthAttachment);
		glNamedRenderbufferStorage(m_DepthAttachment, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
		glNamedFramebufferRenderbuffer(m_RendererID, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthAttachment);

		APOLLO_CORE_ASSERT(glCheckNamedFramebufferStatus(m_RendererID, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");
	}

	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Window& window = Application::Get().GetWindow();
		glViewport(0, 0, (int)window.GetWidth(), (int)window.GetHeight());
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || (width == m_Specification.Width && height == m_Specification.Height))
			return;

		m_Specification.Width = width;
		m_Specification.Height = height;
		Invalidate();
	}
}
