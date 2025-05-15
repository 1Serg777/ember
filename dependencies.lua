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
include_dirs["glad"] = external_dependencies_path .. "/glad/include"
include_dirs["glfw"] = external_dependencies_path .. "/glfw/include"
include_dirs["glm"]  = external_dependencies_path .. "/glm/glm"

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
src_dirs["glad"] = external_dependencies_path .. "/glad/src"
src_dirs["glfw"] = external_dependencies_path .. "/glfw/src"

-- internal dependencies source code directories
-- TODO

-- dev projects source code directories
src_dirs["ember"] = dev_path .. "/ember/src"
src_dirs["ember_lvl_editor"] = dev_path .. "/ember-lvl-editor/src"

-------------------------
-- library directories --
-------------------------
lib_dirs = {}