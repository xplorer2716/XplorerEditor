#pragma once

// Identity of the main menu bar: the top-level menu order and every menu
// item's id, declared once for the three sites that would otherwise repeat the
// same raw integers — menu construction, keyboard-shortcut binding and
// selection dispatch.
//
// UI-framework-free on purpose: MainComponent.cpp lives in the GUI target,
// which is off by default (XPL_BUILD_APP) and covered by no test, so an id
// declared there would gain a name but no verification. Declared here, the
// invariants below are checked in the headless suite.
// [RQ-QLT-001, RQ-QLT-002, RQ-QLT-003, RQ-QLT-010,
//  ADR-QLT-001 (DEC-QLT-001, DEC-QLT-002, DEC-QLT-004)]

#include "xplorer/app/ControlTable.hpp"
#include "xpl/util/EnumUtils.hpp"

#include <array>
#include <cstddef>

namespace xplorer::app
{
    /// Top-level menus, in reference display order. The enumerator values ARE
    /// the MenuBarModel indices, so the name list and the per-index builder
    /// cannot drift apart. [RQ-QLT-003, DEC-QLT-004]
    enum class TopLevelMenu : int
    {
        File = 0,
        Patch = 1,
        View = 2,
        Tools = 3,
        Help = 4,
    };

    /// Top-level menus in display order — the single declaration
    /// getMenuBarNames() and getMenuForIndex() both derive from.
    inline constexpr std::array TOP_LEVEL_MENUS{
        TopLevelMenu::File, TopLevelMenu::Patch, TopLevelMenu::View,
        TopLevelMenu::Tools, TopLevelMenu::Help};

    /// Menu item ids. The values are the reference's own and are carried over
    /// unchanged — this is a declaration change, not a renumbering, so the
    /// parity recorded in ADR-JUC-032 is untouched. The gaps in the sequence
    /// are the reference's too. [RQ-QLT-001, DEC-QLT-002]
    enum class MenuItem : int
    {
        // File
        FileNew = 1,
        FileOpen = 2,
        FileSave = 3,
        FileExit = 4,

        // Patch
        PatchNext = 10,
        PatchPrevious = 11,
        PatchGoto = 12,
        PatchStore = 13,
        PatchRename = 14,
        PatchRandomize = 15,
        PatchSynchronize = 16,

        // Tools
        ToolsSettings = 20,
        ToolsTuneRequest = 21,
        ToolsPianoKeyboard = 22,
        ToolsTestDisplay = 23, // [RQ-GUI-082]

        // Help
        HelpAbout = 30,
        HelpUserManual = 31,
        HelpReleases = 32,
        HelpWebsite = 33,

        // Tools submenus
        ToolsGetAllSinglePatches = 40,
        ToolsExtractSinglePatches = 41,
        ToolsBackupAllData = 42,
        ToolsRestoreAllData = 43,
    };

    /// Every declared menu item, for the uniqueness check that no reader can
    /// perform by inspection. Order is declaration order. [RQ-QLT-002]
    inline constexpr std::array ALL_MENU_ITEMS{
        MenuItem::FileNew,
        MenuItem::FileOpen,
        MenuItem::FileSave,
        MenuItem::FileExit,
        MenuItem::PatchNext,
        MenuItem::PatchPrevious,
        MenuItem::PatchGoto,
        MenuItem::PatchStore,
        MenuItem::PatchRename,
        MenuItem::PatchRandomize,
        MenuItem::PatchSynchronize,
        MenuItem::ToolsSettings,
        MenuItem::ToolsTuneRequest,
        MenuItem::ToolsPianoKeyboard,
        MenuItem::ToolsTestDisplay,
        MenuItem::HelpAbout,
        MenuItem::HelpUserManual,
        MenuItem::HelpReleases,
        MenuItem::HelpWebsite,
        MenuItem::ToolsGetAllSinglePatches,
        MenuItem::ToolsExtractSinglePatches,
        MenuItem::ToolsBackupAllData,
        MenuItem::ToolsRestoreAllData};

    /// The View menu's scale presets occupy a CONTIGUOUS range starting here,
    /// one id per entry of WINDOW_SCALE_PRESETS, because menuItemSelected
    /// dispatches them by offset rather than by individual case. Kept clear of
    /// every id above so "View" can be inserted mid-bar without disturbing
    /// File/Patch/Tools/Help. [RQ-SCL-002, DEC-JUC-065]
    inline constexpr int VIEW_SCALE_FIRST_ID = 50;
    inline constexpr int VIEW_FULL_SCREEN_ID = 60;

    /// One past the last scale-preset id. Adding a preset widens this range,
    /// which is why RQ-QLT-002 checks it against the fixed ids rather than
    /// trusting the 50/60 gap to stay wide enough.
    [[nodiscard]] inline constexpr int viewScaleIdEnd() noexcept
    {
        return VIEW_SCALE_FIRST_ID + static_cast<int>(WINDOW_SCALE_PRESETS.size());
    }

    /// The id JUCE is handed for a menu item, at the one boundary where the
    /// typed enumerator meets PopupMenu's plain `int`.
    [[nodiscard]] inline constexpr int menuItemId(MenuItem item) noexcept
    {
        return xpl::util::toUnderlying(item);
    }

    /// The MenuBarModel index of a top-level menu.
    [[nodiscard]] inline constexpr int topLevelMenuIndex(TopLevelMenu menu) noexcept
    {
        return xpl::util::toUnderlying(menu);
    }
}
