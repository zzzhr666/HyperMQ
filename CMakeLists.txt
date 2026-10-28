cmake_minimum_required(VERSION 3.20)

project(
        HyperMQ
        VERSION 0.1.0
        LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(HYPERMQ_BUILD_EXAMPLE "Build HyperMQ examples" OFF)
option(HYPERMQ_BUILD_TESTS "Build HyperMQ tests" ON)

add_subdirectory(core)

if(HYPERMQ_BUILD_EXAMPLE)
    add_subdirectory(example)
endif()

include(CTest)

if(BUILD_TESTING AND HYPERMQ_BUILD_TESTS)
    add_subdirectory(test)
endif()
