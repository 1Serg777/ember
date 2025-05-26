-- Common paths
build_path        = "%{wks.location}"
dependencies_path = "%{wks.location}/../dependencies"
dev_path          = "%{wks.location}/../dev"
tools_path        = "%{wks.location}/../tools"

external_dependencies_path = dependencies_path .. "/external"
internal_dependencies_path = dependencies_path .. "/internal"

target_dir = "%{cfg.buildcfg}-%{cfg.architecture}-%{cfg.system}"
obj_dir    = "%{prj.name}/%{cfg.buildcfg}-%{cfg.architecture}-%{cfg.system}"

-------------------------
-- include directories --
-------------------------
include_dirs = {}

-- external dependencies include directories
include_dirs["glad"]  = external_dependencies_path .. "/glad/include"
include_dirs["glfw"]  = external_dependencies_path .. "/glfw/include"
include_dirs["glm"]   = external_dependencies_path .. "/glm/glm"
include_dirs["imgui"] = external_dependencies_path .. "/imgui"
-- include_dirs["vulkan_win32"] = "C:\\VulkanSDK\\1.4.313.0\\Include"
include_dirs["vulkan_win32"] = _OPTIONS["vulkan_sdk_path"] .. "/Include"
-- internal dependencies include directories
-- TODO

-- dev projects include directories
include_dirs["ember"]            = dev_path .. "/ember/include"
include_dirs["ember_lvl_editor"] = dev_path .. "/ember-lvl-editor/include"

-----------------------------
-- source code directories --
-----------------------------
src_dirs = {}

-- external dependencies source code directories
src_dirs["glad"]  = external_dependencies_path .. "/glad/src"
src_dirs["glfw"]  = external_dependencies_path .. "/glfw/src"
src_dirs["imgui"] = external_dependencies_path .. "/imgui"

-- internal dependencies source code directories
-- TODO

-- dev projects source code directories
src_dirs["ember"] = dev_path .. "/ember/src"
src_dirs["ember_lvl_editor"] = dev_path .. "/ember-lvl-editor/src"

-------------------------
-- library directories --
-------------------------
lib_dirs = {}

-- lib_dirs["vulkan_win32"] = "C:\\VulkanSDK\\1.4.313.0\\Lib"
lib_dirs["vulkan_win32"] = _OPTIONS["vulkan_sdk_path"] .. "/Lib"