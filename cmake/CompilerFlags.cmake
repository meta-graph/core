# CompilerFlags.cmake - Nuclear-level C23 compiler flags

# The "Nuclear Option" - Maximum Strictness

# Common warning flags for GCC/Clang
set(METAGRAPH_WARNING_FLAGS
    -Wall
    -Wextra
    -Wpedantic
    -Wcast-align=strict
    -Wcast-qual
    -Wconversion
    -Wdouble-promotion
    -Wfloat-equal
    -Wformat=2
    -Wformat-overflow=2
    -Wformat-signedness
    -Wformat-truncation=2
    -Wimplicit-fallthrough=5
    -Wmissing-declarations
    -Wmissing-prototypes
    -Wnull-dereference
    -Wpacked
    -Wpointer-arith
    -Wredundant-decls
    -Wshadow
    -Wstack-protector
    -Wstrict-prototypes
    -Wswitch-default
    -Wswitch-enum
    -Wundef
    -Wunused-macros
    -Wvla
    -Wwrite-strings
    -Wno-unused-parameter  # Common in function pointer interfaces
    -Wno-gnu-zero-variadic-macro-arguments  # Allow empty __VA_ARGS__
)

# Security hardening flags (platform-specific)
set(METAGRAPH_SECURITY_FLAGS
    -U_FORTIFY_SOURCE
    -D_FORTIFY_SOURCE=3
    -fstack-protector-all
    -fPIE
)

set(METAGRAPH_SECURITY_LINK_FLAGS)

# Platform-specific security flags
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND METAGRAPH_SECURITY_FLAGS
        -fstack-clash-protection
    )
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|i[3-6]86")
        list(APPEND METAGRAPH_SECURITY_FLAGS -fcf-protection=full)
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # macOS-specific flags
    # Note: -fstack-clash-protection and -fcf-protection not supported on Apple Clang
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Windows-specific flags handled in MSVC section
endif()

# Compiler-specific flags
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    list(APPEND METAGRAPH_WARNING_FLAGS
        -Wduplicated-branches
        -Wduplicated-cond
        -Wlogical-op
        -Walloca
        -Wanalyzer-too-complex
        -Warith-conversion
        -Wbad-function-cast
        -Wstrict-overflow=5
        -Wtrampolines
        -Wvector-operation-performance
    )
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
    list(REMOVE_ITEM METAGRAPH_WARNING_FLAGS
        -Wcast-align=strict
        -Wformat-overflow=2
        -Wformat-signedness
        -Wformat-truncation=2
        -Wimplicit-fallthrough=5
    )
    list(APPEND METAGRAPH_WARNING_FLAGS
        -Wcast-align
        -Wimplicit-fallthrough
    )
    list(APPEND METAGRAPH_WARNING_FLAGS
        -Wthread-safety
        -Wthread-safety-beta
    )

    # Clang-specific hardening
    if(METAGRAPH_SANITIZERS)
        # CFI requires LTO
        if(CMAKE_INTERPROCEDURAL_OPTIMIZATION)
            list(APPEND METAGRAPH_SECURITY_FLAGS
                -fsanitize=cfi
            )
        endif()
    else()
        # safe-stack is not supported on all platforms and conflicts with ASAN
        if(NOT CMAKE_SYSTEM_NAME STREQUAL "Darwin")
            list(APPEND METAGRAPH_SECURITY_FLAGS
                -fsanitize=safe-stack
            )
            list(APPEND METAGRAPH_SECURITY_LINK_FLAGS
                -fsanitize=safe-stack
            )
        endif()
    endif()
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    set(METAGRAPH_WARNING_FLAGS
        /W4
        /permissive-
        /analyze
        /analyze:external-
        /external:anglebrackets
        /external:W0
    )

    set(METAGRAPH_SECURITY_FLAGS
        /guard:cf
        /Qspectre
        /sdl
    )
endif()

# Apply warning flags to all targets
add_compile_options(${METAGRAPH_WARNING_FLAGS})
add_compile_options(${METAGRAPH_SECURITY_FLAGS})

if(METAGRAPH_SECURITY_LINK_FLAGS)
    add_link_options(${METAGRAPH_SECURITY_LINK_FLAGS})
endif()

# Enable PIE for all builds (not just release)
if(NOT CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        add_link_options(-pie)
    endif()
    # macOS automatically handles PIE with CMAKE_POSITION_INDEPENDENT_CODE
endif()

# Warnings as errors in development mode
if(METAGRAPH_DEV OR METAGRAPH_WERROR)
    if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(/WX)
    else()
        add_compile_options(-Werror)
    endif()
endif()

# Release optimization flags
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(/O2)
    else()
        add_compile_options(-O3)

        # Architecture-specific optimizations
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
            add_compile_options(-march=x86-64-v3)  # Reproducible baseline (AVX2+FMA)
        elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
            # ARM64 optimizations
            if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
                add_compile_options(-mcpu=apple-m1)  # Apple Silicon
            else()
                add_compile_options(-march=armv8.2-a)  # Generic ARM64
            endif()
        endif()

        # LTO and linker plugin
        add_compile_options(-flto=auto)
        if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
            add_compile_options(-fuse-linker-plugin)
        endif()

        # Linker flags for release (platform-specific)
        if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
            add_link_options(
                -Wl,-z,relro,-z,now
                -Wl,-z,noexecstack
            )
        endif()
    endif()
endif()

# Character encoding (critical for Windows)
if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    add_compile_options(
        -finput-charset=UTF-8
        -fexec-charset=UTF-8
    )
endif()
