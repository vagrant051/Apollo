#include "pch.h"

#include "OpenGLContext.h"

#include "GLFW/glfw3.h"
#include "glad/glad.h"

namespace Apollo
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		:m_WindowHandle(windowHandle)
	{
		APOLLO_CORE_ASSERT(windowHandle, "windowHandle doesn't exist!");
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		APOLLO_CORE_ASSERT(status, "≥ı ºªØGlad ß∞‹");

		APOLLO_CORE_INFO("OpenGL Vendor: {0}", (char*)glGetString(GL_VENDOR));
		APOLLO_CORE_INFO("OpenGL Renderer: {0}", (char*)glGetString(GL_RENDERER));
		APOLLO_CORE_INFO("OpenGL Version: {0}", (char*)glGetString(GL_VERSION));
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}