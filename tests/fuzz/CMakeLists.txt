set(_metagraph_fuzz_targets
    bundle-parser
    graph-traversal
    dependency-resolver
    memory-pool
)

foreach(_mg_name IN LISTS _metagraph_fuzz_targets)
    set(_mg_target "fuzz-${_mg_name}")
    string(REPLACE "-" "_" _mg_file_suffix "${_mg_name}")
    set(_mg_source "${CMAKE_CURRENT_SOURCE_DIR}/fuzz_${_mg_file_suffix}.c")

    if(NOT EXISTS "${_mg_source}")
        message(FATAL_ERROR "Missing fuzz source for ${_mg_target}: ${_mg_source}")
    endif()

    add_executable(${_mg_target} ${_mg_source})
    target_link_libraries(${_mg_target} PRIVATE metagraph::metagraph)
    target_include_directories(${_mg_target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_features(${_mg_target} PRIVATE c_std_23)
    set_target_properties(${_mg_target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests/fuzz"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests/fuzz"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests/fuzz"
    )
endforeach()

unset(_metagraph_fuzz_targets)
