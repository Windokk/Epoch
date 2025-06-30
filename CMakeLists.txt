cmake_minimum_required(VERSION 3.28.2)

set(CMAKE_COLOR_MAKEFILE ON)

set(NAME SHA.me)
project(${NAME} VERSION 1.0.0)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

add_definitions(-DGLFW_INCLUDE_NONE -DGLFW_STATIC)

if(WIN32)
  set(GLFW_LIB_DIR "${CMAKE_SOURCE_DIR}/src/engine/third-party/lib/glfw/glfw-win")
  set(FREETYPE_LIB_DIR "${CMAKE_SOURCE_DIR}/src/engine/third-party/lib/freetype/freetype-win")
  set(COSEM_PACKER_LIB_DIR "${CMAKE_SOURCE_DIR}/src/engine/filesystem/packer")
  set(JOLT_LIB_DIR "${CMAKE_SOURCE_DIR}/src/engine/third-party/lib/jolt/jolt-win")
  set(FMOD_LIB_DIR "${CMAKE_SOURCE_DIR}/src/engine/third-party/lib/fmod/fmod-win")
elseif(UNIX)
  set(GLFW_LIB_DIR "${CMAKE_SOURCE_DIR}/src/engine/third-party/lib/glfw/glfw-linux")
  set(FREETYPE_LIB_DIR "${CMAKE_SOURCE_DIR}/src/engine/third-party/lib/freetype/freetype-linux")
  set(COSEM_PACKER_LIB_DIR "${CMAKE_SOURCE_DIR}/src/engine/filesystem/packer/linux")
  set(JOLT_LIB_DIR "${CMAKE_SOURCE_DIR}/src/engine/third-party/lib/jolt/jolt-linux")
  set(FMOD_LIB_DIR "${CMAKE_SOURCE_DIR}/src/engine/third-party/lib/fmod/fmod-linux")
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
    ${FMOD_LIB_DIR}
  )

target_compile_definitions(${PROJECT_NAME} PRIVATE JPH_PROFILE_ENABLED JPH_DEBUG_RENDERER JPH_OBJECT_STREAM)
if(WIN32)
  target_link_libraries(${PROJECT_NAME} freetype CosemPacker Jolt fmod glfw3 -lz opengl32 gdi32 user32 shell32 advapi32)
elseif(UNIX)
  find_package(PNG REQUIRED)
  find_package(BZip2 REQUIRED)
  find_package(PkgConfig REQUIRED)

  pkg_check_modules(BROTLI REQUIRED IMPORTED_TARGET libbrotlidec)

  target_link_libraries(${PROJECT_NAME} freetype CosemPacker Jolt fmod glfw3 -lz PNG::PNG BZip2::BZip2 PkgConfig::BROTLI)
endif()