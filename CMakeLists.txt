#####################################################################
# License
#####################################################################
# This file is part of Zagreus.
#
# Zagreus is a UCI chess engine
# Copyright (C) 2023  Danny Jelsma
#
# Zagreus is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Zagreus is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with Zagreus.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 23)
project(Zagreus)
set(ZAGREUS_VERSION_MAJOR "6")
set(ZAGREUS_VERSION_MINOR "0")

# Default values
set(BUILD_FLAGS "")

# Define the different profiles and their corresponding flags
set(DEBUG_FLAGS "-static-libgcc -static-libstdc++ -fno-omit-frame-pointer -pthread -gdwarf-4 -g -Wno-unused-command-line-argument -std=c++23")
set(PROFILING_FLAGS "-static-libgcc -static-libstdc++ -pthread -g -Wno-unused-command-line-argument -std=c++23")
set(RELEASE_FLAGS "-static-libgcc -static-libstdc++ -pthread -Wno-unused-command-line-argument -std=c++23")

if (WIN32)
    # Add the --pdb flag for Windows
    set(DEBUG_FLAGS "${DEBUG_FLAGS} -gcodeview -Wl,--pdb=Zagreus.pdb")
    set(PROFILING_FLAGS "${PROFILING_FLAGS} -gcodeview -Wl,--pdb=Zagreus.pdb")
endif ()

# Define toggleable flags
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    option(ENABLE_OPTIMIZATION "Enable optimization flags (-O3)" OFF)
    option(ENABLE_OPTIMIZATION_FAST_MATH "Enable fast math optimization flags (-Ofast)" OFF)
    option(ENABLE_LTO "Enable Link Time Optimization (LTO)" OFF)
    option(ENABLE_WARNINGS "Enable all compiler warnings" ON)
    option(ENABLE_SANITIZER "Enable undefined behavior sanitizer flags (-fsanitize=undefined -fsanitize-trap=all)" OFF)
    option(ENABLE_MARCH "Enable -march flag" ON)
    set(MARCH_VALUE "native" CACHE STRING "Value for -march flag")
    option(ENABLE_MTUNE "Enable -mtune flag" OFF)
    set(MTUNE_VALUE "native" CACHE STRING "Value for -mtune flag")
    option(APPEND_VERSION "Append version to filename" ON)
    option(APPEND_VERSION_USE_GIT "Append version or branch to filename using git, when off it always uses just the version" ON)
    option(ENABLE_CLANG_TIDY "Enable the use of clang-tidy (slows down compiling a lot)" OFF)
    option(ENABLE_TESTS "Enable the compilation and execution of tests" ON)
    option(ENABLE_IWYU "Enable the use of Include What You Use (IWYU)" ON)
    option(ENABLE_TUNER "Enable compilation of the tuner" OFF)
else ()
    option(ENABLE_OPTIMIZATION "Enable optimization flags (-O3)" ON)
    option(ENABLE_OPTIMIZATION_FAST_MATH "Enable fast math optimization flags (-Ofast)" ON)
    option(ENABLE_LTO "Enable Link Time Optimization (LTO)" ON)
    option(ENABLE_WARNINGS "Enable all compiler warnings" ON)
    option(ENABLE_SANITIZER "Enable undefined behavior sanitizer flags" OFF)
    option(ENABLE_MARCH "Enable -march flag" ON)
    set(MARCH_VALUE "x86-64-v3" CACHE STRING "Value for -march flag")
    option(ENABLE_MTUNE "Enable -mtune flag" ON)
    set(MTUNE_VALUE "native" CACHE STRING "Value for -mtune flag")
    option(APPEND_VERSION "Append version to filename" ON)
    option(APPEND_VERSION_USE_GIT "Append version or branch to filename using git, when off it always uses just the version" ON)
    option(ENABLE_CLANG_TIDY "Enable the use of clang-tidy (slows down compiling a lot)" OFF)
    option(ENABLE_TESTS "Enable the compilation and execution of tests" OFF)
    option(ENABLE_IWYU "Enable the use of Include What You Use (IWYU)" OFF)
    option(ENABLE_TUNER "Enable compilation of the tuner" OFF)
endif ()

if (ENABLE_TESTS)
    Include(FetchContent)

    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.5.4
    )

    FetchContent_MakeAvailable(Catch2)
endif ()

if (ENABLE_CLANG_TIDY)
    # if clang-tidy is available, enable it
    find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
    if (CLANG_TIDY_EXE)
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE};--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy")
    endif ()
endif ()

message("ENABLE_OPTIMIZATION: ${ENABLE_OPTIMIZATION}")
message("ENABLE_OPTIMIZATION_FAST_MATH: ${ENABLE_OPTIMIZATION_FAST_MATH}")
message("ENABLE_LTO: ${ENABLE_LTO}")
message("ENABLE_WARNINGS: ${ENABLE_WARNINGS}")
message("ENABLE_SANITIZER: ${ENABLE_SANITIZER}")
message("ENABLE_MARCH: ${ENABLE_MARCH}")
message("MARCH_VALUE: ${MARCH_VALUE}")
message("ENABLE_MTUNE: ${ENABLE_MTUNE}")
message("MTUNE_VALUE: ${MTUNE_VALUE}")
message("APPEND_VERSION: ${APPEND_VERSION}")
message("APPEND_VERSION_USE_GIT: ${APPEND_VERSION_USE_GIT}")
message("ENABLE_CLANG_TIDY: ${ENABLE_CLANG_TIDY}")
message("ENABLE_TESTS: ${ENABLE_TESTS}")
message("ENABLE_IWYU: ${ENABLE_IWYU}")
message("ENABLE_TUNER: ${ENABLE_TUNER}")

if (APPEND_VERSION)
    execute_process(COMMAND git rev-parse --abbrev-ref HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_BRANCH
            OUTPUT_STRIP_TRAILING_WHITESPACE)

    if ((NOT ${GIT_BRANCH} STREQUAL "master") AND (NOT ${GIT_BRANCH} STREQUAL "main") AND APPEND_VERSION_USE_GIT)
        # Get the short commit hash
        execute_process(COMMAND git rev-parse --short HEAD
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_COMMIT_HASH
                OUTPUT_STRIP_TRAILING_WHITESPACE)

        # Append the branch and commit hash to the executable suffix
        set(CMAKE_EXECUTABLE_SUFFIX "-dev-${GIT_BRANCH}-${GIT_COMMIT_HASH}${CMAKE_EXECUTABLE_SUFFIX}")
        set(ZAGREUS_VERSION_MAJOR "dev")
        set(ZAGREUS_VERSION_MINOR "${GIT_BRANCH}-${GIT_COMMIT_HASH}")
    else ()
        if (ENABLE_MARCH)
            set(CMAKE_EXECUTABLE_SUFFIX "-${MARCH_VALUE}${CMAKE_EXECUTABLE_SUFFIX}")
        endif ()

        set(CMAKE_EXECUTABLE_SUFFIX "-${CMAKE_SYSTEM_NAME}${CMAKE_EXECUTABLE_SUFFIX}")
        set(CMAKE_EXECUTABLE_SUFFIX "-v${ZAGREUS_VERSION_MAJOR}.${ZAGREUS_VERSION_MINOR}${CMAKE_EXECUTABLE_SUFFIX}")
    endif ()
endif ()

# Apply toggleable flags to profiles
if (ENABLE_OPTIMIZATION)
    if (ENABLE_OPTIMIZATION_FAST_MATH)
        set(RELEASE_FLAGS "${RELEASE_FLAGS} -Ofast")
        set(DEBUG_FLAGS "${DEBUG_FLAGS} -Ofast")
        set(PROFILING_FLAGS "${PROFILING_FLAGS} -Ofast")
    else ()
        set(RELEASE_FLAGS "${RELEASE_FLAGS} -O3")
        set(DEBUG_FLAGS "${DEBUG_FLAGS} -O3")
        set(PROFILING_FLAGS "${PROFILING_FLAGS} -O3")
    endif ()
else ()
    set(RELEASE_FLAGS "${RELEASE_FLAGS} -O0 -fno-inline")
    set(DEBUG_FLAGS "${DEBUG_FLAGS} -O0 -fno-inline")
    set(PROFILING_FLAGS "${PROFILING_FLAGS} -O0 -fno-inline")
endif ()

if (ENABLE_LTO)
    set(RELEASE_FLAGS "${RELEASE_FLAGS} -flto")
endif ()

if (ENABLE_WARNINGS)
    set(DEBUG_FLAGS "${DEBUG_FLAGS} -Wall -Wextra")
    set(PROFILING_FLAGS "${PROFILING_FLAGS} -Wall -Wextra")
    set(RELEASE_FLAGS "${RELEASE_FLAGS} -Wall -Wextra")
endif ()

if (ENABLE_MARCH)
    set(MARCH_FLAG "-march=${MARCH_VALUE}")
endif ()

if (ENABLE_MTUNE)
    set(MTUNE_FLAG "-mtune=${MTUNE_VALUE}")
endif ()

if (ENABLE_SANITIZER)
    if (WIN32)
        set(DEBUG_FLAGS "${DEBUG_FLAGS} -fsanitize=address,undefined -fsanitize-trap=all")
        set(PROFILING_FLAGS "${PROFILING_FLAGS} -fsanitize=address,undefined -fsanitize-trap=all")
        set(RELEASE_FLAGS "${RELEASE_FLAGS} -fsanitize=address,undefined -fsanitize-trap=all")
    else ()
        set(DEBUG_FLAGS "${DEBUG_FLAGS} -fsanitize=address,undefined,leak")
        set(PROFILING_FLAGS "${PROFILING_FLAGS} -fsanitize=address,undefined,leak")
        set(RELEASE_FLAGS "${RELEASE_FLAGS} -fsanitize=address,undefined,leak")
    endif ()
endif ()

# Construct the final flags based on the selected profile and toggleable flags
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_FLAGS "${DEBUG_FLAGS}")
elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    set(BUILD_FLAGS "${PROFILING_FLAGS}")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(BUILD_FLAGS "${RELEASE_FLAGS}")
endif ()

# Add the march and mtune flags
if (ENABLE_MARCH)
    set(BUILD_FLAGS "${BUILD_FLAGS} ${MARCH_FLAG}")
    if (ENABLE_MTUNE)
        set(BUILD_FLAGS "${BUILD_FLAGS} ${MTUNE_FLAG}")
    endif ()
endif ()

# Set the flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${BUILD_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${BUILD_FLAGS}")

file(GLOB inc_zagreus "src/*.h" "src/*.cpp")

add_executable(Zagreus src/main.cpp ${inc_zagreus})

target_compile_definitions(Zagreus PRIVATE ZAGREUS_VERSION_MAJOR="${ZAGREUS_VERSION_MAJOR}")
target_compile_definitions(Zagreus PRIVATE ZAGREUS_VERSION_MINOR="${ZAGREUS_VERSION_MINOR}")

if (ENABLE_IWYU)
    find_program(IWYU_PATH NAMES "include-what-you-use")

    if (IWYU_PATH)
        set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "${IWYU_PATH}")
        set_property(TARGET Zagreus PROPERTY CXX_INCLUDE_WHAT_YOU_USE "${IWYU_PATH};-Xiwyu;--error;-Xiwyu;--no_comments;-Xiwyu;--cxx17ns;-Xiwyu;--no_fwd_decls")
        message(STATUS "IWYU found and enabled: ${IWYU_PATH}")
    else ()
        message(WARNING "IWYU requested but not found. Please install IWYU or disable ENABLE_IWYU.")
    endif ()
endif ()

if (ENABLE_TUNER)
    add_compile_definitions(ZAGREUS_TUNER)
endif ()

if (ENABLE_TESTS)
    file(GLOB tests_folder "tests/*.h" "tests/*.cpp")

    # Remove main from inc_zagreus
    list(REMOVE_ITEM inc_zagreus "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

    add_executable(zagreus-tests ${tests_folder} ${inc_zagreus})
    target_link_libraries(zagreus-tests PRIVATE Catch2::Catch2WithMain)

    target_compile_definitions(zagreus-tests PRIVATE ZAGREUS_VERSION_MAJOR="${ZAGREUS_VERSION_MAJOR}")
    target_compile_definitions(zagreus-tests PRIVATE ZAGREUS_VERSION_MINOR="${ZAGREUS_VERSION_MINOR}")

    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
    include(CTest)
    include(Catch)
    catch_discover_tests(zagreus-tests)
endif ()
