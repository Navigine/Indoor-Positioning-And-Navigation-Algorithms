cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project(geolib VERSION 2.1.6 LANGUAGES CXX)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra -Wpedantic -Wno-overlength-strings -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_RELEASE} -g -ggdb")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fconcepts")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconcepts")
endif()

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

option(BUILD_TESTS "If enabled will compile unit tests." OFF)

include(CTest)

add_library(geolib INTERFACE)

target_link_libraries(geolib INTERFACE)

target_include_directories(geolib INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

if(BUILD_TESTS OR BUILD_NAVIGATION_CORE_TESTS)
    add_subdirectory(tests)
endif()
