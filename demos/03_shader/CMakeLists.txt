set(SHADER_DEMO_SOURCE
        01_shader/01_shader.c
)

foreach (src ${SHADER_DEMO_SOURCE})
    get_filename_component(target ${src} NAME_WE)

    get_filename_component(src_dir ${src} DIRECTORY)

    add_executable(${target} ${src})
    target_compile_definitions(${target} PRIVATE GLFW_INCLUDE_NONE)
    target_link_libraries(${target} PRIVATE glad glfw OpenGL::GL m ogl_util)
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)

    set_target_properties(${target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${src_dir}"
    )

    add_custom_command(
            TARGET ${target}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${target}>/shaders"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/${src_dir}/shaders"
            "$<TARGET_FILE_DIR:${target}>/shaders"
    )
endforeach ()
