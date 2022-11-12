add_library(physx INTERFACE)
target_include_directories(physx INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

if(WIN32)
    target_link_directories(physx 
        INTERFACE "${CMAKE_CURRENT_LIST_DIR}/lib/$<$<CONFIG:Debug>:checked>$<$<CONFIG:Release>:release>/win64/")
    target_link_libraries(physx 
        INTERFACE 
            PhysX_64.lib
            PhysXCommon_64.lib
            PhysXCooking_64.lib
            PhysXFoundation_64.lib
            PVDRuntime_64.lib
            PhysXExtensions_static_64.lib
        )    
endif()

function(GetPhysXDynLibList OutArray)
    set(PREFIX "${CMAKE_SOURCE_DIR}/Dependencies/physx/bin/$<$<CONFIG:Debug>:checked>$<$<CONFIG:Release>:release>/")
    if(WIN32)
        set(LIBS
            "win64/PhysX_64.dll"
            "win64/PhysXCommon_64.dll"
            "win64/PhysXCooking_64.dll"
            "win64/PhysXFoundation_64.dll"
            "win64/PVDRuntime_64.dll"
        )
    endif()

    set(OUT_LIBS)
    foreach(library ${LIBS})
        string(CONCAT outLib "${PREFIX}" "${library}")
        list(APPEND OUT_LIBS "${outLib}")
    endforeach()
    set(${OutArray} "${OUT_LIBS}" PARENT_SCOPE)
endfunction()

function(CopyPhysXDlls ModuleName)
    GetPhysXDynLibList(PHYSX_LIBRARIES)
    foreach(lib ${PHYSX_LIBRARIES})
        get_filename_component(LibName ${lib} NAME)
        add_custom_command(TARGET 
            ${ModuleName} 
                POST_BUILD
            COMMAND 
                ${CMAKE_COMMAND} -E copy_if_different "${lib}" "$<TARGET_FILE_DIR:${ModuleName}>/${LibName}")
    endforeach()
endfunction()