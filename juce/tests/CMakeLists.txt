# Test suites: one executable per layer, Catch2 + CTest. [RQ-TST-001, ADR-JUC-003]
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
    framework/ServicesTests.cpp
    framework/PortableFormatTests.cpp)
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

add_executable(xpl_tests_app
    app/ControlTableTests.cpp
    app/ParameterBindingRegistryTests.cpp
    app/PageFamilyModelTests.cpp
    app/ComboBoxSizingTests.cpp
    app/ModulationHighlightTests.cpp
    app/SegmentFontTests.cpp
    app/WindowGeometryTests.cpp
    # Layout-independent piano key mapping: OS-free, JUCE-free, so it belongs
    # in the headless target and runs even with -DXPL_BUILD_APP=OFF.
    # [RQ-GUI-074, RQ-BLD-025]
    app/PianoKeyMappingTests.cpp
    # Key-label decision logic, measurer injected (ComboBoxSizing's own
    # pattern) so it stays JUCE-free too. [RQ-GUI-075, RQ-BLD-025]
    app/PianoKeyLabelPlanTests.cpp)
target_link_libraries(xpl_tests_app PRIVATE
    xpl_app_core xpl::warnings Catch2::Catch2WithMain)
catch_discover_tests(xpl_tests_app)

# Real-metrics combo-box fit check. Needs JUCE graphics and the embedded font,
# so it only exists in an app-enabled build; the headless layer keeps its
# JUCE-free coverage untouched. This is what makes the RQ-GUI-047 fit promise
# verifiable per platform in CI: jassert only logs when no debugger is
# attached, so the development-build assertion cannot fail a run on its own.
# [RQ-GUI-047, RQ-GUI-048, RQ-DSN-096, ADR-JUC-022 (DEC-JUC-050)]
if(XPL_BUILD_APP)
    add_executable(xpl_tests_app_juce
        app/ComboBoxRealMetricsTests.cpp
        app/VfdSegmentRendererTests.cpp
        app/BackgroundRendererTests.cpp
        # Section vertical rhythm. Reads DesignTokens.hpp (hence this
        # JUCE-linked target rather than the headless one) but instantiates no
        # juce::Graphics: it asserts layout constants against the control table.
        # [RQ-CLR-010, ADR-CLR-001 (DEC-CLR-001-D)]
        app/SectionRhythmTests.cpp
        app/ShortcutIconTests.cpp
        app/DialogIconTests.cpp
        app/BoundRadioGroupTests.cpp
        app/LedPanelGeometryTests.cpp
        app/DialogsPatchNameTests.cpp
        app/SbomReaderTests.cpp
        # The knob LED colour is retuned in place for live preview, so the
        # values derived from it at construction can now go stale where a
        # rebuild made that impossible. [RQ-GUI-073, ADR-JUC-020 (DEC-JUC-113)]
        app/XplorerLookAndFeelTests.cpp
        ../app/src/XplorerLookAndFeel.cpp
        ../app/src/VfdSegmentRenderer.cpp
        ../app/src/BackgroundRenderer.cpp
        ../app/src/ShortcutIcons.cpp
        ../app/src/DialogIcons.cpp
        ../app/src/BlockPalette.cpp
        # BoundControls carries the RQ-GUI-053 orientation rule, which is a
        # widget-geometry decision: it can only be pinned against real
        # juce::Component bounds. [RQ-GUI-053, ADR-JUC-016 (DEC-JUC-086)]
        ../app/src/BoundControls.cpp
        ../app/src/BlockPalette.cpp
        # isPatchNameValid needs the whole translation unit (AlertWindow-based
        # dialogs, controller/settings/midi includes) to compile alongside it.
        # [RQ-GUI-025]
        ../app/src/Dialogs.cpp
        # The SPDX reader is pure data in / data out by design, precisely so its
        # field mapping, filtering and failure results can be pinned without a
        # display. [RQ-GUI-057, ADR-ABT-001 (DEC-ABT-006)]
        ../app/src/SbomReader.cpp)
    target_include_directories(xpl_tests_app_juce PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../app/src)
    # Same JUCE feature switches as the app target, so this test compiles the
    # very same juce_core/juce_graphics configuration the shipped binary does.
    target_compile_definitions(xpl_tests_app_juce PRIVATE
        JUCE_USE_CURL=0
        JUCE_WEB_BROWSER=0
        JUCE_STANDALONE_APPLICATION=1)
    # juce_gui_extra + xpl_settings come in with BackgroundRenderer/BlockPalette
    # (BackgroundRenderer.hpp includes juce_gui_extra; BlockPalette.hpp includes
    # AllUsersSettings.hpp), so the paint-order test of RQ-GUI-051 can render the
    # real painter rather than a re-implementation of it. [ADR-JUC-027]
    target_link_libraries(xpl_tests_app_juce PRIVATE
        xpl_app_core xpl_settings xpl_midi XplorerAssets
        juce::juce_graphics juce::juce_gui_extra juce::juce_events
        xpl::warnings Catch2::Catch2WithMain)
    catch_discover_tests(xpl_tests_app_juce)
endif()
