include("dependencies.lua")

workspace("ember")
   architecture  ("x86_64")
   platforms     ("x64") --check what happens if this is omitted
   configurations({"Debug", "Release"})
   location      ("build") -- location ( "%{wks.location}" ) causes a stack overflow!
   startproject  ("ember-lvl-editor")

include("dependencies/external/glad")
include("dependencies/external/glfw")
include("dependencies/external/imgui")
include("dev/ember")
include("dev/ember-lvl-editor")

newoption {
   trigger     = "vulkan_sdk_path",
   value       = "PATH",
   description = "Provide Vulkan SDK path (Windows-only). If empty, the default 'C:\\Vulkan\\SDK' is used.",
   default     = "C:\\Vulkan\\SDK"
}