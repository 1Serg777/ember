project ( "ember" )
    kind       ( "StaticLib" )
    language   ( "C++" )
    cppdialect ( "C++17" )
    location   ( build_path .. "/ember" )
    targetdir  ( build_path .. "/bin/" .. target_dir )
    objdir     ( build_path .. "/bin-int/" .. obj_dir )

    includedirs {
        "%{include_dirs.glad}",
        "%{include_dirs.glfw}",
        "%{include_dirs.glm}",
        "%{include_dirs.ember}",
    }
    libdirs {
        build_path .. "/bin/" .. target_dir
    }

    links {
        "glad",
        "glfw",
    }

    files {
        "%{include_dirs.ember}/**.h",
        "%{src_dirs.ember}/**.cpp",
    }

    filter ( "configurations:Debug" )
        defines ( { "DEBUG", "_DEBUG" } )
        runtime ( "Debug" )
        symbols ( "On" )

    filter ( "configurations:Release" )
        defines  ( { "NDEBUG", "_NDEBUG" } )
        runtime  ( "Release" )
        optimize ( "On" )

    filter ( "system:windows" )
        defines( { "EMBER_PLATFORM_WIN32" } )

    filter ( { "system:windows", "action:vs*" } )
        buildoptions ( { "/utf-8" } ) --spdlog requirement
        vpaths {
            ["Include/*"] = { "%{include_dirs.ember}/**.h", },
            ["Sources/*"] = { "%{src_dirs.ember}/**.cpp" },
        }
      
        -- postbuildcommands {
            -- os.execute("copy-shaders.bat")
        -- }

    filter ( "system:linux" )
        defines( { "EMBER_PLATFORM_LINUX" } )

        -- postbuildcommands {
            -- os.execute("copy-shaders.sh")
        -- }