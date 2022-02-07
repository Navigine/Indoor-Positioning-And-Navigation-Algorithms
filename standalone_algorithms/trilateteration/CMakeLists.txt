cmake_minimum_required(VERSION 3.7)
project(Trilateration)

set("test-target" "test")

file(GLOB ${PROJECT_NAME}_TEST_SOURCES src/*.cpp)
add_executable("${test-target}" ${${PROJECT_NAME}_TEST_SOURCES})
target_include_directories("${test-target}" PRIVATE "include/")

set_property(TARGET "${test-target}" PROPERTY CXX_STANDARD 17)
set_target_properties("${test-target}" PROPERTIES COMPILE_DEFINITIONS "BUILD_FOR_LINUX;_DEBUG;")
set_target_properties("${test-target}" PROPERTIES COMPILE_FLAGS "-fno-strict-aliasing -funwind-tables -fPIC -m64 -march=x86-64 -fstack-protector-strong -O3 -fno-omit-frame-pointer -g2 -fvisibility-inlines-hidden -std=c++17 -frtti -fexceptions ")
target_link_libraries("${test-target}")
