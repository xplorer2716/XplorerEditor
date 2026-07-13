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

#include <functional>
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

        /// The current value formatted for the VFD, by control type: combo
        /// selection label, checkbox "Y"/"N", knob numeric. Mirrors the
        /// reference VfdDisplayHelper value formatting. [RQ-GUI-020]
        [[nodiscard]] virtual std::string displayText() const = 0;
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

        /// Fan-out for local (user) edits, invoked with the edited parameter
        /// name after the model is updated and skipped during model refreshes.
        /// The application wires this to the VFD display so a panel tweak shows
        /// the parameter/value, like the reference
        /// MainForm.AnyValuedControl_ValueChanged. [RQ-GUI-020]
        void setLocalEditHandler(std::function<void(const std::string&)> handler);

        /// The bound control's value formatted for the VFD, or empty when the
        /// name is unbound. [RQ-GUI-020]
        [[nodiscard]] std::string displayTextFor(const std::string& parameterName) const;

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
        std::function<void(const std::string&)> _localEditHandler;
        bool _refreshing = false;
    };
}
