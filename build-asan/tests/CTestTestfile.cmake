# CMake generated Testfile for 
# Source directory: /Users/james/git/meta-graph/core/tests
# Build directory: /Users/james/git/meta-graph/core/build-asan/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[placeholder_test]=] "/Users/james/git/meta-graph/core/build-asan/bin/placeholder_test")
set_tests_properties([=[placeholder_test]=] PROPERTIES  LABELS "unit;placeholder" TIMEOUT "10" _BACKTRACE_TRIPLES "/Users/james/git/meta-graph/core/tests/CMakeLists.txt;9;add_test;/Users/james/git/meta-graph/core/tests/CMakeLists.txt;0;")
add_test([=[dpoi_qca_rmg_test]=] "/Users/james/git/meta-graph/core/build-asan/bin/dpoi_qca_rmg_test")
set_tests_properties([=[dpoi_qca_rmg_test]=] PROPERTIES  LABELS "unit;dpoi_qca" TIMEOUT "20" _BACKTRACE_TRIPLES "/Users/james/git/meta-graph/core/tests/CMakeLists.txt;21;add_test;/Users/james/git/meta-graph/core/tests/CMakeLists.txt;0;")
