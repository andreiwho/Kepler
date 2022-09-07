add_library(spdlog INTERFACE)
target_include_directories(spdlog INTERFACE "spdlog/include")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
add_subdirectory(glfw)
set_target_properties(glfw PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<CONFIG:RelWithDebInfo>:Debug>DLL")

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE "glm")
target_compile_definitions(glm INTERFACE GLM_FORCE_DEPTH_ZERO_TO_ONE)

add_subdirectory(stb_image)

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "")
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "")
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "")
set(BUILD_SHARED_LIBS OFF)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "")
add_subdirectory(assimp)
SetDefaultMSVCLib(assimp)
SetDefaultMSVCLib(zlibstatic)

add_library(miniaudio INTERFACE)
target_include_directories(miniaudio INTERFACE miniaudio)

add_subdirectory(entt)

add_subdirectory(optick)

add_library(rapidjson INTERFACE)
target_include_directories(rapidjson INTERFACE rapidjson/include)

# IMGUI
add_library(imgui 
 STATIC
  imgui/imgui.h
  imgui/imgui.cpp
  imgui/imconfig.h
  imgui/imgui_demo.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_internal.h
  imgui/imgui_tables.cpp
  imgui/imgui_widgets.cpp
  imgui/imstb_rectpack.h
  imgui/imstb_textedit.h
  imgui/imstb_truetype.h)
target_include_directories(imgui PUBLIC imgui)
target_link_libraries(imgui PRIVATE glfw)

# IMGUIZMO
add_library(ImGuizmo 
 STATIC
  ImGuizmo/GraphEditor.h
  ImGuizmo/GraphEditor.cpp
  ImGuizmo/ImGradient.h
  ImGuizmo/ImGradient.cpp
  ImGuizmo/ImGuizmo.h
  ImGuizmo/ImGuizmo.cpp
  )
target_link_libraries(ImGuizmo PUBLIC imgui)
target_include_directories(ImGuizmo PUBLIC ImGuizmo)

if(WIN32)
 target_sources(imgui 
  PRIVATE 
   imgui/backends/imgui_impl_dx11.h
   imgui/backends/imgui_impl_dx11.cpp
   imgui/backends/imgui_impl_glfw.h
   imgui/backends/imgui_impl_glfw.cpp)
else()
endif()

set(THIRD_PARTY_PROJECTS 
 glfw 
 stb_image
 UpdateAssimpLibsDebugSymbolsAndDLLs
 assimp
 imgui
 ImGuizmo
 zlibstatic
 update_mappings 
 uninstall
 OptickCore)

foreach(THIRD_PARTY_PROJECT ${THIRD_PARTY_PROJECTS})
 set_target_properties(${THIRD_PARTY_PROJECT} PROPERTIES FOLDER "ThirdParty")
endforeach()
