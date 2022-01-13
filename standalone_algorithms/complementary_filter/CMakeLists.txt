cmake_minimum_required(VERSION 3.7)
project(Navigation-core)

option(BUILD_TESTS    "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" OFF)

set(BOOST_ENABLE_CMAKE ON)
set(Boost_USE_STATIC_LIBS OFF)

find_package(Boost COMPONENTS system unit_test_framework REQUIRED)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    message(${Boost_INCLUDE_DIR})
    include_directories(${Boost_SOURCE_DIR})
ENDIF ()

set("target" "complementary-filter")
set("test-target" "test-filter")

if(BUILD_TESTS)
  file(GLOB ${PROJECT_NAME}_TEST_SOURCES src/*.cpp tests/*.cpp)
  add_executable("${test-target}" ${${PROJECT_NAME}_TEST_SOURCES})
  target_include_directories("${test-target}" PRIVATE "include/")
  set_property(TARGET "${test-target}" PROPERTY CXX_STANDARD 17)
  set_target_properties("${test-target}" PROPERTIES COMPILE_DEFINITIONS "BUILD_FOR_LINUX;_DEBUG;")
  set_target_properties("${test-target}" PROPERTIES COMPILE_FLAGS "-fno-strict-aliasing -funwind-tables -fPIC -pipe -m64 -march=x86-64 -fstack-protector-strong -pthread -O3 -fno-omit-frame-pointer -g2 -fvisibility-inlines-hidden -Wno-undefined-bool-conversion -Wno-tautological-undefined-compare -std=c++17 -frtti -fexceptions ")
  target_link_libraries("${test-target}"
  #"-lboost_unit_test_framework"
  # Boost::unit_test_framework
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  )
endif()

if(BUILD_EXAMPLES)
  list(APPEND "${target}__cxx_srcs"
      "examples/main.cpp"
      "src/complementary_filter.cpp"
      "src/quaternion.cpp"
      "src/vector3d.cpp")
  add_executable("${target}" ${${target}__cxx_srcs})
  target_include_directories("${target}" PRIVATE "include/")
  set_property(TARGET "${target}" PROPERTY CXX_STANDARD 17)
  set_target_properties("${target}" PROPERTIES COMPILE_DEFINITIONS "BUILD_FOR_LINUX;_DEBUG;")
  set_target_properties("${target}" PROPERTIES COMPILE_FLAGS "-fno-strict-aliasing -funwind-tables -fPIC -pipe -m64 -march=x86-64 -fstack-protector-strong -pthread -O3 -fno-omit-frame-pointer -g2 -fvisibility-inlines-hidden -Wno-undefined-bool-conversion -Wno-tautological-undefined-compare -std=c++17 -frtti -fexceptions ")
endif()
