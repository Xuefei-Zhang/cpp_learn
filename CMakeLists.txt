cmake_minimum_required(VERSION 3.20)

project(cpp_engineering_mastery_lab LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/ProjectOptions.cmake)
include(cmake/Warnings.cmake)
include(cmake/Sanitizers.cmake)
include(cmake/Dependencies.cmake)

enable_testing()

add_executable(span_like_test tests/core/span_like.test.cpp)

add_executable(scope_guard_test tests/core/scope_guard.test.cpp)

add_executable(unique_resource_test tests/core/unique_resource.test.cpp)

add_executable(static_vector_test tests/core/static_vector.test.cpp)

target_link_libraries(
    span_like_test
    PRIVATE
        cppml_project_options
        cppml_warnings
        cppml_sanitizers
        Catch2::Catch2WithMain
)

target_link_libraries(
    scope_guard_test
    PRIVATE
        cppml_project_options
        cppml_warnings
        cppml_sanitizers
        Catch2::Catch2WithMain
)

target_link_libraries(
    unique_resource_test
    PRIVATE
        cppml_project_options
        cppml_warnings
        cppml_sanitizers
        Catch2::Catch2WithMain
)

target_link_libraries(
    static_vector_test
    PRIVATE
        cppml_project_options
        cppml_warnings
        cppml_sanitizers
        Catch2::Catch2WithMain
)

include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)

catch_discover_tests(
    span_like_test
    TEST_PREFIX week01:
)

catch_discover_tests(
    scope_guard_test
    TEST_PREFIX week02:
)

catch_discover_tests(
    unique_resource_test
    TEST_PREFIX week02:
)

catch_discover_tests(
    static_vector_test
    TEST_PREFIX week03:
)

add_executable(static_vector_bench benchmarks/containers/static_vector_bench.cpp)

target_link_libraries(
    static_vector_bench
    PRIVATE
        cppml_project_options
        cppml_warnings
        cppml_sanitizers
        benchmark::benchmark_main
)
