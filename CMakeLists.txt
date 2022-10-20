cmake_minimum_required(VERSION 3.24)
project("KE1" CXX CSharp)
include(Kepler/CMake/Build.cmake)

add_subdirectory(Kepler)
if(NOT ENABLE_EDITOR)
 target_compile_definitions("Kepler" PUBLIC "RELEASE_APPLICATION_NAME=\"Testbed\"")
endif()

add_subdirectory(Testbed)

set_property(DIRECTORY ${CMAKE_CURRENT_LIST_DIR} PROPERTY VS_STARTUP_PROJECT Testbed)