file(GLOB_RECURSE AUTOMATIOTEST_OCRLIB_HEADERS CONFIGURE_DEPENDS "*.hpp")
file(GLOB_RECURSE AUTOMATIOTEST_OCRLIB_SOURCES CONFIGURE_DEPENDS "*.cpp")

option(AUTOMATIOTEST_ENABLE_TESSERACT "Enable native Tesseract OCR support when headers and libraries are available." ON)

set(AUTOMATIOTEST_TESSERACT_INCLUDE_HINTS
    $ENV{TESSERACT_ROOT}/include
    $ENV{TESSERACT_DIR}/include
    "C:/Program Files/Tesseract-OCR/include"
    "C:/vcpkg/installed/x64-windows/include"
)

set(AUTOMATIOTEST_TESSERACT_LIBRARY_HINTS
    $ENV{TESSERACT_ROOT}/lib
    $ENV{TESSERACT_DIR}/lib
    "C:/Program Files/Tesseract-OCR/lib"
    "C:/vcpkg/installed/x64-windows/lib"
)

if(AUTOMATIOTEST_ENABLE_TESSERACT)
    find_package(Tesseract CONFIG QUIET)
    if(NOT TARGET Tesseract::libtesseract)
        find_path(AUTOMATIOTEST_TESSERACT_INCLUDE_DIR
            NAMES tesseract/baseapi.h
            HINTS ${AUTOMATIOTEST_TESSERACT_INCLUDE_HINTS}
        )

        find_library(AUTOMATIOTEST_TESSERACT_LIBRARY
            NAMES tesseract libtesseract-5 tesseract55 tesseract54 tesseract53
            HINTS ${AUTOMATIOTEST_TESSERACT_LIBRARY_HINTS}
        )

        find_library(AUTOMATIOTEST_LEPTONICA_LIBRARY
            NAMES leptonica lept
            HINTS ${AUTOMATIOTEST_TESSERACT_LIBRARY_HINTS}
        )
    endif()
endif()

add_library(AutomatioTestOcrLib
    ${AUTOMATIOTEST_OCRLIB_HEADERS}
    ${AUTOMATIOTEST_OCRLIB_SOURCES}
)

target_include_directories(AutomatioTestOcrLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(AutomatioTestOcrLib PUBLIC AutomatioTestUtilities AutomationTestOpencvLib)

set(AUTOMATIOTEST_HAS_TESSERACT 0)

if(AUTOMATIOTEST_ENABLE_TESSERACT AND TARGET Tesseract::libtesseract)
    target_link_libraries(AutomatioTestOcrLib PRIVATE Tesseract::libtesseract)
    set(AUTOMATIOTEST_HAS_TESSERACT 1)
elseif(AUTOMATIOTEST_ENABLE_TESSERACT AND AUTOMATIOTEST_TESSERACT_INCLUDE_DIR AND AUTOMATIOTEST_TESSERACT_LIBRARY AND AUTOMATIOTEST_LEPTONICA_LIBRARY)
    target_include_directories(AutomatioTestOcrLib PRIVATE ${AUTOMATIOTEST_TESSERACT_INCLUDE_DIR})
    target_link_libraries(AutomatioTestOcrLib PRIVATE ${AUTOMATIOTEST_TESSERACT_LIBRARY} ${AUTOMATIOTEST_LEPTONICA_LIBRARY})
    set(AUTOMATIOTEST_HAS_TESSERACT 1)
endif()

target_compile_definitions(AutomatioTestOcrLib PRIVATE AUTOMATIOTEST_HAS_TESSERACT=${AUTOMATIOTEST_HAS_TESSERACT})
