cmake_minimum_required(VERSION 3.20)

set(T c_opengl_tutorial)

project(${T} LANGUAGES C)

find_package(OpenGL REQUIRED)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(DEMOS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/demos)
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(OGL_UTIL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/util)
set(GLFW_DIR ${EXTERNAL_DIR}/glfw-3.4)
set(GLAD_DIR ${EXTERNAL_DIR}/glad)
set(CGLM_DIR ${EXTERNAL_DIR}/cglm-0.9.6)
set(STB_IMAGE_DIR ${EXTERNAL_DIR}/stb_image)

add_subdirectory(${OGL_UTIL_DIR})

add_subdirectory(${GLFW_DIR} EXCLUDE_FROM_ALL)
add_subdirectory(${CGLM_DIR} EXCLUDE_FROM_ALL)
add_subdirectory(${GLAD_DIR} EXCLUDE_FROM_ALL)

add_subdirectory(${DEMOS_DIR})
