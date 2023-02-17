cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(LINUX UNIX AND NOT APPLE)
set(CMAKE_DEBUG_POSTFIX d)

project(Flux VERSION 0.1 LANGUAGES C CXX)

add_compile_definitions(FLUX_VERSION="${CMAKE_PROJECT_VERSION}")

function(filterSources LIST)
    foreach(FILE ${LIST})
        get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
        string(REPLACE "${CMAKE_CURRENT_LIST_DIR}" "" GROUP "${PARENT_DIR}")
        string(REPLACE "/" "\\" GROUP "${GROUP}")
        source_group("${GROUP}" FILES "${FILE}")
    endforeach()
endfunction()

add_subdirectory(${CMAKE_SOURCE_DIR}/Nucleus)
add_subdirectory(${CMAKE_SOURCE_DIR}/FluxUI)
add_subdirectory(${CMAKE_SOURCE_DIR}/FluxAudio)
add_subdirectory(${CMAKE_SOURCE_DIR}/FluxApplication)