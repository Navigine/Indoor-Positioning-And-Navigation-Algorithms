cmake_minimum_required(VERSION 3.7)
project(Position-estimation)

option(BUILD_EXAMPLES "Build examples" ON)

set("target" "position-estimation")

if(BUILD_EXAMPLES)
  list(APPEND "${target}__cxx_srcs"
      "examples/main.cpp"
      "src/measurement_preprocessor.cpp"
      "src/nearest_transmitter_estimator.cpp"
      "src/position_smoother.cpp")
  add_executable("${target}" ${${target}__cxx_srcs})
  target_include_directories("${target}" PRIVATE "include/")
  set_property(TARGET "${target}" PROPERTY CXX_STANDARD 17)
  set_target_properties("${target}" PROPERTIES COMPILE_DEFINITIONS "BUILD_FOR_LINUX;_DEBUG;")
  set_target_properties("${target}" PROPERTIES COMPILE_FLAGS "-fno-strict-aliasing -funwind-tables -fPIC -pipe -m64 -march=x86-64 -fstack-protector-strong -pthread -O3 -fno-omit-frame-pointer -g2 -fvisibility-inlines-hidden -Wno-undefined-bool-conversion -Wno-tautological-undefined-compare -std=c++17 -frtti -fexceptions ")
endif()
