add_subdirectory(ThirdParty)

CreateLibrary("Kepler" "Main")
target_link_libraries("Kepler" PUBLIC "spdlog" PRIVATE "glfw")

if(WIN32)
 target_link_libraries("Kepler" PRIVATE 
  "d3d11" 
  "d3d12" 
  "dxgi" 
  "d3dcompiler"
  "dxguid")
endif()