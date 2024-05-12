#include "pch.h"
#include "WindowsInput.h"
#include "Apollo/Application.h"

#include "GLFW/glfw3.h"

Apollo::Input* Apollo::Input::s_Instance = new Apollo::WindowsInput();

bool Apollo::WindowsInput::IsKeyPressedImpl(int keycode)
{
	auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	auto state = glfwGetKey(window, keycode);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Apollo::WindowsInput::IsMouseButtonPressedImpl(int button)
{
	auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	auto state = glfwGetKey(window, button);
	return state == GLFW_PRESS;
}

float Apollo::WindowsInput::GetMouseXImpl()
{
	auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	return (float)xpos;
}

float Apollo::WindowsInput::GetMouseYImpl()
{
	auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	return (float)ypos;
}

std::pair<float, float> Apollo::WindowsInput::GetMousePositionImpl()
{
	auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	return { (float)ypos, (float)ypos };
}
