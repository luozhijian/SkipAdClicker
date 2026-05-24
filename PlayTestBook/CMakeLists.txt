add_library(AutomatioTestPlayTestBook
    TestBookPlayer.cpp
    TestBookPlayer.hpp
    Services/RecentActivityService.cpp
    Services/RecentActivityService.hpp
    Services/StaticFunctionService.cpp
    Services/StaticFunctionService.hpp
    Services/StaticResourceService.cpp
    Services/StaticResourceService.hpp
    Services/TestParameterHelper.cpp
    Services/TestParameterHelper.hpp
)

target_include_directories(AutomatioTestPlayTestBook PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(AutomatioTestPlayTestBook
    PUBLIC
        AutomatioTestUtilities
        AutomatioTestTestBookLib
        AutomationTestOpencvLib
        AutomationTestUserInterfaceLib
        AutomatioTestOcrLib
)
