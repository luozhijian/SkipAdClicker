file(GLOB_RECURSE AUTOMATIOTEST_UTILITY_HEADERS CONFIGURE_DEPENDS "*.hpp")
file(GLOB_RECURSE AUTOMATIOTEST_UTILITY_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_library(AutomatioTestUtilities
    ${AUTOMATIOTEST_UTILITY_HEADERS}
    ${AUTOMATIOTEST_UTILITY_SOURCES}
)

target_include_directories(AutomatioTestUtilities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    target_link_libraries(AutomatioTestUtilities PUBLIC user32 gdi32)
endif()
