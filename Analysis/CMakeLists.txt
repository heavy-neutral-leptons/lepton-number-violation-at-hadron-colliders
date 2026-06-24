# Copyright 2023-2026 Bruno M. S. Oliveira

# ----- CMake Version -----
cmake_minimum_required(VERSION 3.30)

# ----- Require out-of-source builds -----
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(
        FATAL_ERROR
        "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles."
    )
endif()

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(
        STATUS
        "Setting build type to '${default_build_type}' as none was specified."
    )
    set(CMAKE_BUILD_TYPE
        "${default_build_type}"
        CACHE STRING
        "Choose the type of build."
        FORCE
    )
    set_property(
        CACHE CMAKE_BUILD_TYPE
        PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo"
    )
endif()

# ----- Project Setup -----
project(
    SPSS-Searches
    DESCRIPTION
        "Searches for HNLs in symmetry protected type-I seesaw scenarios"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wno-cpp
)
add_link_options(-Wl,--as-needed)

# ----- External Packages -----
find_package(ROOT CONFIG REQUIRED)

set(DELPHES_PATH $ENV{MG5_HOME}/Delphes)
add_library(Delphes::Delphes SHARED IMPORTED)
set_target_properties(
    Delphes::Delphes
    PROPERTIES
        INCLUDE_DIRECTORIES "${DELPHES_PATH}"
        IMPORTED_IMPLIB "${DELPHES_PATH}/libDelphes.so"
)

# ----- Library Code -----
add_subdirectory(SimulationReaders)

add_subdirectory(Common)
add_subdirectory(Reconstruction)
add_subdirectory(ReconstructedData)

add_subdirectory(MCPID)
add_subdirectory(Models)

add_subdirectory(Statistics)

add_subdirectory(MG5_aMC)
add_subdirectory(DelphesWrapper)

add_subdirectory(MachineLearning)

add_subdirectory(DimuonSS)
add_subdirectory(Trimuon)

# ----- Executable Code -----
add_subdirectory(Summary)

add_subdirectory(DimuonSS-FCChh-Generator)
add_subdirectory(DimuonSS-FCChh-Detector)

add_subdirectory(Trimuon-FCChh-Generator)
add_subdirectory(Trimuon-FCChh-Detector)

add_subdirectory(TMVA-Train)
add_subdirectory(TMVA-Apply)
add_subdirectory(TMVA-Separation)
add_subdirectory(TMVA-Distribution)
