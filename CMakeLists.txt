cmake_minimum_required(VERSION 3.11)

project(cmake_issue25912_demo
    VERSION 1.0.0
    LANGUAGES CXX
)

# generator --------------------------------------------------------------------
add_executable(schema_compiler)
target_sources(schema_compiler
    PRIVATE
    schema_compiler.cpp
)

# ------------------------------------------------------------------------------
set(schema_file "${CMAKE_CURRENT_LIST_DIR}/schema.txt")
get_filename_component(schema_real_path "${schema_file}" REALPATH)
string(SHA256 anchor_file_name "${schema_real_path}")
# `cmake_path` requires CMake >=3.20 so concatenate by hand
set(anchor_file_path "${CMAKE_BINARY_DIR}/${anchor_file_name}.anchor")

# custom command ---------------------------------------------------------------
add_custom_command(
    VERBATIM
    COMMAND
        "$<TARGET_FILE:schema_compiler>" "${schema_file}"
    DEPENDS
        "${schema_file}" schema_compiler
    COMMENT
        "Compiling schema: ${schema_file}"

    # We don't know any output file name because it depends on the content
    # of the schema so we generate a fake anchor file to depend on it in
    # `add_custom_target`
    COMMAND
        "${CMAKE_COMMAND}" -E touch "${anchor_file_path}"
    OUTPUT
        "${anchor_file_path}"
)

# custom target ----------------------------------------------------------------
set(custom_target_name "compiled_schema_custom_target")

if(NOT TARGET "${custom_target_name}")
    add_custom_target(
        "${custom_target_name}"
        # To support multiple schemas with the same `TARGET_NAME` we depend
        # on an extendable list of anchor files stored in a custom property
        # `SBEPPC_ANCHORS`
        DEPENDS $<TARGET_PROPERTY:${custom_target_name},SBEPPC_ANCHORS>
    )
endif()

set_property(TARGET "${custom_target_name}"
    APPEND PROPERTY SBEPPC_ANCHORS
    "${anchor_file_path}"
)

# interface library ------------------------------------------------------------
set(interface_target_name "compiled_schema")

add_library(${interface_target_name} INTERFACE)

target_include_directories(${interface_target_name}
    SYSTEM INTERFACE "${CMAKE_BINARY_DIR}"
)

add_dependencies(${interface_target_name} ${custom_target_name})

# consumer ---------------------------------------------------------------------
add_executable(consumer)
target_sources(consumer
    PRIVATE
    consumer.cpp
)

target_link_libraries(consumer
    PRIVATE
    ${interface_target_name}
)