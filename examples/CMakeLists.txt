
MESSAGE( STATUS "RTL DIRECTORY:  " ${RTL_OUTPUT_LIB_DIR})

include_directories(${CMAKE_SOURCE_DIR}/src)

link_directories(${RTL_OUTPUT_LIB_DIR})

if(WIN32)
    set(CMAKE_CXX_FLAGS "  ${CMAKE_CXX_FLAGS} -Ox -MP")
else()
    set(CMAKE_CXX_FLAGS "  ${CMAKE_CXX_FLAGS} -std=c++11 -O3 -fopenmp")
endif()



option(BUILD_EXAMPLE "Example Files" true)

if(BUILD_EXAMPLE)
    add_executable ( ExampleLineFitting ExampleLineFitting.cpp )
    add_executable ( EvaluateLineFitting EvaluateLineFitting.cpp )
endif()
