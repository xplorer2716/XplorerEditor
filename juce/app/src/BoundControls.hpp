#pragma once

// Thin JUCE control wrappers implementing IBoundControl. Functional-first
// (standard JUCE widgets); a later LookAndFeel pass restyles them without
// touching this behavior. A shared BoundControl base holds the (re-bindable)
// parameter name so page-family blocks can retarget a control to another
// instance. [RQ-GUI-030, RQ-GUI-032, RQ-GUI-010, ADR-006]

#include "xplorer/app/ParameterBindingRegistry.hpp"

#include <juce_gui_extra/juce_gui_extra.h>

#include <string>
#include <utility>
#include <vector>

namespace xplorer::app
{
    /// Common behavior for a bound control: it knows its registry and the
    /// parameter it currently targets (which page-family blocks can change).
    class BoundControl : public IBoundControl
    {
    public:
        BoundControl(ParameterBindingRegistry& registry, std::string parameterName)
            : _registry(registry), _parameterName(std::move(parameterName))
        {
        }

        [[nodiscard]] const std::string& parameterName() const { return _parameterName; }
        void setParameterName(std::string parameterName) { _parameterName = std::move(parameterName); }

        /// Returns the underlying juce::Component (for layout/visibility).
        [[nodiscard]] virtual juce::Component& asComponent() = 0;

    protected:
        ParameterBindingRegistry& _registry;
        std::string _parameterName;
    };

    class BoundKnob final : public juce::Slider, public BoundControl
    {
    public:
        BoundKnob(ParameterBindingRegistry& registry, std::string parameterName,
                  int minValue, int maxValue, int step);

        void setDisplayedValue(int value) override;
        [[nodiscard]] std::string displayText() const override;
        juce::Component& asComponent() override { return *this; }
    };

    class BoundComboBox final : public juce::ComboBox, public BoundControl
    {
    public:
        BoundComboBox(ParameterBindingRegistry& registry, std::string parameterName,
                      const std::vector<std::pair<std::string, int>>& options);

        void setDisplayedValue(int value) override;
        [[nodiscard]] std::string displayText() const override;
        juce::Component& asComponent() override { return *this; }

    private:
        std::vector<int> _valueByRow;
    };

    class BoundCheckBox final : public juce::ToggleButton, public BoundControl
    {
    public:
        BoundCheckBox(ParameterBindingRegistry& registry, std::string parameterName,
                      const juce::String& text);

        void setDisplayedValue(int value) override;
        [[nodiscard]] std::string displayText() const override;
        juce::Component& asComponent() override { return *this; }
    };
}
