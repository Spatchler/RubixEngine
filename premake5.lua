workspace "rubixEngine"
    architecture "x64"
    toolset "gcc"

    configurations {
        "debug",
        "release",
        "dist"
    }

outputdir = "%{cfg.buildcfg}/%{cfg.system}/%{cfg.architectrure}" 
project "rubixEngine"
    kind "ConsoleApp"
    language "C++"
    targetname "rubixEngine"
    targetdir ("bin/" .. outputdir)
    objdir ("bin-int/" .. outputdir)

    files {
        "src/**.hpp",
        "src/**.cpp",
        "include/**.hpp",
        "include/**.cpp",
        "deps/src/**.cpp",
        "deps/src/**.c",
        "deps/include/**.h",
        "deps/include/**.hpp"
    }

    includedirs {
        "include",
        "deps/include",
        "/usr/include"
    }

    links {
        "GL",
        --"GLU",
        "glfw",
        --"m",
        --"Xrandr",
        --"Xi",
        --"X11",
        --"Xxf86vm",
        --"pthread",
        --"dl",
        --"Xinerama",
        --"Xcursor",
    }

    libdirs {
        "deps/libs",
        --"/usr/lib"
    }

    postbuildcommands {
        "cp -r res bin/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architectrure}"
    }

    filter "configurations:debug"
        defines {"DEBUG"}
        symbols "On"
    filter "configurations:release"
        defines {"RELEASE"}
        optimize "On"
    filter "configurations:dist"
        defines {"DIST"}
        optimize "On"

    filter "macosx"
        defines {"MACOSX"}

    filter "linux"
        defines {"LINUX"}

    filter "windows"
        defines {"WINDOWS"}
