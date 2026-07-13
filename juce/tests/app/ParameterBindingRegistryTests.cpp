#include <catch2/catch_test_macros.hpp>

#include "xplorer/app/ParameterBindingRegistry.hpp"
#include "xpl/midi/MockMidiBackend.hpp"

#include <chrono>
#include <string>
#include <thread>
#include <vector>

using namespace xplorer;
using namespace xpl::midi;

namespace
{
    /// Fake control recording model→control pushes and able to simulate a
    /// user gesture back into the registry.
    class FakeControl final : public app::IBoundControl
    {
    public:
        FakeControl(app::ParameterBindingRegistry& registry, std::string parameterName)
            : _registry(registry), _parameterName(std::move(parameterName))
        {
        }

        void setDisplayedValue(int value) override
        {
            _displayed = value;
            _current = value;
            // A real JUCE control fires its change callback here; reproduce it
            // so the anti-echo guard is exercised.
            _registry.onControlEdited(_parameterName, value);
        }

        [[nodiscard]] std::string displayText() const override { return std::to_string(_current); }

        void simulateUserEdit(int value)
        {
            _current = value;
            _registry.onControlEditBegan(_parameterName);
            _registry.onControlEdited(_parameterName, value);
            _registry.onControlEditEnded();
        }

        [[nodiscard]] int displayed() const { return _displayed; }

    private:
        app::ParameterBindingRegistry& _registry;
        std::string _parameterName;
        int _displayed = -1;
        int _current = 0;
    };

    struct Fixture
    {
        Fixture()
        {
            backend.addOutputDevice("Synth Out");
            controller.setParameterTransmitDelay(1);
            REQUIRE(controller.setSynthOutputDevice("Synth Out"));
        }

        MockMidiBackend backend;
        settings::InMemorySettingsService settingsService;
        controller::XpanderController controller{backend, settingsService, nullptr, "TEST"};
        app::ParameterBindingRegistry registry{controller};
    };
}

SCENARIO("Binding rejects unknown parameters", "[RQ-GUI-002]")
{
    GIVEN("a registry over a real controller")
    {
        Fixture f;
        FakeControl good(f.registry, "VCO1_FREQ");
        FakeControl bad(f.registry, "NOT_A_PARAM");

        THEN("only known parameters bind")
        {
            CHECK(f.registry.bind("VCO1_FREQ", good));
            CHECK_FALSE(f.registry.bind("NOT_A_PARAM", bad));
            CHECK(f.registry.bindingCount() == 1);
        }
    }
}

SCENARIO("A user edit updates the model and is not echoed back", "[RQ-GUI-003]")
{
    GIVEN("a bound knob")
    {
        Fixture f;
        FakeControl knob(f.registry, "VCF_FREQ");
        REQUIRE(f.registry.bind("VCF_FREQ", knob));

        WHEN("the user edits it")
        {
            knob.simulateUserEdit(42);

            THEN("the controller parameter takes the value")
            {
                CHECK(f.controller.getParameter("VCF_FREQ")->value() == 42);
            }
        }
    }
}

SCENARIO("Local edits are fanned out to the display, model refreshes are not", "[RQ-GUI-020]")
{
    GIVEN("a bound knob and a wired local-edit handler")
    {
        Fixture f;
        FakeControl knob(f.registry, "VCF_FREQ");
        REQUIRE(f.registry.bind("VCF_FREQ", knob));

        std::vector<std::string> shown;
        f.registry.setLocalEditHandler([&](const std::string& name)
                                       { shown.push_back(name + "=" + f.registry.displayTextFor(name)); });

        WHEN("the user edits the control")
        {
            knob.simulateUserEdit(42);

            THEN("the handler receives the parameter and its formatted value (for the VFD)")
            {
                REQUIRE(shown.size() == 1);
                CHECK(shown.front() == "VCF_FREQ=42");
            }
        }

        WHEN("the value arrives from a model refresh instead")
        {
            f.controller.setParameter("VCF_FREQ", 7);
            f.registry.refreshAllFromModel(); // re-enters onControlEdited via the fake

            THEN("the handler is not called (anti-echo)")
            {
                CHECK(shown.empty());
            }
        }
    }
}

SCENARIO("A model change refreshes the control without re-sending to the synth", "[RQ-GUI-003]")
{
    GIVEN("a bound control and a started controller")
    {
        Fixture f;
        FakeControl knob(f.registry, "VCF_RES");
        REQUIRE(f.registry.bind("VCF_RES", knob));
        f.controller.start();
        f.backend.clearSentMessages();

        WHEN("the controller reports a parameter change (e.g. from the synth)")
        {
            f.registry.onParameterChanged("VCF_RES", 55);

            THEN("the control shows the value and no MIDI is emitted for it")
            {
                CHECK(knob.displayed() == 55);
                // setDisplayedValue re-enters onControlEdited, but the guard
                // must have blocked the setParameter → nothing sent.
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                CHECK(f.backend.sentMessages("Synth Out").empty());
                // and the model value was not changed by the refresh path
                CHECK(f.controller.getParameter("VCF_RES")->value() == 0);
            }
        }
    }
}

SCENARIO("Interactive edit disables and restores the mapped CC automation", "[RQ-GUI-004]")
{
    GIVEN("a parameter mapped to a CC")
    {
        Fixture f;
        f.controller.controlChangeAutomationTable().add("VCF_FREQ", 74);
        FakeControl knob(f.registry, "VCF_FREQ");
        REQUIRE(f.registry.bind("VCF_FREQ", knob));

        WHEN("the edit begins")
        {
            f.registry.onControlEditBegan("VCF_FREQ");

            THEN("that CC is the disabled one")
            {
                CHECK(f.controller.disabledControlChangeNumber() == 74);
            }
        }

        WHEN("the edit ends")
        {
            f.registry.onControlEditBegan("VCF_FREQ");
            f.registry.onControlEditEnded();

            THEN("no CC is disabled anymore")
            {
                CHECK(f.controller.disabledControlChangeNumber()
                      == controller::XpanderController::NO_CONTROL_CHANGE);
            }
        }

        WHEN("the parameter has no mapped CC")
        {
            f.registry.onControlEditBegan("VCF_RES"); // unmapped

            THEN("nothing is disabled")
            {
                CHECK(f.controller.disabledControlChangeNumber()
                      == controller::XpanderController::NO_CONTROL_CHANGE);
            }
        }
    }
}

SCENARIO("Full-tone refresh pushes every bound value from the model", "[RQ-GUI-003]")
{
    GIVEN("several bound controls and a loaded model")
    {
        Fixture f;
        FakeControl a(f.registry, "VCO1_FREQ");
        FakeControl b(f.registry, "ENV_1_ATTACK");
        REQUIRE(f.registry.bind("VCO1_FREQ", a));
        REQUIRE(f.registry.bind("ENV_1_ATTACK", b));
        f.controller.setParameter("VCO1_FREQ", 24);
        f.controller.setParameter("ENV_1_ATTACK", 40);

        WHEN("a full refresh is requested")
        {
            f.registry.refreshAllFromModel();

            THEN("all controls mirror the model")
            {
                CHECK(a.displayed() == 24);
                CHECK(b.displayed() == 40);
            }
        }
    }
}

SCENARIO("Rebinding a name replaces the previous control", "[RQ-GUI-010]")
{
    GIVEN("a page-family control rebound to another instance")
    {
        Fixture f;
        FakeControl first(f.registry, "ENV_1_ATTACK");
        FakeControl second(f.registry, "ENV_2_ATTACK");
        REQUIRE(f.registry.bind("ENV_1_ATTACK", first));

        WHEN("the same logical slot is rebound to ENV_2_ATTACK")
        {
            f.registry.unbind("ENV_1_ATTACK");
            REQUIRE(f.registry.bind("ENV_2_ATTACK", second));
            f.controller.setParameter("ENV_2_ATTACK", 12);
            f.registry.refreshAllFromModel();

            THEN("only the new binding is refreshed")
            {
                CHECK(second.displayed() == 12);
                CHECK(first.displayed() == -1);
                CHECK(f.registry.bindingCount() == 1);
            }
        }
    }
}
