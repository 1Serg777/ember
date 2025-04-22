include "dependencies.lua"

workspace ( "ember" )
   architecture   ( "x86_64" )
   platforms      ( "x64" ) --check what happens if this is omitted
   configurations ( { "Debug", "Release" } )
   location       ( "build" ) -- location ( "%{wks.location}" ) causes a stack overflow!
   startproject   ( "level-editor" )

include ( external_dependencies_path .. "/glad" )
include ( external_dependencies_path .. "/glfw" )

include ( dev_path .. "/ember")
include ( dev_path .. "/ember-lvl-editor")