# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020 Antonio Niño Díaz

# This makes GCC not export the symbols in a shared library by default. This is
# the behaviour in MSVC, so this makes it behave the same way in both compilers.
set(CMAKE_C_VISIBILITY_PRESET hidden)

add_library(libugba SHARED)

# Compiler-specific options
# -------------------------

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(libugba PRIVATE
        # Force all integers to be 2's complement to prevent the compiler from
        # doing optimizations because of undefined behaviour.
        -fwrapv

        # Force usage of extern for external variables
        -fno-common

        # Enable most common warnings
        -Wall -Wextra

        # Disable this warning, which is enabled by default
        -Wformat-truncation=0
    )
    if(CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 9.3)
        target_compile_options(libugba PRIVATE
            # Enable a bunch of warnings that aren't enabled with Wall or Wextra
            -Wformat-overflow=2 -Wformat=2 -Wno-format-nonliteral
            -Wundef -Wunused -Wuninitialized -Wunknown-pragmas -Wshadow
            -Wlogical-op -Wduplicated-cond -Wswitch-enum -Wfloat-equal
            -Wcast-align -Walloc-zero -Winline
            -Wstrict-overflow=5 -Wstringop-overflow=4
            $<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>
            $<$<COMPILE_LANGUAGE:C>:-Wold-style-definition>

            # Enable Wpedantic but disable warning about having strings that are
            # too long
            -Wpedantic -Wno-overlength-strings

            # Make sure we don't use too much stack. Windows doesn't like it
            # when the stack usage is too high, even when Linux doesn't complain
            # about it.
            -Wstack-usage=4096

            # TODO: Enable the following warnings?
            #-Wformat-truncation=1 -Wcast-qual -Wconversion
        )

        # Build option to enable Undefined Behaviour Sanitizer (UBSan)
        # --------------------------------------------------------
        #
        # This should only be enabled in debug builds. It makes the code far
        # slower, so it should only be used during development.
        option(ENABLE_UBSAN "Compile with UBSan support (GCC)" OFF)

        if(ENABLE_UBSAN)
            target_compile_options(libugba PRIVATE -fsanitize=undefined)
            target_link_options(libugba PRIVATE -fsanitize=undefined)
        endif()
    endif()
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    target_compile_definitions(libugba PRIVATE
        # Silence warnings
        -D_USE_MATH_DEFINES
        -D_CRT_SECURE_NO_WARNINGS
    )
    target_compile_options(libugba PRIVATE
        # Enable parallel compilation
        /MP
    )
endif()

# Add source code files
# ---------------------

# Macro that searches all the source files in the specified directory in 'dir'
# and saves them in 'var'
macro(search_source_files dir var)
    file(GLOB ${var} CONFIGURE_DEPENDS ${dir}/*.c ${dir}/*.h)
endmacro()

search_source_files(source FILES_SOURCE)
search_source_files(source/sdl2 FILES_SOURCE_SDL2)
search_source_files(source/sdl2/core FILES_SOURCE_SDL2_CORE)
search_source_files(source/sdl2/gui FILES_SOURCE_SDL2_GUI)

target_sources(libugba PRIVATE
    ${FILES_SOURCE}
    ${FILES_SOURCE_SDL2}
    ${FILES_SOURCE_SDL2_CORE}
    ${FILES_SOURCE_SDL2_GUI}
)

target_include_directories(libugba PUBLIC
    include
)

# Link with libraries and check build options
# -------------------------------------------

# libpng, SLD2 and Lua are required

if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(libugba PRIVATE
        png
        SDL2::SDL2 SDL2::SDL2main
    )
else()
    target_include_directories(libugba PRIVATE
        ${PNG_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}
    )
    target_link_libraries(libugba PRIVATE
        ${PNG_LIBRARIES}
        ${SDL2_LIBRARIES}
    )
endif()

target_link_libraries(libugba PRIVATE ${LUA_LIBRARIES})
target_include_directories(libugba PRIVATE ${LUA_INCLUDE_DIR})

# OpenGL is optional. It can be used as library to output graphics.

if(ENABLE_OPENGL)
    target_compile_definitions(libugba PRIVATE -DENABLE_OPENGL)
    target_include_directories(libugba PRIVATE ${OPENGL_INCLUDE_DIRS})
    target_link_libraries(libugba PRIVATE ${OPENGL_LIBRARIES})
endif()

# Add tools needed by libugba

add_subdirectory(tools)

# Add utilities to work with other tools like GRIT

include(cmake/bin2c.cmake)
include(cmake/grit.cmake)

# Add graphics to library, like the default font

add_grit_files(graphics libugba)