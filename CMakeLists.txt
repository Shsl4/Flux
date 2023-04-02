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

file(GLOB_RECURSE FLUX_DEBUG_DEPENDENCIES ${CMAKE_CURRENT_LIST_DIR}/Libraries/.build/Debug/*)
file(GLOB_RECURSE FLUX_RELEASE_DEPENDENCIES ${CMAKE_CURRENT_LIST_DIR}/Libraries/.build/Release/*)

set(FLUX_DEPENDENCIES_INCLUDES ${CMAKE_CURRENT_LIST_DIR}/Libraries/ 
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/skia/ 
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/Nucleus/Include
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/glfw/include
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/Vulkan-Headers/include)

set(FLUX_ALL ${FLUX_SOURCES} ${FLUX_HEADERS} ${FLUX_INLINE})

add_compile_definitions(FLUX_VERSION="${CMAKE_PROJECT_VERSION}")
add_compile_definitions(FLUX_RESOURCES=\"${CMAKE_CURRENT_LIST_DIR}/Resources\")

add_executable(Flux ${FLUX_SOURCES} ${FLUX_HEADERS} ${FLUX_INLINE})

filterSources("${FLUX_ALL}")

if(WIN32)

target_link_libraries(Flux debug ${FLUX_DEBUG_DEPENDENCIES} winmm.lib dbghelp.lib d3d12.lib dxgi.lib d3dcompiler.lib opengl32.lib)

target_link_libraries(Flux optimized ${FLUX_RELEASE_DEPENDENCIES} winmm.lib dbghelp.lib d3d12.lib dxgi.lib d3dcompiler.lib  opengl32.lib)

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

target_link_libraries(Flux PUBLIC debug ${FOUNDATION} ${COCOA} ${IOKIT} ${QUARTZCORE} ${METAL} ${METALKIT} ${COREMIDI} ${COREAUDIO} ${OPENGL} ${FLUX_DEBUG_DEPENDENCIES})

target_link_libraries(Flux PUBLIC optimized ${FOUNDATION} ${COCOA} ${IOKIT} ${QUARTZCORE} ${METAL} ${METALKIT} ${COREMIDI} ${COREAUDIO} ${OPENGL} ${FLUX_RELEASE_DEPENDENCIES})

else()

set(THREADS_PREFER_PTHREAD_FLAG ON)
set(OpenGL_GL_PREFERENCE GLVND)

find_package(X11 REQUIRED)
find_package(Threads REQUIRED)
find_package(ALSA REQUIRED)
find_package(PulseAudio REQUIRED)
find_package(Freetype REQUIRED)
find_package(Fontconfig REQUIRED)
find_library(PULSEAUDIO_LIBRARY NAMES pulse)
find_package(OpenGL REQUIRED)

target_include_directories(Flux PUBLIC ${X11_INCLUDE_DIR})

target_link_libraries(Flux PUBLIC debug ${X11_LIBRARIES} ${X11_Xrandr_LIB} ${ALSA_LIBRARIES} ${FREETYPE_LIBRARIES} ${Fontconfig_LIBRARIES} ${OPENGL_LIBRARIES} pulse pulse-simple Threads::Threads ${CMAKE_DL_LIBS} ${FLUX_DEBUG_DEPENDENCIES})
target_link_libraries(Flux PUBLIC optimized ${X11_LIBRARIES} ${X11_Xrandr_LIB} ${ALSA_LIBRARIES} ${FREETYPE_LIBRARIES} ${OPENGL_LIBRARIES} pulse pulse-simple Threads::Threads ${CMAKE_DL_LIBS} ${FLUX_RELEASE_DEPENDENCIES})

endif()

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Flux)

target_include_directories(Flux PRIVATE "${CMAKE_CURRENT_LIST_DIR}/Include/" "${CMAKE_CURRENT_LIST_DIR}/Include/Flux" ${FLUX_DEPENDENCIES_INCLUDES})

set_target_properties(Flux PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Build/)
