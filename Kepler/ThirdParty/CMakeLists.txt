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

set(TINYGLTF_BUILD_BUILDER_EXAMPLE OFF CACHE BOOL "")
set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF CACHE BOOL "")
set(TINYGLTF_BUILD_VALIDATOR_EXAMPLE OFF CACHE BOOL "")
set(TINYGLTF_BUILD_GL_EXAMPLES OFF CACHE BOOL "")
set(TINYGLTF_INSTALL OFF CACHE BOOL "")
add_subdirectory(tinygltf)
SetDefaultMSVCLib(tinygltf)

set(THIRD_PARTY_PROJECTS 
 glfw 
 stb_image
 tinygltf
 update_mappings 
 uninstall)

foreach(THIRD_PARTY_PROJECT ${THIRD_PARTY_PROJECTS})
 set_target_properties(${THIRD_PARTY_PROJECT} PROPERTIES FOLDER "ThirdParty")
endforeach()

