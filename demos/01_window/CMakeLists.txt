set(DEMO_SOURCES
        01_window.c
)

foreach (src ${DEMO_SOURCES})
    get_filename_component(target ${src} NAME_WE)

    add_executable(${target} ${src})
    target_compile_definitions(${target} PRIVATE GLFW_INCLUDE_NONE)
    target_link_libraries(${target} PRIVATE glad glfw OpenGL::GL)
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
endforeach ()

