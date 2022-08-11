project(FluxApplication VERSION 0.1 LANGUAGES C CXX)

file(GLOB_RECURSE FLUXAPP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/Source/*.c*)
file(GLOB_RECURSE FLUXAPP_HEADERS ${CMAKE_CURRENT_LIST_DIR}/Include/*.h*)
file(GLOB_RECURSE FLUXAPP_INLINE ${CMAKE_CURRENT_LIST_DIR}/Include/*.inl)

set(FLUXAPP_ALL ${FLUXAPP_SOURCES} ${FLUXAPP_HEADERS} ${FLUXAPP_INLINE})

add_executable(FluxApplication ${FLUXAPP_SOURCES} ${FLUXAPP_HEADERS} ${FLUXAPP_INLINE})
add_dependencies(FluxApplication FluxCore FluxUI FluxAudio)
set_target_properties(FluxApplication PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Build/)

filterSources("${FLUXAPP_ALL}")

find_package(rtmidi CONFIG REQUIRED)
find_package(rtaudio CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

if(WIN32)

target_link_libraries(FluxApplication FluxCore FluxUI FluxAudio RtMidi::rtmidi RtAudio::rtaudio glfw winmm.lib dbghelp.lib)

elseif(APPLE)

find_library(FOUNDATION Foundation)
find_library(COCOA Cocoa)
find_library(IOKIT IoKit)
find_library(QUARTZCORE QuartzCore)
find_package(webp CONFIG REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)

target_link_libraries(FluxApplication PUBLIC ${FOUNDATION} ${COCOA} ${IOKIT} ${QUARTZCORE} FluxCore FluxUI FluxAudio RtMidi::rtmidi RtAudio::rtaudio JPEG::JPEG PNG::PNG ${WEBP_LIBRARIES} glfw)

else()

set(THREADS_PREFER_PTHREAD_FLAG ON)
set(OpenGL_GL_PREFERENCE GLVND)

find_package(X11 REQUIRED)
find_package(Threads REQUIRED)
find_package(webp CONFIG REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)

target_include_directories(FluxApplication PUBLIC ${X11_INCLUDE_DIR})

target_link_libraries(FluxApplication PUBLIC ${X11_LIBRARIES} ${X11_Xrandr_LIB} Threads::Threads ${CMAKE_DL_LIBS} FluxCore FluxUI FluxAudio RtMidi::rtmidi RtAudio::rtaudio JPEG::JPEG ${WEBP_LIBRARIES} glfw PNG::PNG)

endif()

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT FluxApplication)

target_include_directories(FluxApplication PUBLIC ${FLUXCORE_INCLUDE} ${FLUXUI_INCLUDE} ${FLUXAUDIO_INCLUDE})
target_include_directories(FluxApplication PRIVATE "${CMAKE_CURRENT_LIST_DIR}/Include/")
