#pragma once

// Thin JUCE control wrappers implementing IBoundControl. Functional-first
// (standard JUCE widgets); a later LookAndFeel pass restyles them without
// touching this behavior. A shared BoundControl base holds the (re-bindable)
// parameter name so page-family blocks can retarget a control to another
// instance. [RQ-GUI-030, RQ-GUI-032, RQ-GUI-010, ADR-JUC-006]

#include "xplorer/app/ParameterBindingRegistry.hpp"

#include <juce_gui_extra/juce_gui_extra.h>

#include <memory>
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

        // Double-click opens a transient inline numeric entry (no permanent
        // text box; value otherwise shown on the VFD and the drag bubble).
        // [RQ-GUI-034]
        void mouseDoubleClick(const juce::MouseEvent& event) override;

    private:
        void applyTextEntry();
        void dismissTextEntry();

        std::unique_ptr<juce::TextEditor> _entryEditor;
    };

    class BoundComboBox final : public juce::ComboBox, public BoundControl
    {
    public:
        BoundComboBox(ParameterBindingRegistry& registry, std::string parameterName,
                      const std::vector<std::pair<std::string, int>>& options);

        void setDisplayedValue(int value) override;
        [[nodiscard]] std::string displayText() const override;
        juce::Component& asComponent() override { return *this; }

        // juce::ComboBox does not repaint on plain hover (unlike Slider), so the
        // LookAndFeel's hover state would never be redrawn; trigger it here.
        // [RQ-GUI-041, ADR-JUC-017]
        void mouseEnter(const juce::MouseEvent&) override { repaint(); }
        void mouseExit(const juce::MouseEvent&) override { repaint(); }

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

    // An exclusive radio-button group bound to one parameter: N labelled
    // ToggleButtons (mutually exclusive via a shared radio group id) stacked in
    // the control's bounds, each mapping to a parameter value. Replaces the
    // combo-box rendering of the FM destination panel. [RQ-GUI-038, ADR-JUC-016]
    class BoundRadioGroup final : public juce::Component, public BoundControl
    {
    public:
        BoundRadioGroup(ParameterBindingRegistry& registry, std::string parameterName,
                        const std::vector<std::pair<std::string, int>>& options);

        void setDisplayedValue(int value) override;
        [[nodiscard]] std::string displayText() const override;
        juce::Component& asComponent() override { return *this; }

        void resized() override;

    private:
        void onOptionClicked(int value);

        struct Option
        {
            std::unique_ptr<juce::ToggleButton> button;
            int value;
        };
        std::vector<Option> _options;
    };
}
