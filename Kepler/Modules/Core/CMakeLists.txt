CreateSharedModule("Core" "Main/Modules")
target_link_libraries("Core" 
 PUBLIC 
  "spdlog" 
  "glm" 
  "OptickCore"
  "rapidjson"
 PRIVATE 
  "nfd")

target_compile_definitions("Core"
 PUBLIC
#  FORCE_REPORT_LIVE_OBJECTS      # Uncomment if you want to see live DXGI objects on shutdown   
  $<$<CONFIG:Debug>:ENABLE_PROFILING>
  $<$<CONFIG:RelWithDebInfo>:ENABLE_PROFILING>
  USE_PMR_ALLOCATORS

  # This is for release builds only, disables windows console window
  $<$<CONFIG:Release>:RELEASE_BUILD>
)

target_precompile_headers("Core" PRIVATE Source/Core/Pch.h)