message(STATUS "Building GLES")

set(EGL_PATH "${CMAKE_SOURCE_DIR}/EGL")
set(GLES_PATH "${CMAKE_SOURCE_DIR}/GLES")
set(GTEST_PATH "${CMAKE_SOURCE_DIR}/External/googletest" CACHE PATH "")
set(GLSLANG_PATH "${CMAKE_SOURCE_DIR}/External/glslang" CACHE PATH "")

if(WIN32)
    add_definitions(-DKHRONOS_STATIC)
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${CMAKE_INSTALL_PREFIX})

add_subdirectory(source)

if(NOT WIN32 AND NOT APPLE)
    add_subdirectory(unitTests)
endif()
