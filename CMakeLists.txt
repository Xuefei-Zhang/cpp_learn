# Refuse to configure with older CMake versions.
# We choose a version that supports the commands used in this repo cleanly.
cmake_minimum_required(VERSION 3.20)

# Give the project a stable name and declare that we are building C++ code.
project(cpp_engineering_mastery_lab LANGUAGES CXX)

# Export compile_commands.json so editor tooling and LSP servers can see the
# exact compiler flags CMake generated for each translation unit.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Pull in the small policy modules that define project-wide defaults.
# Reading these in order helps explain how the build is composed:
# options -> warnings -> sanitizers -> third-party dependencies.
include(cmake/ProjectOptions.cmake)
include(cmake/Warnings.cmake)
include(cmake/Sanitizers.cmake)
include(cmake/Dependencies.cmake)

# Turn on CTest support so Catch2-discovered tests can be registered.
enable_testing()

# Each test executable compiles exactly one test file.
# This keeps build/test boundaries simple while the project is still small.
add_executable(span_like_test tests/core/span_like.test.cpp)

add_executable(scope_guard_test tests/core/scope_guard.test.cpp)

add_executable(unique_resource_test tests/core/unique_resource.test.cpp)

add_executable(static_vector_test tests/core/static_vector.test.cpp)

# Link the shared project policy bundles plus Catch2's main-enabled target.
# PRIVATE means these link requirements apply only to the executable itself,
# which is what we want for leaf test binaries.
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

# Catch2 ships a helper CMake module that can auto-register test cases with
# CTest by scanning the compiled executable.
include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)

# We prefix discovered test names by week so `ctest -R week01` and similar
# commands map neatly onto the mastery-lab phases.
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

# Benchmarks are built as separate executables because they are not part of the
# pass/fail unit-test flow; they are measurement programs.
add_executable(static_vector_bench benchmarks/containers/static_vector_bench.cpp)

# benchmark::benchmark_main supplies the benchmark framework's `main()`.
# That means our benchmark source file only needs to define benchmark bodies.
target_link_libraries(
    static_vector_bench
    PRIVATE
        cppml_project_options
        cppml_warnings
        cppml_sanitizers
        benchmark::benchmark_main
)
