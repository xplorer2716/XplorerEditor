# Xplorer JUCE application (GUI). Built only with -DXPL_BUILD_APP=ON. [RQ-GUI, ADR-JUC-006]

juce_add_gui_app(XplorerApp
    PRODUCT_NAME "Xplorer"
    COMPANY_NAME "Xplorer"
    # Numeric form only: this feeds FILEVERSION (four 16-bit fields) and
    # CFBundleVersion, neither of which can carry the "-preprod"/"-canary"
    # suffix. The full form reaches the About box and the Windows
    # ProductVersion string further down. [RQ-BLD-015, RQ-BLD-020]
    VERSION ${XPL_VERSION_NUMERIC}
    # The application icon: the executable's own icon, the main window's, the
    # macOS bundle's, and (via the same PNG) the Linux AppImage's desktop entry.
    # The application had NO icon at all before this -- neither ICON_BIG nor
    # ICON_SMALL was declared -- which is why it showed the platform's generic
    # one everywhere, dialogs included until RQ-GUI-070 gave those their own.
    #
    # Reused in place from the reference artwork rather than copied here or
    # redrawn: JUCE generates the Windows .ico and the macOS .icns from these
    # PNGs itself, so xdata/IconeXplorer/Xplorer.ico is NOT a build input and
    # stays what it always was, the archived original.
    #
    # ADR-GUI-001's "no binary asset" rule is not being broken: it governs
    # control-surface icons, painted every frame by juce::Path and required to
    # scale with the canvas. An application icon is OS chrome consumed by the
    # shell at fixed sizes -- the same category as about.jpg and the three
    # menu_*.png already kept verbatim below.
    # [RQ-BLD-026, ADR-BLD-004 (DEC-BLD-023), ADR-GUI-001 (referenced)]
    ICON_BIG "${CMAKE_CURRENT_SOURCE_DIR}/../../xdata/IconeXplorer/Hazard_256.png"
    ICON_SMALL "${CMAKE_CURRENT_SOURCE_DIR}/../../xdata/IconeXplorer/Hazard_32.png")

target_sources(XplorerApp PRIVATE
    src/Main.cpp
    src/DesignTokens.hpp
    src/MainComponent.cpp
    src/BackgroundRenderer.cpp
    src/ShortcutIcons.cpp
    src/DialogIcons.cpp
    src/BlockPalette.cpp
    src/BoundControls.cpp
    src/PageFamilyBlock.cpp
    src/ModMatrixComboBox.hpp
    src/ModMatrixPanel.cpp
    src/DisplayPanel.cpp
    src/VfdSegmentRenderer.cpp
    src/VfdDisplayHelper.cpp
    src/Dialogs.cpp
    src/SbomReader.cpp
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
    # than redrawn, same rule as the About image above.
    # (The 24 shortcut-button GIFs that used to follow were replaced by vector
    #  icons in TASK-GUI-002 — RQ-GUI-063, ADR-GUI-001.)
    # [RQ-GUI-008, ADR-JUC-032 (DEC-JUC-098)]
    assets/menu_new.png
    assets/menu_open.png
    assets/menu_save.png)

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

# The Software Bill of Materials the About > Dependencies window reads at run
# time. Shipped as a sibling of the executable rather than embedded, precisely
# so it can be replaced -- by CI, with GitHub's own dependency-graph SPDX
# export -- without rebuilding the application. The application never hard-codes
# a dependency name, version or licence; this file is the only source.
# The copy is deliberately NOT a build dependency of anything: a build whose
# SBOM went missing must still produce a working binary, which then reports the
# absence in its own UI. [RQ-BLD-014, RQ-GUI-057, ADR-ABT-001 (DEC-ABT-002, DEC-ABT-007)]
add_custom_command(TARGET XplorerApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/sbom/xplorer.sbom.spdx.json"
            "$<TARGET_FILE_DIR:XplorerApp>/xplorer.sbom.spdx.json"
    COMMENT "Copying xplorer.sbom.spdx.json (dependency disclosure) next to the executable")

target_include_directories(XplorerApp PRIVATE src)
target_compile_definitions(XplorerApp PRIVATE
    JUCE_USE_CURL=0
    JUCE_WEB_BROWSER=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:XplorerApp,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:XplorerApp,JUCE_VERSION>"
    # What the About box shows. JUCE_APPLICATION_VERSION_STRING above is the
    # NUMERIC form (it comes from the target's VERSION), so it cannot say which
    # stream a binary came from -- which is exactly what a bug report needs.
    # [RQ-BLD-016, RQ-BLD-020]
    XPL_VERSION_FULL_STRING="${XPL_VERSION_FULL}")

# --- Windows version resource [RQ-BLD-020, ADR-BLD-003 (DEC-BLD-014)] --------
# juceaide puts the NUMERIC version in both the FileVersion and the
# ProductVersion strings, so nothing in an executable's properties says which
# stream it came from. JUCE's own JUCE_USER_DEFINED_RC_FILE hook replaces its
# VERSIONINFO block -- and only that block; the icon statements sit outside the
# guard, so RQ-BLD-026's icon is unaffected.
if(WIN32)
    string(REPLACE "." "," XPL_VERSION_RC_COMMA "${XPL_VERSION_NUMERIC}")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/XplorerVersion.rc.in"
                   "${CMAKE_CURRENT_BINARY_DIR}/XplorerVersion.rc" @ONLY)
    # The value must expand to a QUOTED path: juceaide emits
    # `#include JUCE_USER_DEFINED_RC_FILE` verbatim.
    target_compile_definitions(XplorerApp PRIVATE
        JUCE_USER_DEFINED_RC_FILE="${CMAKE_CURRENT_BINARY_DIR}/XplorerVersion.rc")
endif()
target_link_libraries(XplorerApp PRIVATE
    XplorerAssets
    xpl_app_core
    xpl_midi_juce
    juce::juce_audio_utils
    juce::juce_gui_extra
    juce::juce_audio_devices)
