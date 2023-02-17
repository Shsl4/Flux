project(FluxUI VERSION 0.1 LANGUAGES C CXX)

file(GLOB_RECURSE FLUXUI_SOURCES ${CMAKE_CURRENT_LIST_DIR}/Source/*.c*)
file(GLOB_RECURSE FLUXUI_HEADERS ${CMAKE_CURRENT_LIST_DIR}/Include/*.h*)
file(GLOB_RECURSE FLUXUI_INLINE ${CMAKE_CURRENT_LIST_DIR}/Include/*.inl)

filterSources("${FLUXUI_SOURCES}")
filterSources("${FLUXUI_HEADERS}")
filterSources("${FLUXUI_INLINE}")

add_library(FluxUI STATIC ${FLUXUI_SOURCES} ${FLUXUI_HEADERS} ${FLUXUI_INLINE})
add_dependencies(FluxUI Nucleus)
set_target_properties(FluxUI PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Build/)

find_package(skia CONFIG REQUIRED)
target_link_libraries(FluxUI PUBLIC skia skia::skia)

set(FLUXUI_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/Include" PARENT_SCOPE)
set(FLUXUI_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/Include")

target_include_directories(FluxUI PUBLIC ${FLUXUI_INCLUDE} ${NUCLEUS_INCLUDE})
target_include_directories(FluxUI PRIVATE "${CMAKE_CURRENT_LIST_DIR}/Include/Flux/UI")
