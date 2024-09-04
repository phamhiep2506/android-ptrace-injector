cmake_minimum_required(VERSION 3.30.2)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
if(NOT CMAKE_ANDROID_NDK)
  set(CMAKE_ANDROID_NDK /opt/sdk/ndk/27.0.12077973)
endif()
set(CMAKE_TOOLCHAIN_FILE
    ${CMAKE_ANDROID_NDK}/build/cmake/android.toolchain.cmake)
set(CMAKE_ANDROID_STANDALONE_TOOLCHAIN
    ${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64)
set(ANDROID_ABI arm64-v8a)
set(ANDROID_PLATFORM 21)

project(Injector)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

add_compile_options(-O3)

add_executable(${PROJECT_NAME} ${SOURCES_FILE})

add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_ANDROID_STANDALONE_TOOLCHAIN}/bin/llvm-strip ARGS --strip-all
          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})
