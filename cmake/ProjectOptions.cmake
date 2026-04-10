# This INTERFACE library does not build a real .a/.so file.
# Instead, it is a reusable bundle of project-wide build settings.
# Any target that links against it automatically inherits those settings.
add_library(cppml_project_options INTERFACE)

# Require C++20 for every target that links against cppml_project_options.
# This is more maintainable than repeating `cxx_std_20` on every executable.
target_compile_features(cppml_project_options INTERFACE cxx_std_20)

# Make the repository root an include directory for dependents.
# That allows includes such as "core/utility/span_like.hpp" to work
# from tests and benchmarks without extra per-target include wiring.
target_include_directories(cppml_project_options INTERFACE ${PROJECT_SOURCE_DIR})
