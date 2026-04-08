add_library(cppml_sanitizers INTERFACE)

option(CPPML_ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(CPPML_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(CPPML_ENABLE_TSAN "Enable ThreadSanitizer" OFF)

if(CPPML_ENABLE_ASAN AND CPPML_ENABLE_TSAN)
    message(FATAL_ERROR "CPPML_ENABLE_ASAN and CPPML_ENABLE_TSAN cannot be enabled together")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    if(CPPML_ENABLE_ASAN)
        target_compile_options(cppml_sanitizers INTERFACE -fsanitize=address)
        target_link_options(cppml_sanitizers INTERFACE -fsanitize=address)
    endif()

    if(CPPML_ENABLE_UBSAN)
        target_compile_options(cppml_sanitizers INTERFACE -fsanitize=undefined)
        target_link_options(cppml_sanitizers INTERFACE -fsanitize=undefined)
    endif()

    if(CPPML_ENABLE_TSAN)
        target_compile_options(cppml_sanitizers INTERFACE -fsanitize=thread)
        target_link_options(cppml_sanitizers INTERFACE -fsanitize=thread)
    endif()
endif()
