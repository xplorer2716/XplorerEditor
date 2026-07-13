#include "xplorer/app/ParameterBindingRegistry.hpp"

namespace xplorer::app
{
    ParameterBindingRegistry::ParameterBindingRegistry(controller::XpanderController& controller)
        : _controller(controller)
    {
    }

    bool ParameterBindingRegistry::bind(const std::string& parameterName, IBoundControl& control)
    {
        if (_controller.getParameter(parameterName) == nullptr)
        {
            return false;
        }
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
        std::function<void(const std::string&, int)> handler)
    {
        _localEditHandler = std::move(handler);
    }

    void ParameterBindingRegistry::onControlEdited(const std::string& parameterName, int value)
    {
        if (_refreshing)
        {
            return; // change fired by a model refresh: never echo back [RQ-GUI-003]
        }
        _controller.setParameter(parameterName, value);
        if (_localEditHandler)
        {
            _localEditHandler(parameterName, value); // e.g. VFD refresh [RQ-GUI-020]
        }
    }

    void ParameterBindingRegistry::onParameterChanged(const std::string& parameterName, int value)
    {
        const auto found = _bindings.find(parameterName);
        if (found == _bindings.end())
        {
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
