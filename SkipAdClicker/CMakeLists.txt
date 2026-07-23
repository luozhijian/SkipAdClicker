cmake_minimum_required(VERSION 3.20)

find_package(Qt6 QUIET COMPONENTS Widgets)
if(Qt6_FOUND)
    set(AUTOMATIOTEST_QT_WIDGETS_TARGET Qt6::Widgets)
    find_package(Qt6 QUIET COMPONENTS DBus)
    if(TARGET Qt6::DBus)
        set(AUTOMATIOTEST_QT_DBUS_TARGET Qt6::DBus)
    endif()
else()
    find_package(Qt5 REQUIRED COMPONENTS Widgets)
    set(AUTOMATIOTEST_QT_WIDGETS_TARGET Qt5::Widgets)
    find_package(Qt5 QUIET COMPONENTS DBus)
    if(TARGET Qt5::DBus)
        set(AUTOMATIOTEST_QT_DBUS_TARGET Qt5::DBus)
    endif()
endif()

get_target_property(AUTOMATIOTEST_QMAKE_EXECUTABLE ${AUTOMATIOTEST_QT_WIDGETS_TARGET} INTERFACE_QMAKE_EXECUTABLE)
if(AUTOMATIOTEST_QMAKE_EXECUTABLE)
    get_filename_component(AUTOMATIOTEST_QT_BIN_DIR "${AUTOMATIOTEST_QMAKE_EXECUTABLE}" DIRECTORY)
    find_program(AUTOMATIOTEST_WINDEPLOYQT_EXECUTABLE
        NAMES windeployqt windeployqt6
        HINTS "${AUTOMATIOTEST_QT_BIN_DIR}"
        NO_DEFAULT_PATH
    )
endif()
if(NOT AUTOMATIOTEST_WINDEPLOYQT_EXECUTABLE)
    find_program(AUTOMATIOTEST_WINDEPLOYQT_EXECUTABLE
        NAMES windeployqt windeployqt6
        HINTS
            "${Qt6_DIR}/../../../bin"
            "${Qt5_DIR}/../../../bin"
    )
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

file(GLOB_RECURSE AUTOMATIOTEST_APP_HEADERS CONFIGURE_DEPENDS "*.hpp")
file(GLOB_RECURSE AUTOMATIOTEST_APP_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_executable(SkipAdClicker
    ${AUTOMATIOTEST_APP_HEADERS}
    ${AUTOMATIOTEST_APP_SOURCES}
    SkipAdClicker.qrc
)

if(WIN32)
    target_sources(SkipAdClicker PRIVATE SkipAdClicker.rc)
endif()

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

if(AUTOMATIOTEST_QT_DBUS_TARGET)
    target_link_libraries(SkipAdClicker PRIVATE ${AUTOMATIOTEST_QT_DBUS_TARGET})
    target_compile_definitions(SkipAdClicker PRIVATE AUTOMATIOTEST_HAS_QT_DBUS=1)
endif()

if(WIN32)
    target_link_libraries(SkipAdClicker PRIVATE user32 gdi32 wtsapi32 advapi32 taskschd ole32 oleaut32)
endif()

if(WIN32)
    set_target_properties(SkipAdClicker PROPERTIES
        WIN32_EXECUTABLE TRUE
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/x64/$<CONFIG>"
    )
    set(AUTOMATIOTEST_BROWSER_APPLICATIONS_INI
        "${CMAKE_CURRENT_SOURCE_DIR}/BrowserApplications.Windows.ini")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set_target_properties(SkipAdClicker PROPERTIES
        OUTPUT_NAME "skipadclicker"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
    set(AUTOMATIOTEST_BROWSER_APPLICATIONS_INI
        "${CMAKE_CURRENT_SOURCE_DIR}/BrowserApplications.Linux.ini")
elseif(APPLE)
    set_target_properties(SkipAdClicker PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
    set(AUTOMATIOTEST_BROWSER_APPLICATIONS_INI
        "${CMAKE_CURRENT_SOURCE_DIR}/BrowserApplications.macOS.ini")
else()
    message(FATAL_ERROR "SkipAdClicker has no BrowserApplications.ini template for ${CMAKE_SYSTEM_NAME}")
endif()

add_custom_command(TARGET SkipAdClicker POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/tessdata"
        "$<TARGET_FILE_DIR:SkipAdClicker>/tessdata"
	COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/SkipAd/Settings"
        "$<TARGET_FILE_DIR:SkipAdClicker>/SkipAd/Settings"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/SkipAd/SkipAd.txt"
        "$<TARGET_FILE_DIR:SkipAdClicker>/SkipAd/SkipAd.txt"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/SkipAdClicker.ico"
        "$<TARGET_FILE_DIR:SkipAdClicker>/SkipAdClicker.ico"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/SkipAdClicker_ON.ico"
        "$<TARGET_FILE_DIR:SkipAdClicker>/SkipAdClicker_ON.ico"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/SkipAdClicker.ini"
        "$<TARGET_FILE_DIR:SkipAdClicker>/SkipAdClicker.ini"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${AUTOMATIOTEST_BROWSER_APPLICATIONS_INI}"
        "$<TARGET_FILE_DIR:SkipAdClicker>/BrowserApplications.ini"
)

if(WIN32 AND CMAKE_VERSION VERSION_GREATER_EQUAL 3.21)
    add_custom_command(TARGET SkipAdClicker POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:SkipAdClicker>
            $<TARGET_FILE_DIR:SkipAdClicker>
        COMMAND_EXPAND_LISTS
    )
endif()

if(WIN32 AND TARGET Qt6::QWindowsIntegrationPlugin)
    add_custom_command(TARGET SkipAdClicker POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:SkipAdClicker>/platforms"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:Qt6::QWindowsIntegrationPlugin>"
            "$<TARGET_FILE_DIR:SkipAdClicker>/platforms"
    )
endif()

if(WIN32 AND AUTOMATIOTEST_WINDEPLOYQT_EXECUTABLE)
    add_custom_command(TARGET SkipAdClicker POST_BUILD
        COMMAND "${AUTOMATIOTEST_WINDEPLOYQT_EXECUTABLE}"
            --no-translations
            --no-system-d3d-compiler
            --no-opengl-sw
            "$<TARGET_FILE:SkipAdClicker>"
    )
endif()
