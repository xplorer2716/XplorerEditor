# Xplorer JUCE application (GUI). Built only with -DXPL_BUILD_APP=ON. [RQ-GUI, ADR-006]

juce_add_gui_app(XplorerApp
    PRODUCT_NAME "Xplorer"
    COMPANY_NAME "Xplorer"
    VERSION 0.1.0)

target_sources(XplorerApp PRIVATE
    src/Main.cpp
    src/MainComponent.cpp
    src/BoundControls.cpp
    src/PageFamilyBlock.cpp
    src/ModMatrixPanel.cpp
    src/DisplayPanel.cpp
    src/Dialogs.cpp
    src/PianoWindow.cpp
    src/XplorerLookAndFeel.cpp
    src/JuceEventDispatcher.cpp)

# UI assets (background, button images) embedded as binary data
juce_add_binary_data(XplorerAssets SOURCES
    assets/main-background.jpg
    assets/gotodown.gif
    assets/gotohover.gif
    assets/gotopatch.gif
    assets/load.gif
    assets/loaddown.gif
    assets/loadhover.gif
    assets/minus.gif
    assets/minusdown.gif
    assets/minushover.gif
    assets/plus.gif
    assets/plusdown.gif
    assets/plushover.gif
    assets/random.gif
    assets/randomdown.gif
    assets/randomhover.gif
    assets/save.gif
    assets/savedown.gif
    assets/savehover.gif
    assets/settings.gif
    assets/settingsdown.gif
    assets/settingshover.gif
    assets/store.gif
    assets/storedown.gif
    assets/storehover.gif)

target_include_directories(XplorerApp PRIVATE src)
target_compile_definitions(XplorerApp PRIVATE
    JUCE_USE_CURL=0
    JUCE_WEB_BROWSER=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:XplorerApp,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:XplorerApp,JUCE_VERSION>")
target_link_libraries(XplorerApp PRIVATE
    XplorerAssets
    xpl_app_core
    xpl_midi_juce
    juce::juce_audio_utils
    juce::juce_gui_extra
    juce::juce_audio_devices)
