add_subdirectory(ThirdParty)
add_subdirectory(Shaders)
CreateLibrary("Kepler" "Main")
target_link_libraries("Kepler" 
 PUBLIC 
  "spdlog" 
  "glm" 
  "tinygltf"
  "EnTT"
  "OptickCore"
  "rapidjson"
 PRIVATE 
  "glfw" 
  "stb_image" 
  "miniaudio")
target_compile_definitions("Kepler"
 PUBLIC
#  FORCE_REPORT_LIVE_OBJECTS      # Uncomment if you want to see live DXGI objects on shutdown   
  $<$<CONFIG:Debug>:ENABLE_PROFILING>
  $<$<CONFIG:RelWithDebInfo>:ENABLE_PROFILING>
)

target_precompile_headers("Kepler" PRIVATE Source/Core/Pch.h)
if(WIN32)
 target_link_libraries("Kepler" PRIVATE 
  "d3d11" 
  "d3d12" 
  "dxgi" 
  "d3dcompiler"
  "dxguid")
endif()