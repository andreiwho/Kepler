file(GLOB_RECURSE PROJECT_FILES **.hlsl LIST_DIRECTORIES TRUE)
add_custom_target("Shaders" SOURCES ${PROJECT_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${PROJECT_FILES})
set_target_properties("Shaders" PROPERTIES FOLDER Main)