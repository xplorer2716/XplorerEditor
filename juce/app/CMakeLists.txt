# Xplorer JUCE application (GUI). Built only with -DXPL_BUILD_APP=ON. [RQ-GUI, ADR-JUC-006]

juce_add_gui_app(XplorerApp
    PRODUCT_NAME "Xplorer"
    COMPANY_NAME "Xplorer"
    VERSION 0.1.0)

target_sources(XplorerApp PRIVATE
    src/Main.cpp
    src/DesignTokens.hpp
    src/MainComponent.cpp
    src/BackgroundRenderer.cpp
    src/BlockPalette.cpp
    src/BoundControls.cpp
    src/PageFamilyBlock.cpp
    src/ModMatrixComboBox.hpp
    src/ModMatrixPanel.cpp
    src/DisplayPanel.cpp
    src/VfdSegmentRenderer.cpp
    src/VfdDisplayHelper.cpp
    src/Dialogs.cpp
    src/SettingsDialog.cpp
    src/ProgressWindow.cpp
    src/PianoWindow.cpp
    src/XplorerLookAndFeel.cpp
    src/JuceEventDispatcher.cpp)

# UI assets (button images) embedded as binary data. The main background and
# the VFD glyphs are both drawn vectorially now, so neither is embedded.
#
# assets/vfd-matrix.png is deliberately NOT here while remaining in the
# repository: it stopped being a runtime asset when ADR-JUC-023 replaced the
# sprite blit, but it is the reference of record — the .NET tree that produced
# it has left for XplorerEditor-dotnet-archive, so it is the only surviving
# witness of the original artwork, and juce/tools/fit_vfd_tokens.py fits the
# design tokens against it. [ADR-JUC-013, ADR-JUC-023]
juce_add_binary_data(XplorerAssets SOURCES
    # Combo-box typeface: embedded so label widths are a property of the build,
    # not of the host's default sans-serif. Licence text ships beside it.
    # [RQ-DSN-096, ADR-JUC-022 (DEC-JUC-048)]
    assets/RobotoCondensed-Regular.ttf
    assets/about.jpg
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
