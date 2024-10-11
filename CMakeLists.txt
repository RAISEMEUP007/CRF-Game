cmake_minimum_required(VERSION 3.24)
project(node_maze)

# Use C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Point CMake to the location where Conan generates CMake config files
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

# Find required packages
find_package(raylib REQUIRED)
find_package(Catch2 3 REQUIRED)
find_package(flecs REQUIRED)
find_package(di REQUIRED)
find_package(nlohmann_json 3.11.3 REQUIRED)
find_package(sml REQUIRED)

# Source and test files
set(SOURCE_FILES
    ./src/Main.cpp
    ./src/Components.hpp
    ./src/interfaces/IExecutes.hpp
    ./src/lib/FloydWarshal.cpp
    ./src/lib/QuadTree.cpp
    ./src/lib/Renderer.cpp
)

set(TEST_FILES
    test/Test.cpp
    test/lib/FloydWarshalTest.cpp
    test/lib/QuadTreeTest.cpp
    test/lib/RendererTest.cpp

    src/lib/FloydWarshal.cpp  # Include implementation for tests
    src/lib/QuadTree.cpp      # Include implementation for tests
    src/lib/Renderer.cpp      # Include implementation for tests
)

# Include directories
include_directories(
    src/lib      # For FloydWarshal.h
    src/interfaces # For IExecutes.hpp
)

#Add executables
if (WIN32)
  add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES} resources/resource.rc)
else()
  add_executable(${PROJECT_NAME} ${SOURCE_FILES} resources/resource.rc)
endif()

add_executable(node_maze_tests ${TEST_FILES})

# Link libraries
target_link_libraries(${PROJECT_NAME} raylib)
target_link_libraries(${PROJECT_NAME} flecs::flecs_static)
target_link_libraries(${PROJECT_NAME} di::di)
target_link_libraries(${PROJECT_NAME} sml::sml)
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)
target_link_libraries(node_maze_tests Catch2::Catch2WithMain)
target_link_libraries(node_maze_tests raylib)
target_link_libraries(node_maze_tests nlohmann_json::nlohmann_json)
target_link_libraries(node_maze_tests flecs::flecs_static)

# Set output directory for the main executable
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin")

# Define the source and destination paths based on the platform
set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources/)
set(RESOURCE_DEST_DIR "")

if(WIN32)
    set(RESOURCE_DEST_DIR ${CMAKE_BINARY_DIR}/../bin/${CMAKE_BUILD_TYPE}/resources)
else()
    set(RESOURCE_DEST_DIR ${CMAKE_BINARY_DIR}/../bin/resources)
endif()

# Add the custom command to copy the resource directory after the target is built
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${RESOURCE_DIR} ${RESOURCE_DEST_DIR}
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
