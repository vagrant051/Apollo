#pragma once

#ifdef APOLLO_PLATFORM_WINDOWS

extern Apollo::Application* Apollo::createApplication();
	
int main(int argc, char** argv)
{
	Apollo::Log::Init();
	APOLLO_CORE_INFO("this is core");
	APOLLO_WARN("this is a user interface");
	auto app = Apollo::createApplication();
	app->run();
	delete app;
}

#endif // APOLLO_PLATFORM_WINDOWS