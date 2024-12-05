dofile "GL.lua"

 -- description des projets		 
projects = {
	"abcd",
	"cubeworld"
    ,"test_bezier"
    ,"test_deformations"
    ,"test_ffd",
    "test_implicite_surface"
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
