#include <catch2/catch_test_macros.hpp>

#include "NumericEntryKnob.hpp"

#include "xplorer/app/KnobPresetValues.hpp"

#include <juce_gui_basics/juce_gui_basics.h>

#include <cstddef>

// RQ-GUI-034's double-click -> inline numeric entry, now shared by BoundKnob
// and the modulation-matrix amount knob instead of living only on the
// former. Exercised through openNumericEntry() (the mouseDoubleClick body,
// factored out) rather than a synthesized juce::MouseEvent: constructing one
// needs a real MouseInputSource, which needs Desktop::getInstance(), which
// on Linux touches X11 — unavailable on the headless CI runner this target
// is built for (no Xvfb wrapper around ctest, see .github/actions/build-app).
// [RQ-GUI-034, RQ-GUI-015, PLAN-GUI-009 (TASK-GUI-036)]

using namespace xplorer::app;

namespace
{
    juce::TextEditor* findEditor(const juce::Component& parent)
    {
        for (int i = 0; i < parent.getNumChildComponents(); ++i)
        {
            if (auto* editor = dynamic_cast<juce::TextEditor*>(parent.getChildComponent(i)))
            {
                return editor;
            }
        }
        return nullptr;
    }
}

SCENARIO("A numeric entry knob opens an inline editor on double-click", "[RQ-GUI-034]")
{
    GIVEN("a knob with a parent and a current value")
    {
        const juce::ScopedJuceInitialiser_GUI juceInit;

        juce::Component parent;
        NumericEntryKnob knob{juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox};
        knob.setRange(-63, 63, 1);
        knob.setValue(17, juce::dontSendNotification);
        knob.setBounds(10, 20, 40, 40);
        parent.addAndMakeVisible(knob);

        REQUIRE(findEditor(parent) == nullptr);

        WHEN("the double-click action fires")
        {
            knob.openNumericEntry();

            THEN("an inline text editor opens over the knob's bounds, pre-filled with its value")
            {
                auto* editor = findEditor(parent);
                REQUIRE(editor != nullptr);
                REQUIRE(editor->getText() == "17");
                REQUIRE(editor->getBounds() == knob.getBounds());
            }

            THEN("a second double-click does not open a duplicate editor")
            {
                knob.openNumericEntry();
                int editorCount = 0;
                for (int i = 0; i < parent.getNumChildComponents(); ++i)
                {
                    if (dynamic_cast<juce::TextEditor*>(parent.getChildComponent(i)) != nullptr)
                    {
                        ++editorCount;
                    }
                }
                REQUIRE(editorCount == 1);
            }
        }
    }

    GIVEN("a knob with no parent component")
    {
        const juce::ScopedJuceInitialiser_GUI juceInit;

        NumericEntryKnob knob{juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox};
        knob.setRange(-63, 63, 1);
        knob.setValue(5, juce::dontSendNotification);

        THEN("the double-click action is a harmless no-op")
        {
            knob.openNumericEntry();
            REQUIRE(knob.getValue() == 5.0);
        }
    }
}

SCENARIO("Committing the inline editor sets the knob's value", "[RQ-GUI-034]")
{
    GIVEN("an open inline editor")
    {
        const juce::ScopedJuceInitialiser_GUI juceInit;

        juce::Component parent;
        NumericEntryKnob knob{juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox};
        knob.setRange(-63, 63, 1);
        knob.setValue(10, juce::dontSendNotification);
        parent.addAndMakeVisible(knob);
        knob.openNumericEntry();
        auto* editor = findEditor(parent);
        REQUIRE(editor != nullptr);

        WHEN("a new value is typed and Return is pressed")
        {
            editor->setText("-42", false);
            editor->onReturnKey();

            THEN("the knob's value is set to it")
            {
                REQUIRE(knob.getValue() == -42.0);
            }
        }

        WHEN("Escape is pressed without changing the text")
        {
            editor->onEscapeKey();

            THEN("the knob's value is unchanged")
            {
                REQUIRE(knob.getValue() == 10.0);
            }
        }
    }
}

// RQ-GUI-079's other half: the knob-side behaviour of the preset keys. The
// value TABLE is pinned headlessly in KnobPresetValuesTests; what can only be
// checked against a real juce::Slider is that the presets are derived from the
// LIVE range (set after construction) and that applying one moves the knob.
// [RQ-GUI-079, ADR-JUC-037 (DEC-JUC-128, DEC-JUC-130)]

SCENARIO("Preset values are derived from the knob's range, not its construction",
         "[RQ-GUI-079]")
{
    GIVEN("a knob whose range is set after construction, as the matrix amount knob's is")
    {
        const juce::ScopedJuceInitialiser_GUI juceInit;

        NumericEntryKnob knob{juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox};
        knob.setRange(-63, 63, 1);

        WHEN("its presets are asked for")
        {
            const auto& values = knob.presetValues();

            THEN("they span the range that was set, not the default one")
            {
                REQUIRE(values.size() == static_cast<std::size_t>(BIPOLAR_PRESET_COUNT));
                CHECK(values.front() == -63);
                CHECK(values.back() == 63);
            }
        }

        WHEN("the eleventh preset is applied")
        {
            const bool applied = knob.applyPresetValue(BIPOLAR_PRESET_COUNT - 1);

            THEN("the knob moves to its maximum")
            {
                CHECK(applied);
                CHECK(knob.getValue() == 63.0);
            }
        }

        WHEN("the middle preset is applied")
        {
            const bool applied = knob.applyPresetValue(BIPOLAR_PRESET_COUNT / 2);

            THEN("the knob lands exactly on zero")
            {
                CHECK(applied);
                CHECK(knob.getValue() == 0.0);
            }
        }
    }
}

SCENARIO("The eleventh preset key is inert on a knob that has only ten", "[RQ-GUI-079]")
{
    GIVEN("a unipolar 0..63 knob, as every volume and PW knob is")
    {
        const juce::ScopedJuceInitialiser_GUI juceInit;

        NumericEntryKnob knob{juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox};
        knob.setRange(0, 63, 1);
        knob.setValue(17, juce::dontSendNotification);

        WHEN("the tenth preset is applied")
        {
            const bool applied = knob.applyPresetValue(UNIPOLAR_PRESET_COUNT - 1);

            THEN("the knob moves to its maximum")
            {
                CHECK(applied);
                CHECK(knob.getValue() == 63.0);
            }
        }

        WHEN("the eleventh preset is applied")
        {
            knob.setValue(17, juce::dontSendNotification);
            const bool applied = knob.applyPresetValue(BIPOLAR_PRESET_COUNT - 1);

            THEN("nothing happens and the caller is told so")
            {
                CHECK_FALSE(applied);
                CHECK(knob.getValue() == 17.0);
            }
        }
    }
}

SCENARIO("An overridden preset table replaces the range-derived one", "[RQ-GUI-079]")
{
    GIVEN("a VCO frequency knob carrying the harmonic semitone presets")
    {
        const juce::ScopedJuceInitialiser_GUI juceInit;

        NumericEntryKnob knob{juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox};
        knob.setRange(0, 63, 1);
        const auto* harmonics = harmonicPresetValuesFor("VCO1_FREQ");
        REQUIRE(harmonics != nullptr);
        knob.setPresetValues(*harmonics);

        WHEN("the fourth preset is applied")
        {
            const bool applied = knob.applyPresetValue(3);

            THEN("the knob lands on the octave, not on the linear quarter of its range")
            {
                CHECK(applied);
                CHECK(knob.getValue() == 12.0);
            }
        }

        WHEN("its presets are asked for")
        {
            const auto& values = knob.presetValues();

            THEN("the override stands, and no range derivation overwrote it")
            {
                CHECK(values == *harmonics);
            }
        }
    }
}
