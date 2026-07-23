cmake_minimum_required(VERSION 3.20)
project(AutomationTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(AUTOMATIOTEST_BUILD_QT_APP "Build the Qt SkipAdClicker application when Qt development packages are installed." ON)

if(MSVC)
    add_compile_options(/std:c++latest /FS)

    # Do not rely solely on CMake's toolchain defaults for debug information.
    # Some Visual Studio CMake integrations can initialize the per-configuration
    # flag cache entries as empty, which otherwise produces a "Debug" binary
    # without compiler symbols or a linker PDB.
    add_compile_options(
        $<$<CONFIG:Debug>:/Od>
        $<$<CONFIG:Debug>:/Ob0>
        $<$<CONFIG:Debug>:/Zi>
        $<$<CONFIG:Debug>:/RTC1>
    )
    add_compile_definitions($<$<CONFIG:Debug>:_DEBUG>)
    add_link_options(
        $<$<CONFIG:Debug>:/DEBUG:FULL>
        $<$<CONFIG:Debug>:/INCREMENTAL>
    )
endif()

add_subdirectory(Utilities)
add_subdirectory(OpencvLib)
add_subdirectory(UserInterfaceLib)
add_subdirectory(OcrLib)
add_subdirectory(TestBookLib)
add_subdirectory(PlayTestBook)
if(AUTOMATIOTEST_BUILD_QT_APP)
    add_subdirectory(SkipAdClicker)
endif()
