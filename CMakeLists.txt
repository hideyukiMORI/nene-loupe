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

neneloupe_target(neneloupe_core core STATIC
    src/core/RgbColor.cpp src/core/ScreenSample.cpp src/core/ScreenPosition.cpp)
neneloupe_target(neneloupe_application application STATIC
    src/application/LoupeFrame.cpp src/application/LoupeController.cpp)
neneloupe_link(neneloupe_application neneloupe_core)

neneloupe_target(loupe_tests unit_tests EXECUTABLE tests/unit/LoupeTests.cpp)
neneloupe_link(loupe_tests neneloupe_core)
neneloupe_link(loupe_tests neneloupe_application)
add_test(NAME loupe_unit COMMAND loupe_tests)

neneloupe_target(neneloupe_capture adapters_win32 STATIC
    src/adapters/win32/CaptureSurface.cpp src/adapters/win32/Win32ScreenSamplerAdapter.cpp)
neneloupe_link(neneloupe_capture neneloupe_core)
neneloupe_link(neneloupe_capture neneloupe_application)
neneloupe_system_link(neneloupe_capture user32)
neneloupe_system_link(neneloupe_capture gdi32)

neneloupe_target(neneloupe_window ui_win32 STATIC
    src/ui/win32/LoupeRenderer.cpp src/ui/win32/LoupeWindow.cpp)
neneloupe_link(neneloupe_window neneloupe_core)
neneloupe_link(neneloupe_window neneloupe_application)
neneloupe_system_link(neneloupe_window user32)
neneloupe_system_link(neneloupe_window gdi32)

neneloupe_target(NeNeLoupe app EXECUTABLE src/app/Main.cpp src/app/NeNeLoupe.manifest)
neneloupe_link(NeNeLoupe neneloupe_core)
neneloupe_link(NeNeLoupe neneloupe_application)
neneloupe_link(NeNeLoupe neneloupe_capture)
neneloupe_link(NeNeLoupe neneloupe_window)
neneloupe_system_link(NeNeLoupe user32)
set_property(TARGET NeNeLoupe PROPERTY WIN32_EXECUTABLE TRUE)
