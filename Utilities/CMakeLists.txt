file(GLOB_RECURSE AUTOMATIOTEST_UTILITY_HEADERS CONFIGURE_DEPENDS "*.hpp")
file(GLOB_RECURSE AUTOMATIOTEST_UTILITY_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_library(AutomatioTestUtilities
    ${AUTOMATIOTEST_UTILITY_HEADERS}
    ${AUTOMATIOTEST_UTILITY_SOURCES}
)

target_include_directories(AutomatioTestUtilities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    target_link_libraries(AutomatioTestUtilities PUBLIC user32 gdi32)
elseif(APPLE)
    target_link_libraries(AutomatioTestUtilities PUBLIC "-framework ApplicationServices")
elseif(UNIX)
    find_package(X11 REQUIRED)
    find_path(AUTOMATIOTEST_XTEST_INCLUDE_DIR X11/extensions/XTest.h)
    find_library(AUTOMATIOTEST_XTEST_LIBRARY Xtst)
    if(NOT AUTOMATIOTEST_XTEST_INCLUDE_DIR OR NOT AUTOMATIOTEST_XTEST_LIBRARY)
        message(FATAL_ERROR "Linux mouse automation needs XTest. Install libxtst-dev or the equivalent package.")
    endif()
    target_include_directories(AutomatioTestUtilities PUBLIC ${X11_INCLUDE_DIR} ${AUTOMATIOTEST_XTEST_INCLUDE_DIR})
    target_link_libraries(AutomatioTestUtilities PUBLIC ${X11_LIBRARIES} ${AUTOMATIOTEST_XTEST_LIBRARY})
endif()
