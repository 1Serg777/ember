project ( "ember-lvl-editor" )
    kind       ( "ConsoleApp" )
    language   ( "C++" )
    cppdialect ( "C++17" )
    location   ( build_path .. "/ember-lvl-editor" )
    targetdir  ( build_path .. "/bin/" .. target_dir )
    objdir     ( build_path .. "/bin-int/" .. obj_dir )

    includedirs {
        "%{include_dirs.glm}",
        "%{include_dirs.ember}",
        "%{include_dirs.ember_lvl_editor}",
    }
    libdirs {
        build_path .. "/bin/" .. target_dir
    }
   
    links {
        "ember",
        "glfw", -- On Windows it's enough
        "glad", -- to only link to 'ember'
    }

    files {
        "%{include_dirs.ember_lvl_editor}/**.h",
        "%{src_dirs.ember_lvl_editor}/**.cpp",
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
        -- buildoptions ( { "/utf-8" } ) --spdlog requirement
        vpaths {
            ["Include/*"] = { "%{include_dirs.ember_lvl_editor}/**.h", },
            ["Sources/*"] = { "%{src_dirs.ember_lvl_editor}/**.cpp" },
        }
      
        -- postbuildcommands {
            -- os.execute("copy-shaders.bat")
        -- }

    filter ( "system:linux" )
        defines( { "EMBER_PLATFORM_LINUX" } )

        -- postbuildcommands {
            -- os.execute("copy-shaders.sh")
        -- }