dofile "gkit.lua"

 -- description des projets		 
projects = {
}
--[[
for i, name in ipairs(projects) do
    project(name)
        language "C++"
        kind "ConsoleApp"
        targetdir "bin"
        files ( gkit_files )
        files { "projets/" .. name .. ".cpp" }
end

project("bezier")
  language "C++"
  kind "ConsoleApp"
  targetdir "bin"
  buildoptions ( "-std=c++17" )
  files ( gkit_files )
  files { "projets/tp3/bezier-etu/src/*.cpp" }]]



project("camera")
  language "C++"
  kind "ConsoleApp"
  targetdir "bin"
  buildoptions ( "-std=c++17" )
  files ( gkit_files )
  files { "projets/tp2/camera-etu/src/*.cpp" }
  
project("bvh")
  language "C++"
  kind "ConsoleApp"
  targetdir "bin"
  buildoptions ( "-std=c++17" )
  files ( gkit_files )
  files { "projets/tp4/bvh-etu/src/*.cpp" }
