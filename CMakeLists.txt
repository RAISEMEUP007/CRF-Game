cmake_minimum_required(VERSION 3.24)
project(node_maze)

# Use C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Point CMake to the location where Conan generates CMake config files
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

# Find required packages
find_package(raylib REQUIRED)
find_package(Catch2 3 REQUIRED)
find_package(flecs REQUIRED)
find_package(di REQUIRED)
find_package(sml REQUIRED)

# Source and test files
set(SOURCE_FILES
    ./src/Main.cpp
    ./src/lib/FloydWarshal.cpp
)

set(TEST_FILES
    test/Test.cpp
    test/lib/FloydWarshalTest.cpp
    src/lib/FloydWarshal.cpp  # Include implementation for tests
)

# Include directories
include_directories(
    src/lib      # For FloydWarshal.h
)

#Add executables
if (WIN32)
  add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES} resource/resource.rc)
else()
  add_executable(${PROJECT_NAME} ${SOURCE_FILES} resource/resource.rc)
endif()

add_executable(node_maze_tests ${TEST_FILES})

# Link libraries
target_link_libraries(${PROJECT_NAME} raylib)
target_link_libraries(${PROJECT_NAME} flecs::flecs_static)
target_link_libraries(${PROJECT_NAME} di::di)
target_link_libraries(${PROJECT_NAME} sml::sml)
target_link_libraries(node_maze_tests Catch2::Catch2WithMain)

# Set output directory for the main executable
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin")

# Copy the resource directory to the build directory after building the main executable
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resource/
        ${CMAKE_BINARY_DIR}/../bin/${CMAKE_BUILD_TYPE}/resource
    COMMENT "Copying resource directory to build directory"
)

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
  target_link_libraries(${PROJECT_NAME} "-framework IOKit")
  target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
  target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()

# Enable testing
enable_testing()
include(Catch)
catch_discover_tests(node_maze_tests)
