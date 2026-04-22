cmake_minimum_required(VERSION 3.20)
project(Engine VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 REQUIRED)

add_executable(engine src/main.cpp src/glad.c)

target_include_directories(engine PRIVATE include)

if (APPLE)
    target_link_libraries(engine PRIVATE
        glfw
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
else()
    target_link_libraries(engine PRIVATE glfw)
endif()
