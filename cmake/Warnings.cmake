# Like the other support targets, this is an INTERFACE library used only
# to propagate compile options. It exists so warning policy is centralized.
add_library(cppml_warnings INTERFACE)

# For GCC and Clang we use the common "strict but still practical" set.
# -Wall: enable the common warning group
# -Wextra: enable more suspicious-but-not-always-buggy cases
# -Wpedantic: ask the compiler to warn about non-standard extensions
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(cppml_warnings INTERFACE -Wall -Wextra -Wpedantic)
# MSVC uses a different warning flag vocabulary, so we choose the nearest
# equivalent strict mode here.
elseif(MSVC)
    target_compile_options(cppml_warnings INTERFACE /W4 /permissive-)
endif()
