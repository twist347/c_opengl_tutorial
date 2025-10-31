set(T 01_window)

add_executable(${T} main.c)
target_compile_definitions(${T} PRIVATE GLFW_INCLUDE_NONE)
target_include_directories(${T} PRIVATE ${STB_IMAGE_DIR})
target_link_libraries(${T} PRIVATE glad glfw OpenGL::GL cglm)
target_compile_options(${T} PRIVATE -Wall -Wextra -Wpedantic)
