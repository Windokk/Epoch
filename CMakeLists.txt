cmake_minimum_required(VERSION 3.28.2)

set(NAME SHA.me)
project(${NAME} VERSION 1.0.0)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

add_definitions(-DGLFW_INCLUDE_NONE -DGLFW_STATIC)

if(WIN32)
  set(GLFW_LIB_DIR "${CMAKE_SOURCE_DIR}/src/engine/third-party/lib/glfw/glfw-win")
  set(FREETYPE_LIB_DIR "${CMAKE_SOURCE_DIR}/src/engine/third-party/lib/freetype/freetype-win")
  set(COSEM_PACKER_LIB_DIR "${CMAKE_SOURCE_DIR}/src/engine/filesystem/packer")
  set(JOLT_LIB_DIR "${CMAKE_SOURCE_DIR}/src/engine/third-party/lib/jolt/jolt-win")
elseif(UNIX)
  set(GLFW_LIB_DIR "${CMAKE_SOURCE_DIR}/src/engine/third-party/lib/glfw/glfw-linux")
  set(FREETYPE_LIB_DIR "${CMAKE_SOURCE_DIR}/src/engine/third-party/lib/freetype/freetype-linux")
  set(COSEM_PACKER_LIB_DIR "${CMAKE_SOURCE_DIR}/src/engine/filesystem/packer/linux")
  set(JOLT_LIB_DIR "${CMAKE_SOURCE_DIR}/src/engine/third-party/lib/jolt/jolt-linux")
endif()

set(RESOURCES_LIB_DIR "${CMAKE_BINARY_DIR}")

file(GLOB_RECURSE SOURCES
  ${CMAKE_SOURCE_DIR}/src/game/main.cpp
  ${CMAKE_SOURCE_DIR}/src/engine/**.cpp
  ${CMAKE_SOURCE_DIR}/src/engine/third-party/lib/glad.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC 
    "${CMAKE_SOURCE_DIR}/src/engine/third-party/include/"
    "${CMAKE_SOURCE_DIR}/src/"
  )

target_link_directories(${PROJECT_NAME} PUBLIC
    ${GLFW_LIB_DIR}
    ${FREETYPE_LIB_DIR}
    ${COSEM_PACKER_LIB_DIR}
    ${JOLT_LIB_DIR}
  )

target_compile_definitions(${PROJECT_NAME} PRIVATE JPH_PROFILE_ENABLED JPH_DEBUG_RENDERER JPH_OBJECT_STREAM)

target_link_libraries(${PROJECT_NAME} freetype CosemPacker Jolt glfw3 -lz opengl32 gdi32 user32 shell32 advapi32)