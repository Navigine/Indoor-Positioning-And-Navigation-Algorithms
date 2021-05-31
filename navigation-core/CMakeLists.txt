cmake_minimum_required(VERSION 3.7)
project(Navigation-core)


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


set_target_properties("${target}" PROPERTIES COMPILE_DEFINITIONS "BUILD_FOR_LINUX;_DEBUG;")
set_target_properties("${target}" PROPERTIES COMPILE_FLAGS "-fno-strict-aliasing -funwind-tables -fPIC -pipe -m64 -march=x86-64 -fstack-protector-strong -pthread -O3 -fno-omit-frame-pointer -g2 -fvisibility-inlines-hidden -Wno-undefined-bool-conversion -Wno-tautological-undefined-compare -std=c++17 -frtti -fexceptions ")
set_target_properties("${target}" PROPERTIES LINK_FLAGS "-Wl,--fatal-warnings -fPIC -Wl,-z,noexecstack -Wl,-z,now -Wl,-z,relro -Wl,-z,defs -m64 -pthread -Wl,-rpath=$ORIGIN -Wl,-rpath-link=. -Wl,--disable-new-dtags ")
find_library("library__dl" "dl")
find_library("library__rt" "rt")
target_link_libraries("${target}"
  "navigation-core"
  "${library__dl}"
  "${library__rt}")


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
  "src/measurements/measurement_preprocessor.cpp"
  "src/position_estimator/position_estimator_zone.cpp"
  "src/level_estimator/barometer.cpp"
  "src/level_estimator/level_estimator.cpp"
  "src/level_estimator/level_estimator_transmitters.cpp"
  "src/level_estimator/level_history.cpp"
  "src/navigation_client_impl.cpp"
  "src/barriers_geometry_builder.cpp"
  "src/level.cpp"
  "src/point.cpp"
  "src/level_collector.cpp"
  "src/level_geometry.cpp"
  "src/vector3d.cpp"
  "tools/verification/helpers.cpp")
add_library("${target}" STATIC ${${target}__cxx_srcs})
target_include_directories("${target}" PRIVATE
  "include/"
  "tests/build/")
set_target_properties("${target}" PROPERTIES COMPILE_DEFINITIONS "BUILD_FOR_LINUX;_DEBUG;")
set_target_properties("${target}" PROPERTIES COMPILE_FLAGS "-fno-strict-aliasing -funwind-tables -fPIC -pipe -m64 -march=x86-64 -fstack-protector-strong -pthread -O3 -fno-omit-frame-pointer -g2 -fvisibility-inlines-hidden -Wno-undefined-bool-conversion -Wno-tautological-undefined-compare -std=c++17 -frtti -fexceptions ")
set_target_properties("${target}" PROPERTIES LINK_FLAGS "-Wl,--fatal-warnings -fPIC -Wl,-z,noexecstack -Wl,-z,now -Wl,-z,relro -Wl,-z,defs -m64 -pthread ")
