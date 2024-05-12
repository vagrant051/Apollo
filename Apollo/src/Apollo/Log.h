#pragma once

#include "core.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Apollo
{
	class APOLLO_API Log
	{
	public:
		static void Init();

		inline static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};
}

#define APOLLO_CORE_ERROR(...)		::Apollo::Log::GetCoreLogger()->error(__VA_ARGS__)
#define APOLLO_CORE_WARN(...)		::Apollo::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define APOLLO_CORE_INFO(...)		::Apollo::Log::GetCoreLogger()->info(__VA_ARGS__)
#define APOLLO_CORE_TRACE(...)		::Apollo::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define APOLLO_CORE_FATAL(...)		::Apollo::Log::GetCoreLogger()->fatal(__VA_ARGS__)

#define APOLLO_ERROR(...)			::Apollo::Log::GetClientLogger()->error(__VA_ARGS__)
#define APOLLO_WARN(...)			::Apollo::Log::GetClientLogger()->warn(__VA_ARGS__)
#define APOLLO_INFO(...)			::Apollo::Log::GetClientLogger()->info(__VA_ARGS__)
#define APOLLO_TRACE(...)			::Apollo::Log::GetClientLogger()->trace(__VA_ARGS__)
#define APOLLO_FATAL(...)			::Apollo::Log::GetClientLogger()->fatal(__VA_ARGS__)

