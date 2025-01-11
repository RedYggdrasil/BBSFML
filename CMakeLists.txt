cmake_minimum_required (VERSION 3.8)

project("BBSFML"  VERSION 1.0)

set(ALL_TARGETS "App" "RSFML")

option( TRACY_ENABLE "" ON)

OPTION( BUILD_SHARED_LIBS
  "Build package with shared libraries."
  OFF
)


OPTION( USE_WIN32
  "Use sfml-main and Win32 window, only for Windows."
  ON
)

# Include sub-projects.

# CMake libraries
add_subdirectory ("CMakeLibs")

# External libraries
add_subdirectory("ELibs")

# Local libraries
add_subdirectory("LLibs")

# The App
add_subdirectory ("App")





# list(APPEND ALL_TARGETS "App")
foreach(TARGET_ENTRY ${ALL_TARGETS})
    # The Locally build libraries
    target_compile_definitions(${TARGET_ENTRY} PUBLIC ${SHARED_COMPILATION_DEFINITIONS})
 endforeach()


# Set app as the Startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "App")
