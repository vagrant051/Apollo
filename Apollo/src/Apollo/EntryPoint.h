#pragma once

#ifdef APOLLO_PLATFORM_WINDOWS

#include <Windows.h>

extern Apollo::Application* Apollo::createApplication();

static void SetWorkingDirectoryToExecutable()
{
	char path[MAX_PATH];
	DWORD length = GetModuleFileNameA(nullptr, path, MAX_PATH);
	if (length == 0 || length >= MAX_PATH)
		return;

	for (int i = (int)length - 1; i >= 0; --i)
	{
		if (path[i] == '\\' || path[i] == '/')
		{
			path[i] = '\0';
			break;
		}
	}
	SetCurrentDirectoryA(path);
}

int main(int argc, char** argv)
{
	SetWorkingDirectoryToExecutable();
	Apollo::Log::Init();
	APOLLO_CORE_INFO("this is core");
	APOLLO_WARN("this is a user interface");
	auto app = Apollo::createApplication();
	app->run();
	delete app;
}

#endif // APOLLO_PLATFORM_WINDOWS