
cmake_minimum_required(VERSION 3.14)
project(GraphLib VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra)

add_library(GraphLib INTERFACE)
target_include_directories(GraphLib INTERFACE ${CMAKE_SOURCE_DIR}/include)

# ---------------- Demo executable ----------------
# add_executable(graphlib_demo examples/demo.cpp)
# target_link_libraries(graphlib_demo PRIVATE GraphLib)

# ---------------- Tests (optional — only if GTest found) ----------------
option(BUILD_TESTS "Build unit tests (requires GoogleTest)" ON)

if(BUILD_TESTS)
    find_package(GTest QUIET)
    if(GTest_FOUND)
        enable_testing()

        set(TEST_FILES
            # tests/test_traversal.cpp
            tests/test_shortestPath.cpp
            # tests/test_spanningtree.cpp
            # tests/test_connectivity.cpp
            # tests/test_networkflow.cpp
            tests/test_graph.cpp
        )

        foreach(test_file ${TEST_FILES})
            get_filename_component(test_name ${test_file} NAME_WE)

            add_executable(${test_name} ${test_file})
            target_link_libraries(${test_name} PRIVATE GraphLib GTest::gtest GTest::gtest_main pthread)
            
            add_test(NAME ${test_name} COMMAND ${test_name})
        endforeach()

    else()
        message(WARNING "GoogleTest not found — skipping test targets. Install libgtest-dev to enable tests.")
    
    endif()
endif()
