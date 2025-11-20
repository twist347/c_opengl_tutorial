set(BUFFER_DEMO_SOURCES
        01_triangle.c
        02_triangle_refactoring.c
        03_two_triangles.c
        04_two_triangles.c
        05_rectangle.c
)

foreach (src ${BUFFER_DEMO_SOURCES})
    get_filename_component(target ${src} NAME_WE)

    add_executable(${target} ${src})
    target_compile_definitions(${target} PRIVATE GLFW_INCLUDE_NONE)
    target_link_libraries(${target} PRIVATE glad glfw OpenGL::GL m)
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
endforeach ()
