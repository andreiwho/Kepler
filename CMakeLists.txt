cmake_minimum_required(VERSION 3.20)
project("Kepler Engine")
set_property(GLOBAL PROPERTY USE_FOLDERS TRUE)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "ThirdParty/CMake")
set_property(GLOBAL PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<CONFIG:RelWithDebInfo>:Debug>DLL")

if(MSVC)
 add_compile_options(/MP)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/Lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/Lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/Release)

function(SetDefaultMSVCLib ModuleName)
 set_target_properties(${ModuleName} PROPERTIES 
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<CONFIG:RelWithDebInfo>:Debug>DLL")
endfunction()

macro(SetupDefaultProjectProperties ModuleName ModuleFolder)
 target_compile_features(${ModuleName} PUBLIC cxx_std_20)
 target_include_directories(${ModuleName} PUBLIC Source)
 set_target_properties(${ModuleName} PROPERTIES 
  FOLDER ${ModuleFolder}
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<CONFIG:RelWithDebInfo>:Debug>DLL"
  VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )
 target_compile_definitions(${ModuleName} PRIVATE
  # SETUP LOGGING
  $<$<CONFIG:RelWithDebInfo>:ENABLE_LOGGING USE_ASSERT ENABLE_DEBUG ENABLE_VALIDATION_BREAK>
  $<$<CONFIG:Debug>:ENABLE_LOGGING USE_ASSERT ENABLE_DEBUG ENABLE_VALIDATION_BREAK>
 )

 if(WIN32 OR UNIX)
  target_compile_definitions(${ModuleName} PRIVATE PLATFORM_DESKTOP)
 else()
  message(FATAL_ERROR "Unsupported platform detected.")
  target_compile_definitions(${ModuleName} PRIVATE PLATFORM_OTHER)
 endif()
endmacro()

macro(CreateLibrary ModuleName ModuleFolder)
 file(GLOB_RECURSE PROJECT_FILES Source/**.cpp Source/**.h Source/**.inl LIST_DIRECTORIES TRUE)
 add_library(${ModuleName} STATIC ${PROJECT_FILES})
 SetupDefaultProjectProperties(${ModuleName} ${ModuleFolder})
 source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${PROJECT_FILES})
endmacro()

macro(CreateExecutable ModuleName ModuleFolder)
 file(GLOB_RECURSE PROJECT_FILES Source/**.cpp Source/**.h Source/**.inl LIST_DIRECTORIES TRUE)
 add_executable(${ModuleName} ${PROJECT_FILES})
 SetupDefaultProjectProperties(${ModuleName} ${ModuleFolder})
 source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${PROJECT_FILES})
endmacro()

add_subdirectory(Kepler)
add_subdirectory(Testbed)

set_property(DIRECTORY ${CMAKE_CURRENT_LIST_DIR} PROPERTY VS_STARTUP_PROJECT Testbed)