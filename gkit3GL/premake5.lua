dofile "GL.lua"

 -- description des projets		 
projects = {
	"cubeworld"
    ,"test_bezier"
    ,"test_deformations"
    ,"test_ffd"
}

for i, name in ipairs(projects) do
    project(name)
        language "C++"
        kind "ConsoleApp"
        targetdir "bin"
		links { "libgkit3GL" }
		includedirs { ".", "../src", "src" }
		
        files { "projets/" .. name .. ".cpp" }
end