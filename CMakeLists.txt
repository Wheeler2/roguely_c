cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Roguely VERSION 0.1.0 LANGUAGES C)

# Set output directories for all build types
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/Release)

set(CMAKE_PREFIX_PATH
    "C:/cc++libs/SDL3-3.2.4"
    "C:/cc++libs/SDL3_image-3.2.0"
)

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
find_package(SDL3_image REQUIRED CONFIG)

if(SDL3_FOUND)
message(STATUS "SDL3 found")
else()
message(FATAL_ERROR "SDL3 not found")
endif()

if(SDL3_image_FOUND)
message(STATUS "SDL3_image found")
else()
message(FATAL_ERROR "SDL3_image not found")
endif()

file(GLOB_RECURSE SOURCES "src/*.c" "src/core/*.c")

add_executable(Roguely WIN32 ${SOURCES})

target_link_libraries(Roguely PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)

# Copy SDL3.dll to output directory
add_custom_command(TARGET Roguely POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "C:cc++libs/SDL3-3.2.4/x86_64-w64-mingw32/bin/SDL3.dll"
    "C:/cc++libs/SDL3_image-3.2.0/x86_64-w64-mingw32/bin/SDL3_image.dll"
    $<TARGET_FILE_DIR:Roguely>
)