#pragma once

// Thin JUCE control wrappers implementing IBoundControl. Functional-first
// (standard JUCE widgets); a later LookAndFeel pass restyles them without
// touching this behavior. [RQ-GUI-030, RQ-GUI-032, ADR-006]

#include "xplorer/app/ParameterBindingRegistry.hpp"

#include <juce_gui_extra/juce_gui_extra.h>

#include <string>
#include <vector>

namespace xplorer::app
{
    /// Rotary knob bound to an integer parameter. Range/step are applied at
    /// construction from the model (min/max/step passed in by the placer).
    class BoundKnob final : public juce::Slider, public IBoundControl
    {
    public:
        BoundKnob(ParameterBindingRegistry& registry, std::string parameterName,
                  int minValue, int maxValue, int step);

        void setDisplayedValue(int value) override;

    private:
        ParameterBindingRegistry& _registry;
        std::string _parameterName;
    };

    /// Combo bound to a parameter; entries are (label, value) where value is
    /// the parameter value selected. Covers enum combos and radio panels.
    class BoundComboBox final : public juce::ComboBox, public IBoundControl
    {
    public:
        BoundComboBox(ParameterBindingRegistry& registry, std::string parameterName,
                      const std::vector<std::pair<std::string, int>>& options);

        void setDisplayedValue(int value) override;

    private:
        ParameterBindingRegistry& _registry;
        std::string _parameterName;
        std::vector<int> _valueByRow;
    };

    /// Checkbox bound to a 0/1 parameter.
    class BoundCheckBox final : public juce::ToggleButton, public IBoundControl
    {
    public:
        BoundCheckBox(ParameterBindingRegistry& registry, std::string parameterName,
                      const juce::String& text);

        void setDisplayedValue(int value) override;

    private:
        ParameterBindingRegistry& _registry;
        std::string _parameterName;
    };
}
