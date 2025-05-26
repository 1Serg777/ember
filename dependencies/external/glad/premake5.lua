project("glad")
    kind     ("StaticLib")
    language ("C")
    location (build_path .. "/glad")
    targetdir(build_path .. "/bin/" .. target_dir)
    objdir   (build_path .. "/bin-int/" .. obj_dir)

    includedirs {
        "%{include_dirs.glad}",
    }
    
    files {
        "%{include_dirs.glad}/**.h",
        "%{src_dirs.glad}/**.c",
    }

    filter({"system:windows", "action:vs*"})
        vpaths {
            ["Include/*"] = {
                "%{include_dirs.glad}/**.h",
            },
            ["Sources/*"] = {
                "%{src_dirs.glad}/**.c"
            },
        }

    filter("configurations:Debug")
        defines({"DEBUG", "_DEBUG"})
        runtime("Debug")
        symbols("On")

    filter("configurations:Release")
        defines ({"NDEBUG", "_NDEBUG"})
        runtime ("Release")
        optimize("On")