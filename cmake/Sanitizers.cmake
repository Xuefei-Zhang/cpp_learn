# This INTERFACE target lets us turn sanitizers on and off without changing
# every individual executable definition.
add_library(cppml_sanitizers INTERFACE)

# These options expose the sanitizer policy at configure time, which means
# we can choose different build directories for normal, ASan, UBSan, or TSan
# runs without editing CMake files.
option(CPPML_ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(CPPML_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(CPPML_ENABLE_TSAN "Enable ThreadSanitizer" OFF)

# AddressSanitizer and ThreadSanitizer are not meant to be enabled together
# in one build. Failing fast here makes the configuration error obvious.
if(CPPML_ENABLE_ASAN AND CPPML_ENABLE_TSAN)
    message(FATAL_ERROR "CPPML_ENABLE_ASAN and CPPML_ENABLE_TSAN cannot be enabled together")
endif()

# Sanitizer flags below are compiler-specific, so we only apply them on the
# compiler families that use the Clang/GCC spellings.
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    # AddressSanitizer is mainly for memory safety issues such as use-after-free,
    # buffer overrun, and some lifetime-related heap bugs.
    if(CPPML_ENABLE_ASAN)
        target_compile_options(cppml_sanitizers INTERFACE -fsanitize=address)
        target_link_options(cppml_sanitizers INTERFACE -fsanitize=address)
    endif()

    # UndefinedBehaviorSanitizer helps catch operations that compile but are
    # still invalid according to the C++ abstract machine.
    if(CPPML_ENABLE_UBSAN)
        target_compile_options(cppml_sanitizers INTERFACE -fsanitize=undefined)
        target_link_options(cppml_sanitizers INTERFACE -fsanitize=undefined)
    endif()

    # ThreadSanitizer is specifically for data races and synchronization bugs.
    if(CPPML_ENABLE_TSAN)
        target_compile_options(cppml_sanitizers INTERFACE -fsanitize=thread)
        target_link_options(cppml_sanitizers INTERFACE -fsanitize=thread)
    endif()
endif()
