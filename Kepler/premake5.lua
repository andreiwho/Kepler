include "ThirdParty"

function Public_Engine_Includes()
    includedirs {
     "%{wks.location}/Kepler/Source",  
     ThirdPartyIncludes.spdlog
    }
end

project "Kepler"  
 kind "StaticLib"   
 language "C++"   
 files { "Source/**.h", "Source/**.cpp" }
 includedirs {
    "Source", 
 }

 Public_Engine_Includes()