set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory("nlohmann_json")
add_subdirectory("SFML")
add_subdirectory("tracy")



set_property(GLOBAL PROPERTY TRACY_INC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tracy/public/tracy")