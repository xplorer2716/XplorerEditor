# xpl_settings: injectable settings service [RQ-SET, ADR-002]
# Links juce_core (XmlDocument) privately for .NET-schema XML persistence;
# the public headers stay JUCE-free. Strict warnings are not applied because
# JUCE module sources compile into this target (same pattern as xpl_midi_juce).
add_library(xpl_settings STATIC
    src/LibraryInfo.cpp
    src/AllUsersSettingsDefaults.cpp
    src/XmlSettingsService.cpp)
target_include_directories(xpl_settings PUBLIC include)
target_compile_definitions(xpl_settings PRIVATE
    JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1
    JUCE_STANDALONE_APPLICATION=1
    JUCE_USE_CURL=0
    JUCE_WEB_BROWSER=0)
target_link_libraries(xpl_settings
    PUBLIC xpl_model
    PRIVATE juce::juce_core)
