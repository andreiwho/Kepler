project "Testbed"
 kind "ConsoleApp"
 language "C++"
 files { "Source/**.h", "Source/**.cpp" }

 includedirs {
    "Source/", 
    "../Kepler/Source"
 }

 links {
  "Kepler"
 }