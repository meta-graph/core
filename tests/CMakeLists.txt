# MetaGraph Tests
# Minimal test setup for CI validation

# Create a basic test that always passes for now
add_executable(placeholder_test placeholder_test.c)
target_link_libraries(placeholder_test metagraph::metagraph)

# Add the test to CTest
add_test(NAME placeholder_test COMMAND placeholder_test)

# Set test properties
set_tests_properties(placeholder_test PROPERTIES
    TIMEOUT 10
    LABELS "unit;placeholder"
)

add_executable(dpoi_qca_rmg_test dpoi_qca_rmg_test.c)
target_link_libraries(dpoi_qca_rmg_test metagraph::metagraph)
target_compile_options(dpoi_qca_rmg_test PRIVATE $<$<CONFIG:Release>:-UNDEBUG>)

add_test(NAME dpoi_qca_rmg_test COMMAND dpoi_qca_rmg_test)
set_tests_properties(dpoi_qca_rmg_test PROPERTIES
    TIMEOUT 20
    LABELS "unit;dpoi_qca"
)

if(METAGRAPH_FUZZING)
    add_subdirectory(fuzz)
endif()
