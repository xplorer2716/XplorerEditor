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
    # The reference's THREE menu icons -- New, Open, Save -- decoded from the
    # base64 System.Drawing.Bitmap resources in MainForm.resx. No other menu
    # item in the whole reference application has one. Reused verbatim rather
    # than redrawn, same rule as the shortcut-button GIFs below and the About
    # image above. [RQ-GUI-008, ADR-JUC-032 (DEC-JUC-098)]
    assets/menu_new.png
    assets/menu_open.png
    assets/menu_save.png
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

# The default patch "New" loads. Copied beside the executable rather than
# embedded in XplorerAssets: the controller's load path takes a filesystem
# path and opens it with std::ifstream (XpanderToneIO.cpp) -- there is no
# in-memory overload -- so embedding it would mean writing it back out to a
# temp file on every New. The reference ships it loosely next to its own
# executable and resolves it the same way. [RQ-GUI-008, ADR-JUC-032 (DEC-JUC-100)]
add_custom_command(TARGET XplorerApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/../oberheim.syx"
            "$<TARGET_FILE_DIR:XplorerApp>/oberheim.syx"
    COMMENT "Copying oberheim.syx (default patch for File > New) next to the executable")

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
