include(CSharpUtilities)

#############################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS TRUE)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "ThirdParty/CMake")
set_property(GLOBAL PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<CONFIG:RelWithDebInfo>:Debug>DLL")
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING INTERNAL FORCE)

#############################################################################
if(MSVC)
    add_compile_options(/MP)
endif()

#############################################################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/Lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/Lib)

#############################################################################
function(SetDefaultMSVCLib ModuleName)
    set_target_properties(${ModuleName} PROPERTIES 
        MSVC_RUNTIME_LIBRARY 
            "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<CONFIG:RelWithDebInfo>:Debug>DLL")
endfunction()

#############################################################################
macro(SetupDefaultProjectProperties ModuleName ModuleFolder)
    target_compile_features(${ModuleName} PUBLIC cxx_std_20)
    target_include_directories(${ModuleName} PUBLIC Source Generated)
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
    
    add_dependencies(${ModuleName} Reflection CheckNewFiles)

    if(WIN32 OR UNIX)
        target_compile_definitions(${ModuleName} PRIVATE PLATFORM_DESKTOP)
    else()
        target_compile_definitions(${ModuleName} PRIVATE PLATFORM_OTHER)
        message(FATAL_ERROR "Unsupported platform detected.")
    endif()
    
    file(GLOB GENERATED_FILES Generated/**.h Generated/**.cpp LIST_DIRECTORIES TRUE)
    target_sources(${ModuleName} PRIVATE ${GENERATED_FILES})
    source_group(Generated FILES ${GENERATED_FILES})
    # Append list of reflected modules with this one
    set(REFLECTED_MODULES ${REFLECTED_MODULES} ${ModuleName} PARENT_SCOPE)
    add_dependencies(${ModuleName} Reflection CheckNewFiles)
endmacro()

#############################################################################
macro(CreateLibrary ModuleName ModuleFolder)
    file(GLOB_RECURSE PROJECT_FILES Source/**.cpp Source/**.h Source/**.inl LIST_DIRECTORIES TRUE)
    add_library(${ModuleName} STATIC ${PROJECT_FILES})
    SetupDefaultProjectProperties(${ModuleName} ${ModuleFolder})
    source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${PROJECT_FILES})
endmacro()

#############################################################################
macro(CreateExecutable ModuleName ModuleFolder)
    file(GLOB_RECURSE PROJECT_FILES Source/**.cpp Source/**.h Source/**.inl LIST_DIRECTORIES TRUE)
    if(WIN32)
        add_executable(${ModuleName} WIN32 ${PROJECT_FILES})
    else()
        add_executable(${ModuleName} ${PROJECT_FILES})
    endif()
    SetupDefaultProjectProperties(${ModuleName} ${ModuleFolder})
    source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${PROJECT_FILES})
    set_target_properties(${ModuleName} PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "${ModuleName}")
endmacro()

#############################################################################
macro(CreateCSharpExecutable ModuleName ModuleFolder)
    file(GLOB_RECURSE PROJECT_FILES Source/**.cs LIST_DIRECTORIES TRUE)
    add_executable(${ModuleName} ${PROJECT_FILES})
    set_target_properties(${ModuleName} PROPERTIES 
        FOLDER ${ModuleFolder}
        VS_DEBUGGER_COMMAND_ARGUMENTS "${CMAKE_SOURCE_DIR} ${ModuleName}"
    )
    set_target_properties(${ModuleName} PROPERTIES
        DOTNET_SDK "Microsoft.NET.Sdk"
        DOTNET_TARGET_FRAMEWORK "net6.0")

    if(ENABLE_EDITOR)
        target_compile_definitions(${ModuleName} PUBLIC ENABLE_EDITOR)
    endif()
endmacro()