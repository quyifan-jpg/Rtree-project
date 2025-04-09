cmake_minimum_required(VERSION 3.10)
project(RTree VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set compilation options
if (MSVC)
    add_compile_options(/W4)
else ()
    add_compile_options(-Wall -Wextra -pedantic)
endif ()

# Set debug/release mode
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

# Add include directories
include_directories(${CMAKE_SOURCE_DIR})

# Set source files for RTree library
set(RTREE_SOURCES
        src/RTree/impl/pojo/Point.cpp
        src/RTree/impl/Region.cpp
        src/RTree/impl/Data.cpp
        src/RTree/impl/node/LeafNode.cpp
        src/RTree/impl/node/InternalNode.cpp
        src/RTree/impl/strategy/LinearSplitStrategy.cpp
        src/RTree/impl/strategy/QuadraticSplitStrategy.cpp
        src/RTree/impl/strategy/RStarSplitStrategy.cpp
        src/RTree/impl/node/Node.h
        src/RTree/impl/pojo/Point.h
        src/RTree/impl/Region.h
        src/RTree/impl/strategy/LinearSplitStrategy.h
        src/RTree/impl/strategy/QuadraticSplitStrategy.h
        src/RTree/impl/strategy/RStarSplitStrategy.h
        src/RTree/impl/node/LeafNode.h
        src/RTree/impl/node/InternalNode.h
        src/RTree/impl/Data.h
        src/RTree/impl/common.h
        src/RTree/impl/strategy/SplitStrategy.h
        src/RTree/impl/tree/RTree.h
        src/RTree/impl/tree/RTree.cpp
        src/RTree/impl/metric/MetricManager.h
        src/generator/TestGenerator.h
)

add_executable(rtree_app src/main.cpp ${RTREE_SOURCES})

# Output configuration information
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}") 