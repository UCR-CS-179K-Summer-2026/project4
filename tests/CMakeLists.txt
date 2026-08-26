add_executable(project4_tests test_runner.cpp)
target_link_libraries(project4_tests PRIVATE detector_core)

add_test(NAME project4_smoke COMMAND project4_tests --suite smoke)
add_test(NAME project4_edge_cases COMMAND project4_tests --suite edge)
add_test(NAME project4_detector_coverage COMMAND project4_tests --suite detectors)
add_test(NAME project4_implementation COMMAND project4_tests --suite implementation)
add_test(NAME project4_clean_control COMMAND project4_tests --case clean-control)

set_tests_properties(
    project4_smoke
    project4_edge_cases
    project4_detector_coverage
    project4_implementation
    project4_clean_control
    PROPERTIES WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
