# xpl_midi: backend-agnostic MIDI abstraction [RQ-MID-040, ADR-004]
# xpl_midi_juce: JUCE adapter, kept separate so strict warnings never apply
# to JUCE module sources compiled into the target. [RQ-BLD-003, ADR-004]

add_library(xpl_midi STATIC
    src/LibraryInfo.cpp)
target_include_directories(xpl_midi PUBLIC include)
target_link_libraries(xpl_midi PRIVATE xpl::warnings)

add_library(xpl_midi_juce STATIC
    src/juce/JuceBackendInfo.cpp)
target_compile_definitions(xpl_midi_juce PRIVATE
    JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1
    JUCE_STANDALONE_APPLICATION=1
    JUCE_USE_CURL=0
    JUCE_WEB_BROWSER=0)
target_link_libraries(xpl_midi_juce
    PUBLIC xpl_midi
    PRIVATE juce::juce_audio_devices)
