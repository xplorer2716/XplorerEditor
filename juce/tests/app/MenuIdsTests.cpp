#include <catch2/catch_test_macros.hpp>

// Menu identity invariants. These are the checks no reader performs by
// inspection: that 22 hand-assigned ids collide with none of their peers, and
// that the View menu's COMPUTED id range has not grown over a fixed id.
// Headless by construction — MenuIds.hpp pulls in no UI framework, so this
// suite runs in the default XPL_BUILD_APP=OFF build.
// [RQ-QLT-002, RQ-QLT-003, RQ-QLT-010, ADR-QLT-001 (DEC-QLT-001, DEC-QLT-004)]

#include "xplorer/app/ControlTable.hpp"
#include "xplorer/app/MenuIds.hpp"

#include <algorithm>
#include <vector>

using namespace xplorer::app;

SCENARIO("Menu item ids are pairwise distinct", "[RQ-QLT-002]")
{
    GIVEN("every declared menu item")
    {
        std::vector<int> ids;
        ids.reserve(ALL_MENU_ITEMS.size());
        for (const auto item : ALL_MENU_ITEMS)
        {
            ids.push_back(menuItemId(item));
        }

        WHEN("the ids are sorted")
        {
            std::sort(ids.begin(), ids.end());

            THEN("no two items share an id")
            {
                // Dispatch is by id, so a collision routes one item's
                // selection to another's action.
                CHECK(std::adjacent_find(ids.begin(), ids.end()) == ids.end());
            }
        }
    }
}

SCENARIO("No fixed menu id falls inside the View scale preset range", "[RQ-QLT-002]")
{
    GIVEN("the View menu's computed scale id range")
    {
        const int first = VIEW_SCALE_FIRST_ID;
        const int end = viewScaleIdEnd();

        THEN("the range covers exactly one id per preset")
        {
            CHECK(end - first == static_cast<int>(WINDOW_SCALE_PRESETS.size()));
        }

        AND_THEN("no fixed item id lies within it")
        {
            // The range grows with WINDOW_SCALE_PRESETS. This is the check
            // that turns "adding a sixth preset silently misroutes a menu
            // action" into a test failure.
            for (const auto item : ALL_MENU_ITEMS)
            {
                const int id = menuItemId(item);
                INFO("menu item id: " << id);
                CHECK_FALSE((id >= first && id < end));
            }
        }

        AND_THEN("the full-screen id lies outside it too")
        {
            CHECK_FALSE((VIEW_FULL_SCREEN_ID >= first && VIEW_FULL_SCREEN_ID < end));
        }
    }
}

SCENARIO("Top-level menu order is declared once", "[RQ-QLT-003]")
{
    GIVEN("the top-level menu declaration")
    {
        THEN("the enumerator values are the MenuBarModel indices, in order")
        {
            // getMenuBarNames() and getMenuForIndex() are coupled by position
            // only; this is what keeps that coupling honest.
            for (std::size_t i = 0; i < TOP_LEVEL_MENUS.size(); ++i)
            {
                INFO("top-level menu slot: " << i);
                CHECK(topLevelMenuIndex(TOP_LEVEL_MENUS[i]) == static_cast<int>(i));
            }
        }

        AND_THEN("the declared menus are the five reference ones")
        {
            REQUIRE(TOP_LEVEL_MENUS.size() == 5);
            CHECK(TOP_LEVEL_MENUS[0] == TopLevelMenu::File);
            CHECK(TOP_LEVEL_MENUS[1] == TopLevelMenu::Patch);
            CHECK(TOP_LEVEL_MENUS[2] == TopLevelMenu::View);
            CHECK(TOP_LEVEL_MENUS[3] == TopLevelMenu::Tools);
            CHECK(TOP_LEVEL_MENUS[4] == TopLevelMenu::Help);
        }
    }
}

SCENARIO("Menu item ids keep their reference values", "[RQ-QLT-001]")
{
    GIVEN("the ids the reference assigned")
    {
        THEN("each enumerator carries the value it had before extraction")
        {
            // DEC-QLT-002: a declaration change, not a renumbering. Spot-check
            // one id per menu plus both submenu bounds, so an accidental
            // renumber during a later edit fails here rather than in the GUI.
            CHECK(menuItemId(MenuItem::FileNew) == 1);
            CHECK(menuItemId(MenuItem::FileExit) == 4);
            CHECK(menuItemId(MenuItem::PatchNext) == 10);
            CHECK(menuItemId(MenuItem::PatchSynchronize) == 16);
            CHECK(menuItemId(MenuItem::ToolsSettings) == 20);
            CHECK(menuItemId(MenuItem::ToolsPianoKeyboard) == 22);
            CHECK(menuItemId(MenuItem::HelpAbout) == 30);
            CHECK(menuItemId(MenuItem::HelpWebsite) == 33);
            CHECK(menuItemId(MenuItem::ToolsGetAllSinglePatches) == 40);
            CHECK(menuItemId(MenuItem::ToolsRestoreAllData) == 43);
        }
    }
}
