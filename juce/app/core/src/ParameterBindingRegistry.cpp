#include "xplorer/app/ParameterBindingRegistry.hpp"

#include "midiapp/service/Logger.hpp"

namespace xplorer::app
{
    using midiapp::service::LogDomain;
    using midiapp::service::TraceLevel;

    ParameterBindingRegistry::ParameterBindingRegistry(controller::XpanderController& controller)
        : _controller(controller)
    {
    }

    bool ParameterBindingRegistry::bind(const std::string& parameterName, IBoundControl& control)
    {
        if (_controller.getParameter(parameterName) == nullptr)
        {
            // [RQ-FMW-075, ADR-FMW-001 (DEC-FMW-002, DEC-FMW-004)]
            XPL_LOG(LogDomain::ControllerCalls, TraceLevel::Warning,
                    "Could not find parameter [" + parameterName + "] for registered control");
            return false;
        }
        // [RQ-FMW-075, ADR-FMW-001 (DEC-FMW-002, DEC-FMW-004)]
        XPL_LOG(LogDomain::ControllerCalls, TraceLevel::Verbose,
                "Registering control for parameter " + parameterName);
        _bindings[parameterName] = &control;
        return true;
    }

    void ParameterBindingRegistry::unbind(const std::string& parameterName)
    {
        _bindings.erase(parameterName);
    }

    void ParameterBindingRegistry::onControlEditBegan(const std::string& parameterName)
    {
        // Reference MainForm behavior: while the user turns a knob, its
        // mapped CC must not fight the gesture. [RQ-GUI-004, RQ-FMW-033]
        const auto cc = _controller.controlChangeAutomationTable().ccNumberFor(parameterName);
        _controller.setDisabledControlChangeNumber(
            cc.value_or(controller::XpanderController::NO_CONTROL_CHANGE));
    }

    void ParameterBindingRegistry::onControlEditEnded()
    {
        _controller.setDisabledControlChangeNumber(controller::XpanderController::NO_CONTROL_CHANGE);
    }

    void ParameterBindingRegistry::setLocalEditHandler(
        std::function<void(const std::string&)> handler)
    {
        _localEditHandler = std::move(handler);
    }

    std::string ParameterBindingRegistry::displayTextFor(const std::string& parameterName) const
    {
        const auto found = _bindings.find(parameterName);
        return found == _bindings.end() ? std::string{} : found->second->displayText();
    }

    void ParameterBindingRegistry::onControlEdited(const std::string& parameterName, int value)
    {
        if (_refreshing)
        {
            return; // change fired by a model refresh: never echo back [RQ-GUI-003]
        }
        if (!_controller.setParameter(parameterName, value))
        {
            // [RQ-FMW-075, ADR-FMW-001 (DEC-FMW-002, DEC-FMW-004)]
            XPL_LOG(LogDomain::ControllerCalls, TraceLevel::Warning,
                    "setParameter rejected for " + parameterName);
        }
        if (_localEditHandler)
        {
            _localEditHandler(parameterName); // e.g. VFD refresh [RQ-GUI-020]
        }
    }

    void ParameterBindingRegistry::onParameterChanged(const std::string& parameterName, int value)
    {
        const auto found = _bindings.find(parameterName);
        if (found == _bindings.end())
        {
            // [RQ-FMW-075, ADR-FMW-001 (DEC-FMW-002, DEC-FMW-004)]
            XPL_LOG(LogDomain::ControllerCalls, TraceLevel::Warning,
                    "No control found for parameter " + parameterName);
            return;
        }
        _refreshing = true;
        found->second->setDisplayedValue(value);
        _refreshing = false;
    }

    void ParameterBindingRegistry::refreshAllFromModel()
    {
        _refreshing = true;
        for (const auto& [parameterName, control] : _bindings)
        {
            if (const auto* parameter = _controller.getParameter(parameterName))
            {
                control->setDisplayedValue(parameter->value());
            }
        }
        _refreshing = false;
    }
}
