cmake_minimum_required(VERSION 3.7)
project(Navigation-core)

include(FetchContent)

FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent.git
  GIT_TAG v3.10.3)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_Declare(
  Eigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG master
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE)
set(EIGEN_BUILD_DOC OFF)
set(BUILD_TESTING OFF)
set(EIGEN_BUILD_PKGCONFIG OFF)
set( OFF)

FetchContent_MakeAvailable(Eigen)

set(BOOST_ENABLE_CMAKE ON)
set(Boost_USE_STATIC_LIBS ON)

find_package(Boost REQUIRED)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    message(${Boost_INCLUDE_DIR})
    include_directories(${Boost_SOURCE_DIR})
ENDIF ()

#tests:navigation_test
set("target" "navigation_test")
list(APPEND "${target}__cxx_srcs"
  "tests/navigation_test.cpp"
  "tools/verification/helpers.cpp")
add_executable("${target}" ${${target}__cxx_srcs})
target_include_directories("${target}" PRIVATE
  "tests/include/"
  "tests/build/"
  "include/")
set_property(TARGET "${target}" PROPERTY CXX_STANDARD 17)
set_target_properties("${target}" PROPERTIES COMPILE_DEFINITIONS "BUILD_FOR_LINUX;_DEBUG;")
set_target_properties("${target}" PROPERTIES COMPILE_FLAGS "-fno-strict-aliasing -funwind-tables -fPIC -pipe -m64 -march=x86-64 -fstack-protector-strong -pthread -O3 -fno-omit-frame-pointer -g2 -fvisibility-inlines-hidden -Wno-undefined-bool-conversion -Wno-tautological-undefined-compare -std=c++17 -frtti -fexceptions ")
target_link_libraries("${target}" PRIVATE
  "navigation-core"
  nlohmann_json::nlohmann_json
  Eigen3::Eigen)


#navigation-core:copy_public_include_dir
set("target" "navigation-core_copy_public_include_dir")
set("${target}__other_srcs" "include")
set("${target}__output" "tests/build/obj/include")
add_custom_command(OUTPUT ${${target}__output}
  COMMAND ${CMAKE_COMMAND} -E copy_directory "include" "tests/build/obj/include"
  DEPENDS  ${${target}__other_srcs}
  WORKING_DIRECTORY "tests/build/"
  COMMENT "Copy ${target}"
  VERBATIM)
add_custom_target("${target}" SOURCES ${${target}__other_srcs} DEPENDS ${${target}__output})


#navigation-core:navigation-core
set("target" "navigation-core")
list(APPEND "${target}__cxx_srcs"
      "src/geometry.cpp"
      "src/triangulation.cpp"
      "src/knn/knn_utils.cpp"
      "src/likelihood/likelihood.cpp"
      "src/likelihood/likelihood_radiomap.cpp"
      "src/measurements/measurement_preprocessor.cpp"
      "src/position_estimator/position_estimator_knn.cpp"
      "src/position_estimator/position_estimator_zone.cpp"
      "src/position_estimator/position_estimator_outdoor.cpp"
      "src/position_postprocessor/navigation_time_smoother.cpp"
      "src/position_postprocessor/polynomial_fit.cpp"
      "src/position_postprocessor/position_postprocessor.cpp"
      "src/position_postprocessor/position_smoother_ab.cpp"
      "src/position_postprocessor/position_smoother_lstsq.cpp"
      "src/sensor_fusion/complementary_filter.cpp"
      "src/sensor_fusion/quaternion.cpp"
      "src/sensor_fusion/pedometer.cpp"
      "src/sensor_fusion/sensor_fusion.cpp"
      "src/level_estimator/barometer.cpp"
      "src/level_estimator/level_estimator.cpp"
      "src/level_estimator/level_estimator_radiomap.cpp"
      "src/level_estimator/level_estimator_transmitters.cpp"
      "src/level_estimator/level_history.cpp"
      "src/navigation_client_impl.cpp"
      "src/barriers_geometry_builder.cpp"
      "src/level.cpp"
      "src/point.cpp"
      "src/level_collector.cpp"
      "src/level_geometry.cpp"
      "src/radiomap.cpp"
      "src/vector3d.cpp"
      "tools/verification/helpers.cpp"
      "src/trilateration.cpp")
add_library("${target}" STATIC ${${target}__cxx_srcs})
# add_dependencies("${target}" boost_geometry)
target_include_directories("${target}" PRIVATE
  "include/"
  "tests/build/")
set_property(TARGET "${target}" PROPERTY CXX_STANDARD 17)
set_target_properties("${target}" PROPERTIES COMPILE_DEFINITIONS "BUILD_FOR_LINUX;_DEBUG;")
set_target_properties("${target}" PROPERTIES COMPILE_FLAGS "-fno-strict-aliasing -funwind-tables -fPIC -pipe -m64 -march=x86-64 -fstack-protector-strong -pthread -O3 -fno-omit-frame-pointer -g2 -fvisibility-inlines-hidden -Wno-undefined-bool-conversion -Wno-tautological-undefined-compare -std=c++17 -frtti -fexceptions ")
target_link_libraries("${target}" PRIVATE nlohmann_json::nlohmann_json
					  Eigen3::Eigen)
