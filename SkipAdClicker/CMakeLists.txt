cmake_minimum_required(VERSION 3.20)

find_package(Qt6 QUIET COMPONENTS Widgets)
if(Qt6_FOUND)
    set(AUTOMATIOTEST_QT_WIDGETS_TARGET Qt6::Widgets)
else()
    find_package(Qt5 REQUIRED COMPONENTS Widgets)
    set(AUTOMATIOTEST_QT_WIDGETS_TARGET Qt5::Widgets)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

file(GLOB_RECURSE AUTOMATIOTEST_APP_HEADERS CONFIGURE_DEPENDS "*.hpp")
file(GLOB_RECURSE AUTOMATIOTEST_APP_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_executable(SkipAdClicker
    ${AUTOMATIOTEST_APP_HEADERS}
    ${AUTOMATIOTEST_APP_SOURCES}
)

target_include_directories(SkipAdClicker PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(SkipAdClicker
    PRIVATE
        ${AUTOMATIOTEST_QT_WIDGETS_TARGET}
        AutomatioTestUtilities
        AutomationTestOpencvLib
        AutomationTestUserInterfaceLib
        AutomatioTestOcrLib
        AutomatioTestTestBookLib
        AutomatioTestPlayTestBook
)

if(WIN32)
    target_link_libraries(SkipAdClicker PRIVATE user32)
endif()

set_target_properties(SkipAdClicker PROPERTIES
    WIN32_EXECUTABLE TRUE
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/x64/$<CONFIG>"
)

add_custom_command(TARGET SkipAdClicker POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/SkipAd"
        "$<TARGET_FILE_DIR:SkipAdClicker>/SkipAd"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/SkipAdClicker.ico"
        "$<TARGET_FILE_DIR:SkipAdClicker>/SkipAdClicker.ico"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/SkipAdClicker_ON.ico"
        "$<TARGET_FILE_DIR:SkipAdClicker>/SkipAdClicker_ON.ico"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/SkipAdClicker.ini"
        "$<TARGET_FILE_DIR:SkipAdClicker>/SkipAdClicker.ini"
)
