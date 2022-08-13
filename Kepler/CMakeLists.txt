add_subdirectory(ThirdParty)

CreateLibrary("Kepler" "Main")
target_link_libraries("Kepler" PUBLIC "spdlog" PRIVATE "glfw")