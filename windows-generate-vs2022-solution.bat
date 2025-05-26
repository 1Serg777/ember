@echo off
setlocal enabledelayedexpansion

:: set batch_script_path=%~dp0
:: set premake_exe_path=%batch_script_path%tools\windows\premake
set premake_exe_path=tools\windows\premake

set os_premake_flag=--os=windows
set ide_action=vs2022

if not defined VULKAN_SDK (
    set vulkan_sdk_path="C:\Vulkan\SDK"
    echo VULKAN_SDK environment variable doesn't exist. Using the default "C:\Vulkan\SDK" path.
) else (
    set vulkan_sdk_path=%VULKAN_SDK%
    echo Using "%VULKAN_SDK%" path to locate the Vulkan SDK.
)
set vulkan_sdk_option=--vulkan_sdk_path="%vulkan_sdk_path%"

set premake_options=%os_premake_flag% %vulkan_sdk_option%
set premake_action=%ide_action%
set premake_call=%premake_exe_path%\premake5.exe %premake_options% %premake_action%
echo Calling: %premake_call%
call %premake_call%

:: echo Copying "resource" folder...
:: echo D | xcopy /E /Y "resource" "build\ember-lvl-editor\resource"
:: xcopy /I /E "resource" "build\ember-lvl-editor\resource"
if not exist "build\ember-lvl-editor\resource" (
    echo Copying the "resource" folder...
    :: echo D | xcopy /E "resource" "build\ember-lvl-editor\resource"
    xcopy /I /E "resource" "build\ember-lvl-editor\resource"
) else (
    set /p response="The 'resource' folder already exists, do you want to overwrite its contents? [Y-Yes, N-No] "
    :: echo "!response!"
    if /I "!response!"=="Y" (
        echo Overwriting the "resource" folder in "build\ember-lvl-editor\resource"...
        :: xcopy /E /Y "resource" "build\ember-lvl-editor\resource"
        xcopy /E /Y "resource" "build\ember-lvl-editor\resource"
    ) else (
        echo Leaving the "resource" folder in "build\ember-lvl-editor\resource" as is.
    )
)