set(TEXTURE_DEMO_SOURCE
        01_texture/01_texture.c
        02_texture/02_texture.c
        03_texture/03_texture.c
        04_texture/04_texture.c
)

foreach (src ${TEXTURE_DEMO_SOURCE})
    get_filename_component(target ${src} NAME_WE)
    get_filename_component(src_dir ${src} DIRECTORY)

    add_executable(${target} ${src})
    target_include_directories(${target} PRIVATE ${STB_IMAGE_DIR})
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

    add_custom_target(copy_textures_${target} ALL
            COMMAND ${CMAKE_COMMAND} -E make_directory
            "${CMAKE_CURRENT_BINARY_DIR}/${src_dir}/textures"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/${src_dir}/textures"
            "${CMAKE_CURRENT_BINARY_DIR}/${src_dir}/textures"
    )

    add_dependencies(${target} copy_shaders_${target})
    add_dependencies(${target} copy_textures_${target})
endforeach ()