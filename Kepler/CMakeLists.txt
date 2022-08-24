add_subdirectory(ThirdParty)
add_subdirectory(Shaders)
CreateLibrary("Kepler" "Main")
target_link_libraries("Kepler" PUBLIC "spdlog" "glm" PRIVATE "glfw")

if(WIN32)
 target_link_libraries("Kepler" PRIVATE 
  "d3d11" 
  "d3d12" 
  "dxgi" 
  "d3dcompiler"
  "dxguid"
  "glm")
endif()