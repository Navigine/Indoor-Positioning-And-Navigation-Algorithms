enable_testing()

function(add_test_list TEST_LIST_FILE MODULE_NAME EXECUTABLE)
    file(READ ${TEST_LIST_FILE} SOURCE_FILE_COMPONENTS)
    string(REGEX MATCHALL "ADD_TEST\\([A-Za-z0-9_]+\\)" FOUND_TESTS ${SOURCE_FILE_COMPONENTS})

    foreach(TEST_NAME ${FOUND_TESTS})
        string(REGEX REPLACE "ADD_TEST\\(([A-Za-z0-9_]+)\\)" "\\1" TEST_NAME ${TEST_NAME})
        set(test_id "ut.${test_name}")

        add_test(NAME "ut.${MODULE_NAME}.${TEST_NAME}" COMMAND ${EXECUTABLE} ${TEST_NAME})
        set_tests_properties("ut.${MODULE_NAME}.${TEST_NAME}" PROPERTIES LABELS ${MODULE_NAME})
    endforeach()
endfunction()

set(MODULE_NAME "${PROJECT_NAME}")
set(EXECUTABLE_NAME "ut.${MODULE_NAME}")

add_executable("${EXECUTABLE_NAME}"
    binary_operations.cpp
    buffer.cpp
    contains.cpp
    convex_hull.cpp
    grid_point.cpp
    int128.cpp
    point_index.cpp
    segment_intersection.cpp
    spatial_search_tree.cpp
    split.cpp
    valid.cpp
    wkt.cpp
    main.cpp
)

target_link_libraries("${EXECUTABLE_NAME}"
    geolib
)

add_test_list(
    "${CMAKE_CURRENT_SOURCE_DIR}/add_tests.h"
    "${MODULE_NAME}"
    "${EXECUTABLE_NAME}"
)
