workspace "OpenGLPhysicsEngine3D"
    architecture "x86"

    configurations { "Debug", "Release" }

    multiprocessorcompile "on"


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "OpenGLPhysicsEngine3D"
    location "OpenGLPhysicsEngine3D"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir("bin/" .. outputdir .. "/%{prj.name}")
    objdir("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "Dependencies/GLFW/include",
        "Dependencies/GLEW/include",
        "Dependencies/helpers/include",
        "OpenGLPhysicsEngine3D/src",
        "OpenGLPhysicsEngine3D/src/vendor"
    }

    filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"GLEW_STATIC",
            "ROTATIONAL_PHYSICS_TEST",
            "ROTATIONAL_PHYSICS_FRICTION"
		}

        links 
        { 
            "glfw3.lib", 
            "glew32s.lib", 
            "opengl32.lib",
            "helpers_dll.lib"
        }

        libdirs 
        { 
            "Dependencies/GLFW/lib", 
            "Dependencies/GLEW/lib/Release/Win32",
            "Dependencies/helpers/lib",
        }

        postbuildcommands
        {
            "{COPY} \"$(SolutionDir)Dependencies/helpers/lib/helpers_dll.dll\" \"$(TargetDir)\""
        }

	filter "configurations:Debug"
		symbols "On"

	filter "configurations:Release"
		optimize "On"

	filter "configurations:Dist"
		optimize "On"
