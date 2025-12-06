set(TRANSFORM_DEMO_SOURCE
    01_transform/01_transform.c
    02_coordinate_system/02_coordinate_system.c
    03_coordinate_system/03_coordinate_system.c
    04_coordinate_system/04_coordinate_system.c
)

foreach (src ${TRANSFORM_DEMO_SOURCE})
    get_filename_component(target ${src} NAME_WE)
    get_filename_component(src_dir ${src} DIRECTORY)

    add_executable(${target} ${src})
    target_include_directories(${target} PRIVATE ${STB_IMAGE_DIR})
    target_compile_definitions(${target} PRIVATE GLFW_INCLUDE_NONE)
    target_link_libraries(${target} PRIVATE glad glfw cglm OpenGL::GL m ogt_util)
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