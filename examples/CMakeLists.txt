
MESSAGE( STATUS "RTL DIRECTORY:  " ${RTL_OUTPUT_LIB_DIR})

include_directories(${CMAKE_SOURCE_DIR}/src)

link_directories(${RTL_OUTPUT_LIB_DIR})

set(CMAKE_CXX_FLAGS "  ${CMAKE_CXX_FLAGS} -std=c++11 -O3 -fopenmp")


option(BUILD_EXAMPLE "Example Files" true)

if(BUILD_EXAMPLE)
    add_executable ( ExampleLineFitting ExampleLineFitting.cpp )
    add_executable ( EvaluateLineFitting EvaluateLineFitting.cpp )
endif()
