workspace "Kepler Engine"
 configurations {"Development", "Debug", "Release"}
 architecture "x64"
 targetdir "%{wks.location}/Bin/%{cfg.buildcfg}/"
 objdir "%{wks.location}/Bin/_Object/%{cfg.buildcfg}/"
 staticruntime "Off" 
 startproject "Testbed"
 cppdialect "C++20"

filter "configurations:Development"
 defines {"DEBUG"}
 symbols "On"
 optimize "Speed"
 
filter "configurations:Debug"
 defines {"DEBUG"}
 symbols "On"
 optimize "Off"
 
filter "configurations:Release"
 defines {"NDEBUG"}
 symbols "Off"
 optimize "Full"

group "Main"
-- Main engine project
 include "Kepler"
 include "Testbed"
group ""