@echo off

set glslc-path=C:\VulkanSDK\1.4.313.0\Bin
set glslc-name=glslc.exe

set glsl-shaders-path=glsl
set spirv-shaders-path=spirv

set app-shader-folder-debug=..\x64\Debug\resource\shaders
set app-shader-folder-release=..\x64\Release\resource\shaders
set app-shader-folder=..\vulkan-engine\resource\shaders

mkdir %spirv-shaders-path%

for %%f in ( %glsl-shaders-path%\*.vert, %glsl-shaders-path%\*.frag ) do (
    %glslc-path%\%glslc-name% %%f -o %spirv-shaders-path%\%%~nf.spv
)

mkdir %app-shader-folder-debug%
mkdir %app-shader-folder-release%
mkdir %app-shader-folder%

echo Debug folder: %app-shader-folder-debug%
copy %spirv-shaders-path%\*.spv %app-shader-folder-debug%

echo Release folder: %app-shader-folder-release%
copy %spirv-shaders-path%\*.spv %app-shader-folder-release%

echo App folder: %app-shader-folder%
copy %spirv-shaders-path%\*.spv %app-shader-folder%

pause;