cmake_minimum_required(VERSION 3.20)
project(AutomationTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(AUTOMATIOTEST_BUILD_QT_APP "Build the Qt SkipAdClicker application when Qt development packages are installed." ON)

if(MSVC)
    add_compile_options(/std:c++latest /FS)
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
