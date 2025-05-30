project("ember-lvl-editor")
    kind      ("ConsoleApp")
    language  ("C++")
    cppdialect("C++17")
    location  (build_path .. "/ember-lvl-editor")
    targetdir (build_path .. "/bin/" .. target_dir)
    objdir    (build_path .. "/bin-int/" .. obj_dir)

    includedirs {
        -- "%{include_dirs.glad}", -- Uncomment if you ever need to work with
        -- "%{include_dirs.glfw}", -- these directly in the project.
        "%{include_dirs.glm}",
        "%{include_dirs.ember}",
        "%{include_dirs.ember_lvl_editor}",
    }
    libdirs {
        build_path .. "/bin/" .. target_dir
    }
   
    links {
        "ember", -- Putting ember to the bottom causes linking errors!
        "glad", -- On Windows it's enough
        "glfw", -- to only link to 'ember'
        "imgui",
    }

    files {
        "%{include_dirs.ember_lvl_editor}/**.h",
        "%{src_dirs.ember_lvl_editor}/**.cpp",
    }

    filter{"system:windows"}
        includedirs{"%{include_dirs.vulkan_win32}"}
        libdirs    {"%{lib_dirs.vulkan_win32}"}
        links      {"vulkan-1"}
    filter{"system:linux"}
        links      {"vulkan"}

    filter("configurations:Debug")
        defines({"DEBUG", "_DEBUG" })
        runtime("Debug")
        symbols("On")

    filter("configurations:Release")
        defines ({"NDEBUG", "_NDEBUG"})
        runtime ("Release")
        optimize("On")

   
    filter("system:windows")
        defines({"EMBER_PLATFORM_WIN32"})

    filter({"system:windows", "action:vs*"})
        -- buildoptions ( { "/utf-8" } ) --spdlog requirement
        vpaths {
            ["Include/*"] = {"%{include_dirs.ember_lvl_editor}/**.h"},
            ["Sources/*"] = {"%{src_dirs.ember_lvl_editor}/**.cpp"},
        }
      
        -- postbuildcommands {
            -- os.execute("copy-shaders.bat")
        -- }

    filter("system:linux")
        defines({"EMBER_PLATFORM_LINUX"})

        -- postbuildcommands {
            -- os.execute("copy-shaders.sh")
        -- }