#pragma once

// Control⇄parameter binding: routes user edits to the controller and
// controller events back to controls, with an anti-echo guard and
// automation-disable during interactive edits. UI-framework-free: JUCE
// components implement IBoundControl in the app target; tests use fakes.
// The application wires the controller's event handlers to the entry
// points below (the controller exposes one handler per event; fan-out to
// other consumers such as the display panel is the application's glue).
// [RQ-GUI-002..004, ADR-006]

#include "xplorer/controller/XpanderController.hpp"

#include <map>
#include <string>

namespace xplorer::app
{
    /// Minimal view of a bindable control.
    class IBoundControl
    {
    public:
        virtual ~IBoundControl() = default;

        /// Pushes a model value into the control's display. Implementations
        /// may fire their change callback; the registry's guard absorbs it.
        virtual void setDisplayedValue(int value) = 0;
    };

    class ParameterBindingRegistry
    {
    public:
        explicit ParameterBindingRegistry(controller::XpanderController& controller);

        /// Binds a parameter name to a control; false when the parameter is
        /// unknown to the tone (binding is skipped). Rebinding a name
        /// replaces the previous control (used by page-family blocks).
        bool bind(const std::string& parameterName, IBoundControl& control);
        void unbind(const std::string& parameterName);
        [[nodiscard]] std::size_t bindingCount() const { return _bindings.size(); }

        // --- from controls (user interaction) ---
        /// Interactive edit started: disable the mapped CC automation. [RQ-GUI-004]
        void onControlEditBegan(const std::string& parameterName);
        /// Interactive edit finished: re-enable automation.
        void onControlEditEnded();
        /// Value edited by the user; ignored while a model refresh is in
        /// progress (anti-echo). [RQ-GUI-003]
        void onControlEdited(const std::string& parameterName, int value);

        // --- from the controller (model changes) ---
        /// Single parameter changed (automation / synth / clipboard).
        void onParameterChanged(const std::string& parameterName, int value);
        /// Full tone changed: refresh every bound control from the model.
        void refreshAllFromModel();

    private:
        class RefreshGuard;

        controller::XpanderController& _controller;
        std::map<std::string, IBoundControl*> _bindings;
        bool _refreshing = false;
    };
}
