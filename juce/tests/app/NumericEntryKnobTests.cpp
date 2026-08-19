#include <catch2/catch_test_macros.hpp>

#include "NumericEntryKnob.hpp"

#include <juce_gui_basics/juce_gui_basics.h>

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
