set(SC_CONFIG_UTILS_SRC ${SC_TOOLS_SRC}/sc-config-utils)
set(SC_BUILDER_SRC ${SC_TOOLS_SRC}/sc-builder)

add_subdirectory(codegen)
add_subdirectory(sc-config-utils)
add_subdirectory(sc-server)
add_subdirectory(sc-builder)

add_custom_target(sc-tools)

add_dependencies(sc-tools
    sc-config-utils
    sc-builder
)

macro(subdir_list result current_dir)
    file(GLOB children RELATIVE ${current_dir} ${current_dir}/*)
    set(dirlist "")
    foreach(child ${children})
        if(IS_DIRECTORY ${current_dir}/${child})
            list(APPEND dirlist ${child})
        endif()
    endforeach()
    set(${result} ${dirlist})
endmacro()

set(SUBSYSTEMS_PATH "${CMAKE_CURRENT_LIST_DIR}/subsystems")
subdir_list(SUBDIRS ${SUBSYSTEMS_PATH})

foreach(SUBDIR ${SUBDIRS})
    add_subdirectory(${SUBSYSTEMS_PATH}/${SUBDIR})
endforeach()
