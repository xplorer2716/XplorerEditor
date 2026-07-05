#include "PageFamilyBlock.hpp"

#include "xplorer/app/ControlMetadata.hpp"

namespace xplorer::app
{
    namespace
    {
        std::unique_ptr<BoundControl> makeControl(ParameterBindingRegistry& registry,
                                                  controller::XpanderController& controller,
                                                  const ControlSpec& spec,
                                                  const std::string& concreteParameterName)
        {
            const auto* parameter = controller.getParameter(concreteParameterName);
            switch (spec.kind)
            {
                case ControlKind::KnobControl:
                    if (parameter != nullptr)
                    {
                        return std::make_unique<BoundKnob>(registry, concreteParameterName,
                                                           parameter->minValue(), parameter->maxValue(),
                                                           parameter->step());
                    }
                    return nullptr;
                case ControlKind::CheckBoxValuedControl:
                    if (parameter != nullptr)
                    {
                        return std::make_unique<BoundCheckBox>(registry, concreteParameterName,
                                                               juce::String());
                    }
                    return nullptr;
                case ControlKind::ComboBoxValuedControl:
                {
                    std::vector<std::pair<std::string, int>> options;
                    const auto labels = comboLabelsForControl(spec.tag); // enum type keyed on the "_X_" tag
                    for (std::size_t i = 0; i < labels.size(); ++i)
                    {
                        options.emplace_back(labels[i], static_cast<int>(i));
                    }
                    if (options.empty())
                    {
                        return nullptr;
                    }
                    return std::make_unique<BoundComboBox>(registry, concreteParameterName, options);
                }
                default:
                    return nullptr;
            }
        }
    }

    PageFamilyBlock::PageFamilyBlock(juce::Component& parent, ParameterBindingRegistry& registry,
                                     controller::XpanderController& controller,
                                     const PageFamilyDescriptor& descriptor,
                                     const std::vector<ControlSpec>& controlSpecs,
                                     const std::vector<ControlSpec>& selectorSpecs, int radioGroupId)
        : _parent(parent), _registry(registry), _controller(controller), _descriptor(descriptor)
    {
        // Shared controls, initially bound to instance 1.
        for (const auto& spec : controlSpecs)
        {
            auto concrete = resolveControlTag(spec.tag, _activeInstance);
            auto control = makeControl(_registry, _controller, spec, concrete);
            if (control == nullptr)
            {
                continue;
            }
            control->asComponent().setBounds(spec.x, spec.y, spec.width, spec.height);
            _parent.addAndMakeVisible(control->asComponent());
            _registry.bind(concrete, *control);
            _controls.push_back({spec.tag, std::move(control)});
        }

        // Instance selector buttons (radio group).
        for (const auto& spec : selectorSpecs)
        {
            auto button = std::make_unique<juce::TextButton>(juce::String(spec.id).fromLastOccurrenceOf(
                "_", false, false));
            button->setClickingTogglesState(true);
            button->setRadioGroupId(radioGroupId);
            button->setBounds(spec.x, spec.y, spec.width, spec.height);
            const std::string selectorId = spec.id;
            const int instance = selectorId.back() - '0';
            button->onClick = [this, instance] { selectInstance(instance, true); };
            if (instance == _activeInstance)
            {
                button->setToggleState(true, juce::dontSendNotification);
            }
            _parent.addAndMakeVisible(*button);
            _selectors.push_back(std::move(button));
        }
    }

    void PageFamilyBlock::selectInstance(int instance, bool notifySynth)
    {
        if (instance < 1 || instance > _descriptor.count)
        {
            return;
        }
        _activeInstance = instance;
        if (notifySynth)
        {
            // Reference RefreshPage: tell the synth to select this page. [RQ-GUI-011]
            _controller.forceSendPageSubPage();
        }
        rebindControlsToActiveInstance();
    }

    void PageFamilyBlock::setActiveInstanceFromSynth(int instance)
    {
        if (instance < 1 || instance > _descriptor.count)
        {
            return;
        }
        _activeInstance = instance;
        for (auto& selector : _selectors)
        {
            const int selectorInstance = selector->getButtonText().getIntValue();
            selector->setToggleState(selectorInstance == instance, juce::dontSendNotification);
        }
        rebindControlsToActiveInstance();
    }

    void PageFamilyBlock::rebindControlsToActiveInstance()
    {
        for (auto& entry : _controls)
        {
            _registry.unbind(entry.control->parameterName());
            const auto concrete = resolveControlTag(entry.controlTag, _activeInstance);
            entry.control->setParameterName(concrete);
            _registry.bind(concrete, *entry.control);
            if (const auto* parameter = _controller.getParameter(concrete))
            {
                entry.control->setDisplayedValue(parameter->value()); // [RQ-GUI-011]
            }
        }
    }
}
