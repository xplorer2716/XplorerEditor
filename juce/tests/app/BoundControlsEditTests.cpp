#include <catch2/catch_test_macros.hpp>

#include "BoundControls.hpp"

#include "xplorer/app/ControlMetadata.hpp"
#include "xplorer/app/ParameterBindingRegistry.hpp"

#include "common/midi/MockMidiBackend.hpp"

#include <algorithm>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

// A bound control reports a user edit to the registry under ITS OWN parameter
// name, and the registry writes it to the controller. The four widgets reach
// the registry through BoundControl::registry() (TASK-QLT-014: the base no
// longer exposes its registry and name as protected data members), so each
// edit path is driven here through the widget's own callback -- the one JUCE
// invokes on a user gesture -- rather than through a simulated mouse.
// [RQ-GUI-030, RQ-GUI-032, RQ-GUI-004, RQ-QLT-017]

using namespace xplorer;
using namespace common::midi;

namespace
{
    constexpr auto KNOB_PARAMETER = "VCF_FREQ";
    constexpr auto COMBO_PARAMETER = "VCF_MODE";
    constexpr auto CHECK_PARAMETER = "VCO1_MOD_KEYB";
    constexpr auto RADIO_PARAMETER = "FM_DESTINATION";
    constexpr int AUTOMATION_CC = 74;
    constexpr int SECOND_ROW = 1;
    constexpr std::size_t MIN_ROW_COUNT = 2;

    struct Fixture
    {
        Fixture()
        {
            registry.setLocalEditHandler([this](const std::string& name) { editedNames.push_back(name); });
        }

        [[nodiscard]] int valueOf(const std::string& name)
        {
            const auto* parameter = controller.getParameter(name);
            REQUIRE(parameter != nullptr);
            return parameter->value();
        }

        MockMidiBackend backend;
        settings::InMemorySettingsService settingsService;
        controller::XpanderController controller{backend, settingsService, nullptr, "TEST"};
        app::ParameterBindingRegistry registry{controller};
        std::vector<std::string> editedNames;
    };
}

SCENARIO("A bound knob reports its edits under its own parameter name", "[RQ-GUI-030][RQ-GUI-004][RQ-QLT-017]")
{
    GIVEN("a knob bound to the VCF frequency parameter")
    {
        Fixture f;
        const auto* parameter = f.controller.getParameter(KNOB_PARAMETER);
        REQUIRE(parameter != nullptr);
        const int maximum = parameter->maxValue();
        REQUIRE(f.valueOf(KNOB_PARAMETER) != maximum);
        app::BoundKnob knob(f.registry, KNOB_PARAMETER, parameter->minValue(), maximum, parameter->step());

        WHEN("the user turns it to its maximum")
        {
            knob.setValue(maximum, juce::dontSendNotification);
            knob.onValueChange();

            THEN("the controller holds that value and the edit is reported under the parameter name")
            {
                CHECK(f.valueOf(KNOB_PARAMETER) == maximum);
                CHECK(f.editedNames == std::vector<std::string>{KNOB_PARAMETER});
            }
        }

        WHEN("a drag starts, then ends, on a parameter automated by a CC")
        {
            f.controller.controlChangeAutomationTable().add(KNOB_PARAMETER, AUTOMATION_CC);
            knob.onDragStart();
            const int heldDuringDrag = f.controller.disabledControlChangeNumber();
            knob.onDragEnd();

            THEN("that CC is held off for the drag only")
            {
                CHECK(heldDuringDrag == AUTOMATION_CC);
                CHECK(f.controller.disabledControlChangeNumber()
                      == controller::XpanderController::NO_CONTROL_CHANGE);
            }
        }
    }
}

SCENARIO("A bound combo box reports the value of the row the user selects",
         "[RQ-GUI-030][RQ-GUI-004][RQ-QLT-017]")
{
    GIVEN("a combo box bound to the VCF mode parameter, one row per label")
    {
        Fixture f;
        const auto labels = app::comboLabelsForControl(COMBO_PARAMETER);
        REQUIRE(labels.size() >= MIN_ROW_COUNT);
        std::vector<std::pair<std::string, int>> options;
        for (std::size_t i = 0; i < labels.size(); ++i)
        {
            options.emplace_back(labels[i], static_cast<int>(i));
        }
        REQUIRE(f.valueOf(COMBO_PARAMETER) != SECOND_ROW);
        app::BoundComboBox combo(f.registry, COMBO_PARAMETER, options);

        WHEN("the user selects the second row")
        {
            combo.setSelectedItemIndex(SECOND_ROW, juce::dontSendNotification);
            combo.onChange();

            THEN("the controller holds that row's value and the edit is reported under the parameter name")
            {
                CHECK(f.valueOf(COMBO_PARAMETER) == SECOND_ROW);
                CHECK(f.editedNames == std::vector<std::string>{COMBO_PARAMETER});
            }
        }
    }
}

SCENARIO("A bound check box reports its toggle state as 0 or 1", "[RQ-GUI-030][RQ-GUI-032][RQ-QLT-017]")
{
    GIVEN("a check box bound to the VCO1 keyboard-modulation flag, currently off")
    {
        Fixture f;
        REQUIRE(f.valueOf(CHECK_PARAMETER) == 0);
        app::BoundCheckBox check(f.registry, CHECK_PARAMETER, "KEYB");

        WHEN("the user ticks it")
        {
            check.setToggleState(true, juce::dontSendNotification);
            check.onClick();

            THEN("the controller holds 1 and the edit is reported under the parameter name")
            {
                CHECK(f.valueOf(CHECK_PARAMETER) == 1);
                CHECK(f.editedNames == std::vector<std::string>{CHECK_PARAMETER});
            }
        }
    }
}

SCENARIO("A bound radio group reports the value of the option the user picks",
         "[RQ-GUI-030][RQ-GUI-038][RQ-QLT-017]")
{
    GIVEN("a radio group bound to the FM destination parameter")
    {
        Fixture f;
        const auto options = app::radioPanelOptions(RADIO_PARAMETER);
        REQUIRE(options.size() >= MIN_ROW_COUNT);
        const int current = f.valueOf(RADIO_PARAMETER);
        const auto other = std::find_if(options.begin(), options.end(),
                                        [current](const auto& option) { return option.second != current; });
        REQUIRE(other != options.end());
        app::BoundRadioGroup group(f.registry, RADIO_PARAMETER, options);

        WHEN("the user picks the option that is not selected")
        {
            const auto index = static_cast<int>(std::distance(options.begin(), other));
            auto* button = dynamic_cast<juce::ToggleButton*>(group.getChildComponent(index));
            REQUIRE(button != nullptr);
            button->setToggleState(true, juce::dontSendNotification);
            button->onClick();

            THEN("the controller holds that option's value and the edit is reported under the parameter name")
            {
                CHECK(f.valueOf(RADIO_PARAMETER) == other->second);
                CHECK(f.editedNames == std::vector<std::string>{RADIO_PARAMETER});
            }
        }
    }
}
