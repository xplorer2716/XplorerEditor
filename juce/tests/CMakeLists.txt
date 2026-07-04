# Test suites: one executable per layer, Catch2 + CTest. [RQ-TST-001, ADR-003]
set(XPL_FIXTURES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/fixtures")

add_executable(xpl_tests_smoke
    smoke/SmokeTests.cpp)
target_compile_definitions(xpl_tests_smoke PRIVATE
    XPL_FIXTURES_DIR="${XPL_FIXTURES_DIR}")
target_link_libraries(xpl_tests_smoke PRIVATE
    xpl_controller xpl_settings xpl_midi_juce
    xpl::warnings
    Catch2::Catch2WithMain)
catch_discover_tests(xpl_tests_smoke)
