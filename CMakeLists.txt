cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(LINUX UNIX AND NOT APPLE)
set(CMAKE_DEBUG_POSTFIX d)

project(Flux VERSION 0.1 LANGUAGES C CXX)

function(filterSources LIST)
    foreach(FILE ${LIST})
        get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
        string(REPLACE "${CMAKE_CURRENT_LIST_DIR}" "" GROUP "${PARENT_DIR}")
        string(REPLACE "/" "\\" GROUP "${GROUP}")
        source_group("${GROUP}" FILES "${FILE}")
    endforeach()
endfunction()

if(APPLE)
    file(GLOB_RECURSE FLUX_SOURCES ${CMAKE_CURRENT_LIST_DIR}/Source/*.c* ${CMAKE_CURRENT_LIST_DIR}/Source/*.m*)
else()
    file(GLOB_RECURSE FLUX_SOURCES ${CMAKE_CURRENT_LIST_DIR}/Source/*.c*)
endif()

file(GLOB_RECURSE FLUX_HEADERS ${CMAKE_CURRENT_LIST_DIR}/Include/*.h*)
file(GLOB_RECURSE FLUX_INLINE ${CMAKE_CURRENT_LIST_DIR}/Include/*.inl)
file(GLOB_RECURSE FLUX_DEPENDENCIES ${CMAKE_CURRENT_LIST_DIR}/Libraries/.build/*)
set(FLUX_DEPENDENCIES_INCLUDES ${CMAKE_CURRENT_LIST_DIR}/Libraries/ 
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/skia/ 
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/Nucleus/Include
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/glfw/include)

set(FLUX_ALL ${FLUX_SOURCES} ${FLUX_HEADERS} ${FLUX_INLINE})

add_compile_definitions(FLUX_VERSION="${CMAKE_PROJECT_VERSION}")

add_executable(Flux ${FLUX_SOURCES} ${FLUX_HEADERS} ${FLUX_INLINE})

filterSources("${FLUX_ALL}")

if(WIN32)

target_link_libraries(Flux ${FLUX_DEPENDENCIES} winmm.lib dbghelp.lib)

elseif(APPLE)

find_library(FOUNDATION Foundation)
find_library(COREMIDI CoreMidi)
find_library(COREAUDIO CoreAudio)
find_library(COCOA Cocoa)
find_library(IOKIT IoKit)
find_library(QUARTZCORE QuartzCore)
find_library(METAL Metal)
find_library(METALKIT MetalKit)
find_library(OPENGL OpenGL)

target_link_libraries(Flux PUBLIC ${FOUNDATION} ${COCOA} ${IOKIT} ${QUARTZCORE} ${METAL} ${METALKIT} ${COREMIDI} ${COREAUDIO} ${OPENGL} ${FLUX_DEPENDENCIES})

else()

set(THREADS_PREFER_PTHREAD_FLAG ON)
set(OpenGL_GL_PREFERENCE GLVND)

find_package(X11 REQUIRED)
find_package(Threads REQUIRED)

target_include_directories(Flux PUBLIC ${X11_INCLUDE_DIR})
target_link_libraries(Flux PUBLIC ${X11_LIBRARIES} ${X11_Xrandr_LIB} Threads::Threads ${CMAKE_DL_LIBS} ${FLUX_DEPENDENCIES})

endif()

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Flux)

target_include_directories(Flux PUBLIC ${NUCLEUS_INCLUDE})
target_include_directories(Flux PRIVATE "${CMAKE_CURRENT_LIST_DIR}/Include/" "${CMAKE_CURRENT_LIST_DIR}/Include/Flux" ${FLUX_DEPENDENCIES_INCLUDES})

set_target_properties(Flux PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Build/)