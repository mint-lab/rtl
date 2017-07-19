include_directories(${CMAKE_SOURCE_DIR}/rtl)

if(WIN32)
    set(CMAKE_CXX_FLAGS "  ${CMAKE_CXX_FLAGS} -Ox -MP")
else()
    set(CMAKE_CXX_FLAGS "  ${CMAKE_CXX_FLAGS} -std=c++11 -O3 -fopenmp")
endif()

option(BUILD_EXAMPLE "Example Files" true)

if(BUILD_EXAMPLE)
    add_executable ( ExampleMean ExampleMean.cpp )
    add_executable ( ExampleLineFitting ExampleLineFitting.cpp )
    add_executable ( EvaluateLineFitting EvaluateLineFitting.cpp )
endif()
