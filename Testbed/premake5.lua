project "Testbed"
 kind "ConsoleApp"
 language "C++"
 files { "Source/**.h", "Source/**.cpp" }

 includedirs {
    "Source/", 
 }

 Public_Engine_Includes()

 links {
  "Kepler"
 }