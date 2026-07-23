find_package(Tesseract CONFIG QUIET)

if(TARGET Tesseract::libtesseract)
    set(AUTOMATIOTEST_TESSERACT_TARGET Tesseract::libtesseract)
else()
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(TESSERACT QUIET IMPORTED_TARGET tesseract)
    endif()

    if(TARGET PkgConfig::TESSERACT)
        set(AUTOMATIOTEST_TESSERACT_TARGET PkgConfig::TESSERACT)
    else()
        find_path(AUTOMATIOTEST_TESSERACT_INCLUDE_DIR tesseract/baseapi.h)
        find_library(AUTOMATIOTEST_TESSERACT_LIBRARY NAMES tesseract libtesseract)

        if(NOT AUTOMATIOTEST_TESSERACT_INCLUDE_DIR OR NOT AUTOMATIOTEST_TESSERACT_LIBRARY)
            message(FATAL_ERROR
                "Tesseract development files were not found. Install libtesseract-dev "
                "on Ubuntu, or install the tesseract vcpkg port on Windows.")
        endif()

        add_library(AutomatioTestTesseract UNKNOWN IMPORTED)
        set_target_properties(AutomatioTestTesseract PROPERTIES
            IMPORTED_LOCATION "${AUTOMATIOTEST_TESSERACT_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${AUTOMATIOTEST_TESSERACT_INCLUDE_DIR}"
        )
        set(AUTOMATIOTEST_TESSERACT_TARGET AutomatioTestTesseract)
    endif()
endif()

add_library(AutomatioTestOcrLib
    OcrProcesser.cpp
    OcrProcesser.hpp
)

target_include_directories(AutomatioTestOcrLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(AutomatioTestOcrLib
    PUBLIC
        AutomationTestOpencvLib
    PRIVATE
        AutomatioTestUtilities
        ${AUTOMATIOTEST_TESSERACT_TARGET}
)
