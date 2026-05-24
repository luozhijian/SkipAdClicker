file(GLOB_RECURSE AUTOMATIOTEST_OPENCVLIB_HEADERS CONFIGURE_DEPENDS "*.hpp")
file(GLOB_RECURSE AUTOMATIOTEST_OPENCVLIB_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_library(AutomationTestOpencvLib
    ${AUTOMATIOTEST_OPENCVLIB_HEADERS}
    ${AUTOMATIOTEST_OPENCVLIB_SOURCES}
)

target_include_directories(AutomationTestOpencvLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(AutomationTestOpencvLib PUBLIC AutomatioTestUtilities)

if(WIN32 AND NOT Protobuf_DIR AND EXISTS "C:/Tools/vcpkg/installed/x64-windows/share/protobuf/protobuf-config.cmake")
    set(Protobuf_DIR "C:/Tools/vcpkg/installed/x64-windows/share/protobuf" CACHE PATH "Directory containing protobuf-config.cmake")
endif()

find_package(OpenCV REQUIRED)

target_include_directories(AutomationTestOpencvLib PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(AutomationTestOpencvLib PUBLIC ${OpenCV_LIBS})
message(STATUS "AutomationTestOpencvLib using native OpenCV ${OpenCV_VERSION}")
