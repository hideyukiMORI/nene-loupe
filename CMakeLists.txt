cmake_minimum_required(VERSION 3.31)
project(NeNeLoupe LANGUAGES CXX)

if(NOT MSVC OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(FATAL_ERROR "QLT-011: this gate requires the pinned MSVC compiler")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
find_program(NENETIDY clang-tidy REQUIRED)
set(CMAKE_CXX_CLANG_TIDY "${NENETIDY};--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy;--extra-arg=-Werror=covered-switch-default")

include(eng/targets.cmake)
enable_testing()
neneloupe_target(toolchain_smoke verification EXECUTABLE tests/build/ToolchainSmoke.cpp)
add_test(NAME toolchain_smoke COMMAND toolchain_smoke)
