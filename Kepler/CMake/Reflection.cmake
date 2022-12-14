add_custom_target(FakePrebuildTarget ALL)
set_target_properties(FakePrebuildTarget PROPERTIES FOLDER Rules/Misc)

add_custom_target(Reflection ALL DEPENDS FakePrebuildTarget KEReflector)
set_target_properties(Reflection PROPERTIES FOLDER Rules/Reflection)

add_custom_target(CheckNewFiles ALL DEPENDS KECacheChecker Reflection)
set_target_properties(CheckNewFiles PROPERTIES FOLDER Rules/Reflection)

foreach(ModuleName ${REFLECTED_MODULES})
    add_custom_command(TARGET Reflection PRE_BUILD 
        COMMAND 
            $<TARGET_FILE_DIR:KEReflector>/KEReflector.exe "${CMAKE_SOURCE_DIR}" ${ModuleName}
        DEPENDS 
            KEReflector)

    add_custom_command(TARGET CheckNewFiles PRE_BUILD
        COMMAND 
            $<TARGET_FILE_DIR:KEReflector>/KECacheChecker.exe ${CMAKE_SOURCE_DIR} 1
        DEPENDS 
            KECacheChecker
            Reflection)
        message(STATUS "Reflected module ${ModuleName}")
endforeach()

