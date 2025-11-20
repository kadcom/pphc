function(detect_compiler)
    if(EMSCRIPTEN)
        message(STATUS "Detected Emscripten compiler")
        set(EMSCRIPTEN TRUE PARENT_SCOPE)

        # Emscripten-specific flags
        add_compile_options(-Wall -Wextra)

    elseif(CMAKE_C_COMPILER_ID MATCHES "Watcom")
        message(STATUS "Detected OpenWatcom compiler")
        set(WATCOM TRUE PARENT_SCOPE)

        # OpenWatcom-specific flags
        add_compile_options(-zq)  # Quiet mode
        add_compile_options(-ox)  # Optimize for speed
        add_compile_options(-w4)  # Warning level 4
        add_compile_options(-we)  # Warnings as errors
        add_compile_options(-za99)  # Enable C99 support (compound literals, etc.)

    elseif(MSVC)
        message(STATUS "Detected MSVC compiler")

        # MSVC-specific flags
        add_compile_options(/W4)  # Warning level 4

        # Disable specific warnings
        add_compile_options(/wd4996)  # 'function': was declared deprecated
        add_compile_options(/wd4100)  # Unreferenced formal parameter

    elseif(CMAKE_C_COMPILER_ID MATCHES "GNU")
        message(STATUS "Detected GCC compiler")

        # GCC-specific flags
        add_compile_options(-Wall -Wextra -Wpedantic -pedantic)

        if(MINGW)
            message(STATUS "Building with MinGW")
            set(MINGW TRUE PARENT_SCOPE)
        endif()

    elseif(CMAKE_C_COMPILER_ID MATCHES "Clang" OR CMAKE_C_COMPILER_ID MATCHES "AppleClang")
        message(STATUS "Detected Clang compiler")

        # Clang-specific flags
        add_compile_options(-Wall -Wextra -Wpedantic -pedantic)
    endif()
endfunction()
