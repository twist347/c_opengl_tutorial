set(SHADER_DEMO_SOURCE
        01_shader/01_shader.c
        02_uniform/02_uniform.c
        03_uniform/03_uniform.c
        04_uniform/04_uniform.c
        05_uniform/05_uniform.c
        06_circle/06_circle.c
        07_circle/07_circle.c
)

foreach (src ${SHADER_DEMO_SOURCE})
    get_filename_component(target ${src} NAME_WE)
    get_filename_component(src_dir ${src} DIRECTORY)

    add_executable(${target} ${src})
    target_compile_definitions(${target} PRIVATE GLFW_INCLUDE_NONE)
    target_link_libraries(${target} PRIVATE m ogt_util)
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)

    set_target_properties(${target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${src_dir}"
    )

    add_custom_target(copy_shaders_${target} ALL
            COMMAND ${CMAKE_COMMAND} -E make_directory
            "${CMAKE_CURRENT_BINARY_DIR}/${src_dir}/shaders"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/${src_dir}/shaders"
            "${CMAKE_CURRENT_BINARY_DIR}/${src_dir}/shaders"
    )

    add_dependencies(${target} copy_shaders_${target})
endforeach ()
