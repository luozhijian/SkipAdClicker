file(GLOB_RECURSE AUTOMATIOTEST_USERINTERFACELIB_HEADERS CONFIGURE_DEPENDS "*.hpp")
file(GLOB_RECURSE AUTOMATIOTEST_USERINTERFACELIB_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_library(AutomationTestUserInterfaceLib
    ${AUTOMATIOTEST_USERINTERFACELIB_HEADERS}
    ${AUTOMATIOTEST_USERINTERFACELIB_SOURCES}
)

target_include_directories(AutomationTestUserInterfaceLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(AutomationTestUserInterfaceLib PUBLIC AutomatioTestUtilities AutomationTestOpencvLib)
