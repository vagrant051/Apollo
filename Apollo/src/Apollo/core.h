#pragma once

#include <memory>

#ifdef APOLLO_PLATFORM_WINDOWS
	#if APOLLO_DYNAMIC_LINK
		#ifdef APOLLO_BUILD_DLL
			#define APOLLO_API __declspec(dllexport)
		#else
			#define APOLLO_API __declspec(dllimport)
	#endif
	#else
		#define APOLLO_API 
#endif
#else
	#error APOLLO only supports Windows!
#endif


#ifdef APOLLO_ENABLE_ASSERTS
	#define APOLLO_ASSERT(x, ...) { if(!(x)) { APOLLO_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define APOLLO_CORE_ASSERT(x, ...) { if(!(x)) { APOLLO_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define APOLLO_ASSERT(x, ...)
	#define APOLLO_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define APOLLO_BIND_EVENT_FN(fn) std::bind(&fn, this,  std::placeholders::_1)

namespace Apollo
{
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;
}