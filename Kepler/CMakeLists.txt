add_subdirectory(ThirdParty)
add_subdirectory(Shaders)
CreateLibrary("Kepler" "Main")
target_link_libraries("Kepler" 
 PUBLIC 
  "spdlog" 
  "glm" 
  "EnTT"
  "OptickCore"
  "zlibstatic"
  "assimp"
  "rapidjson"
  "imgui"
  "ImGuizmo"
 PRIVATE 
  "glfw" 
  "stb_image" 
  "nfd"
  "miniaudio")

option(ENABLE_EDITOR "Enables editor layer" ON)
target_compile_definitions("Kepler"
 PUBLIC
#  FORCE_REPORT_LIVE_OBJECTS      # Uncomment if you want to see live DXGI objects on shutdown   
  $<$<CONFIG:Debug>:ENABLE_PROFILING>
  $<$<CONFIG:RelWithDebInfo>:ENABLE_PROFILING>
  USE_PMR_ALLOCATORS

  # This is for release builds only, disables windows console window
  $<$<CONFIG:Release>:RELEASE_BUILD>
)

if(ENABLE_EDITOR)
 target_compile_definitions("Kepler" PUBLIC ENABLE_EDITOR)
endif()

target_precompile_headers("Kepler" PRIVATE Source/Core/Pch.h)
if(WIN32)
 target_link_libraries("Kepler" PRIVATE 
  "d3d11" 
  "d3d12" 
  "dxgi" 
  "d3dcompiler"
  "dxguid")
endif()

add_subdirectory(Apps)