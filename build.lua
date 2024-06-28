workspace "Engine"
    configurations { "Debug", "Release" }
    platforms { "x64" }
    language "C++"
    cppdialect "C++17"

group "Dependencies"

project "glad"
    location "Vendor/glad"
    kind "StaticLib"
    language "C"
    targetdir "Binaries/%{cfg.buildcfg}"
    objdir "Binaries-Intermediate/%{cfg.buildcfg}"

    files {
        "Vendor/glad/src/glad.c"
    }

    includedirs {
        "Vendor/glad/include"
    }

project "imgui"
    location "Vendor/imgui"
    kind "StaticLib"
    language "C++"
    targetdir "Binaries/%{cfg.buildcfg}"
    objdir "Binaries-Intermediate/%{cfg.buildcfg}"

    files {
        "Vendor/imgui/*.h",
        "Vendor/imgui/*.cpp",
        "Vendor/imgui/backends/imgui_impl_glfw.cpp",
        "Vendor/imgui/backends/imgui_impl_opengl3.cpp"
    }

    includedirs {
        "vendor/glfw/include",
        "Vendor/imgui"
    }

    links {
        "Vendor/glfw/lib/glfw3.lib",
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

project "tiny_gltf"
    location "Vendor/tiny_gltf"
    kind "StaticLib"
    language "C++"
    targetdir "Binaries/%{cfg.buildcfg}"
    objdir "Binaries-Intermediate/%{cfg.buildcfg}"

    files {
        "Vendor/tiny_gltf/tiny_gltf.h",
        "Vendor/tiny_gltf/tiny_gltf.cc"
    }

    includedirs {
        "Vendor/tiny_gltf",
        "Vendor/json",
        "Vendor/stb_image"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
        
group ""

project "Core"
    location "Core"
    kind "StaticLib"
    language "C++"
    targetdir "Binaries/%{cfg.buildcfg}"
    objdir "Binaries-Intermediate/%{cfg.buildcfg}"

    files {
        "Core/Source/**.h",
        "Core/Source/**.cpp"
    }

    includedirs {
        "Core/Source/",

        -- dependencies
        "vendor/glfw/include",
        "vendor/spdlog/include",
        "vendor/glad/include",
        "vendor/imgui",
        "vendor/json",
        "vendor/stb_image",
        "vendor/tiny_gltf",
        "vendor/entt/src"
    }

    vpaths {
        ["Source"] = { "Core/Source/" },
    }

    links {
        "Vendor/glfw/lib/glfw3.lib",
        "glad",
        "imgui",
        "tiny_gltf"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    targetdir "Binaries/%{cfg.buildcfg}"
    objdir "Binaries-Intermediate/%{cfg.buildcfg}"

    files { 
        "Sandbox/Source/**.h", 
        "Sandbox/Source/**.cpp" 
    }

    includedirs { 
        "Sandbox/Source/",

        -- dependencies
        "Core/Source/",
        "Vendor/glfw/include",
        "Vendor/spdlog/include",
        "Vendor/glad/include",
        "Vendor/imgui",
        "vendor/json",
        "vendor/stb_image",
        "vendor/tiny_gltf",
        "vendor/entt/src"
    }

    vpaths {
        ["Source"] = { "Sandbox/Source/" },
    }

    links {
        "Core"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"