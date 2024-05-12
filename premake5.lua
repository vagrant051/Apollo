workspace "Apollo" 
    architecture "x64" 

    configurations 
    {
        "Debug",
        "Release",
        "Dist"
    }

    startproject "Sandbox"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Apollo/vendor/GLFW/include"
IncludeDir["Glad"] = "Apollo/vendor/Glad/include"
IncludeDir["ImGui"] = "Apollo/vendor/imgui"
IncludeDir["glm"] = "Apollo/vendor/glm"


include "Apollo/vendor/GLFW"
include "Apollo/vendor/Glad"
include "Apollo/vendor/imgui"

project "Apollo" 
    location "Apollo"
    kind "StaticLib" 
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    staticruntime "On"

    pchheader "pch.h"
    pchsource "Apollo/src/pch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl"
    }

    includedirs{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}"
	}

    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib"
    }

    filter "system:windows"
        cppdialect "c++17"
        staticruntime "On"
        systemversion "latest"

        defines 
        {
            "APOLLO_BUILD_DLL",
            "APOLLO_PLATFORM_WINDOWS",
            "APOLLO_ENABLE_ASSERTS",
            "GLFW_INCLUDE_NONE"
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
        }

    filter "configurations:Debug"
        defines "APOLLO_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "APOLLO_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "APOLLO_DIST"
        runtime "Release"
        optimize "on"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "Apollo/vendor/spdlog/include",
        "Apollo/src",
        "%{IncludeDir.glm}",
        "Apollo/vendor"
    }

    links
    {
        "Apollo"
    }

    filter "system:windows"
        cppdialect "c++17"
        systemversion "latest"

        defines
        {
            "APOLLO_PLATFORM_WINDOWS",
        }

    filter "configurations:Debug"
        defines "APOLLO_DEBUG"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines "APOLLO_RELEASE"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        defines "APOLLO_DIST"
        runtime "Release"
        optimize "On"