cmake_minimum_required(VERSION 3.28.2)

set(CMAKE_COLOR_MAKEFILE ON)


project(SHA.me VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

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

add_definitions(-DGLFW_INCLUDE_NONE -DGLFW_STATIC)

# === Engine Sources ===
file(GLOB_RECURSE ENGINE_SOURCES
  ${CMAKE_SOURCE_DIR}/src/engine/*.cpp
  ${CMAKE_SOURCE_DIR}/src/engine/third-party/lib/glad.c
  ${CMAKE_SOURCE_DIR}/src/engine/third-party/lib/ufbx.c
)

add_library(SHAMEEngine STATIC ${ENGINE_SOURCES})

target_include_directories(SHAMEEngine PUBLIC 
  "${CMAKE_SOURCE_DIR}/src/engine/third-party/include/"
  "${CMAKE_SOURCE_DIR}/src/"
)

target_link_directories(SHAMEEngine PUBLIC
  ${GLFW_LIB_DIR}
  ${FREETYPE_LIB_DIR}
  ${COSEM_PACKER_LIB_DIR}
  ${JOLT_LIB_DIR}
  ${FMOD_LIB_DIR}
)

target_compile_definitions(SHAMEEngine PRIVATE BUILD_ENGINE JPH_PROFILE_ENABLED JPH_DEBUG_RENDERER JPH_OBJECT_STREAM)

if(WIN32)
  target_link_libraries(SHAMEEngine PUBLIC freetype CosemPacker Jolt fmod glfw3 -lz opengl32 gdi32 user32 shell32 advapi32)
elseif(UNIX)
  find_package(PNG REQUIRED)
  find_package(BZip2 REQUIRED)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(BROTLI REQUIRED IMPORTED_TARGET libbrotlidec)

  target_link_libraries(SHAMEEngine PUBLIC freetype CosemPacker Jolt fmod glfw3 -lz PNG::PNG BZip2::BZip2 PkgConfig::BROTLI)
endif()

# === Game Module (game logic) ===

file(GLOB_RECURSE GAME_SOURCES
  ${CMAKE_SOURCE_DIR}/src/game/*.cpp
)

add_library(GameModule SHARED ${GAME_SOURCES})

target_include_directories(GameModule PRIVATE
  ${CMAKE_SOURCE_DIR}/src/
)

target_link_libraries(GameModule PRIVATE SHAMEEngine)


# === Launcher (full game executable) ===

add_executable(SHA.me ${CMAKE_SOURCE_DIR}/src/launcher/main.cpp)

target_link_libraries(SHA.me PRIVATE SHAMEEngine)

target_include_directories(SHA.me PRIVATE
  ${CMAKE_SOURCE_DIR}/src/
)