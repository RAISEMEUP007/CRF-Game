cmake_minimum_required(VERSION 3.24)
project(node_maze)

# Use C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Point CMake to the location where Conan generates CMake config files
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

find_package(raylib REQUIRED)
find_package(Catch2 3 REQUIRED)
find_package(flecs REQUIRED)

set(SOURCE_FILES
    ./src/Main.cpp)
set(TEST_FILES
    test/Test.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} resource/resource.rc)
add_executable(node_maze_tests ${TEST_FILES})

target_link_libraries(${PROJECT_NAME} raylib)
target_link_libraries(node_maze_tests Catch2::Catch2WithMain)
target_link_libraries(${PROJECT_NAME} flecs::flecs_static)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin")

# Checks if OSX and links appropriate frameworkds (Only reqirued on MacOS)
if (APPLE)
  target_link_libraries(${PROJECT_NAME} "-framework IOKit")
  target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
  target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()

# Enable testing
enable_testing()
add_test(NAME node_maze_tests COMMAND node_maze_tests)
