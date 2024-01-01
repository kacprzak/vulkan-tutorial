cmake_minimum_required(VERSION 3.28)
project(vulkan-tutorial LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

add_subdirectory(src)
add_subdirectory(shaders)

# For clangd support
if(UNIX)
  execute_process(COMMAND ln -sf "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

configure_file(launch.json.in ${CMAKE_SOURCE_DIR}/launch.json)
