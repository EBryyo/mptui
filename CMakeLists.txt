cmake_minimum_required(VERSION 3.15)

project(mptui
    VERSION 0.1.0
    LANGUAGES CXX
)

include(FetchContent)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#Global warning rules

add_library(project_warnings INTERFACE)

if (MSVC)
    target_compile_options(project_warnings INTERFACE
        /W4
        /permissive-
    )
else()
    target_compile_options(project_warnings INTERFACE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

add_subdirectory(src)
