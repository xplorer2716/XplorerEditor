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

add_executable(xpl_tests_midi
    midi/MidiMessageTests.cpp
    midi/MockMidiBackendTests.cpp
    midi/SysexStreamIteratorTests.cpp)
target_compile_definitions(xpl_tests_midi PRIVATE
    XPL_FIXTURES_DIR="${XPL_FIXTURES_DIR}")
target_link_libraries(xpl_tests_midi PRIVATE
    xpl_midi xpl::warnings Catch2::Catch2WithMain)
catch_discover_tests(xpl_tests_midi)

add_executable(xpl_tests_midi_juce
    midi/JuceMidiBackendTests.cpp)
target_link_libraries(xpl_tests_midi_juce PRIVATE
    xpl_midi_juce xpl::warnings Catch2::Catch2WithMain)
catch_discover_tests(xpl_tests_midi_juce)

add_executable(xpl_tests_framework
    framework/AbstractParameterTests.cpp
    framework/AbstractToneTests.cpp
    framework/DualDictionaryTests.cpp
    framework/AbstractControllerTests.cpp
    framework/ServicesTests.cpp)
target_link_libraries(xpl_tests_framework PRIVATE
    xpl_framework xpl::warnings Catch2::Catch2WithMain)
catch_discover_tests(xpl_tests_framework)

add_executable(xpl_tests_model
    model/XpanderParameterTests.cpp
    model/XpanderSinglePatchTests.cpp
    model/XpanderToneTests.cpp
    model/XpanderToneIOTests.cpp)
target_compile_definitions(xpl_tests_model PRIVATE
    XPL_FIXTURES_DIR="${XPL_FIXTURES_DIR}")
target_link_libraries(xpl_tests_model PRIVATE
    xpl_model xpl::warnings Catch2::Catch2WithMain)
catch_discover_tests(xpl_tests_model)

add_executable(xpl_tests_settings
    settings/SettingsServiceTests.cpp)
target_link_libraries(xpl_tests_settings PRIVATE
    xpl_settings xpl::warnings Catch2::Catch2WithMain)
catch_discover_tests(xpl_tests_settings)

add_executable(xpl_tests_controller
    controller/XpanderControllerTests.cpp)
target_compile_definitions(xpl_tests_controller PRIVATE
    XPL_FIXTURES_DIR="${XPL_FIXTURES_DIR}")
target_link_libraries(xpl_tests_controller PRIVATE
    xpl_controller xpl::warnings Catch2::Catch2WithMain)
catch_discover_tests(xpl_tests_controller)
