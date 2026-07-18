#include "PageFamilyBlock.hpp"

#include "xplorer/app/ControlMetadata.hpp"

namespace xplorer::app
{
    namespace
    {
        // Copy/Paste Page context-menu item ids (reference "toolStripPageMenuItemCopy"/
        // "...Paste", .resx text "Copy Page"/"Paste Page"). [RQ-GUI-027, issue #13]
        constexpr int COPY_PAGE_MENU_ITEM_ID = 1;
        constexpr int PASTE_PAGE_MENU_ITEM_ID = 2;
    }

    PageSelectorButton::PageSelectorButton(const juce::String& text,
                                           controller::XpanderController& controller, std::string id)
        : juce::TextButton(text), _controller(controller), _id(std::move(id))
    {
        setWantsKeyboardFocus(true); // so Ctrl+C/Ctrl+V reach keyPressed while focused
    }

    void PageSelectorButton::mouseDown(const juce::MouseEvent& event)
    {
        if (event.mods.isPopupMenu())
        {
            showPageContextMenu();
            return; // right-click: menu only, no radio-selection toggle
        }
        TextButton::mouseDown(event);
    }

    bool PageSelectorButton::keyPressed(const juce::KeyPress& key)
    {
        if (key == juce::KeyPress('c', juce::ModifierKeys::commandModifier, 0))
        {
            copyPage();
            return true;
        }
        if (key == juce::KeyPress('v', juce::ModifierKeys::commandModifier, 0))
        {
            pastePage();
            return true;
        }
        return TextButton::keyPressed(key);
    }

    void PageSelectorButton::showPageContextMenu()
    {
        juce::PopupMenu menu;
        menu.addItem(COPY_PAGE_MENU_ITEM_ID, "Copy Page");
        menu.addItem(PASTE_PAGE_MENU_ITEM_ID, "Paste Page", _controller.canClipboardPasteTo(_id));
        menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(this),
                           [this](int result)
                           {
                               if (result == COPY_PAGE_MENU_ITEM_ID)
                               {
                                   copyPage();
                               }
                               else if (result == PASTE_PAGE_MENU_ITEM_ID)
                               {
                                   pastePage();
                               }
                           });
    }

    void PageSelectorButton::copyPage() { _controller.setClipboardSource(_id); }

    void PageSelectorButton::pastePage()
    {
        _controller.pasteClipboardTo(_id);
        _controller.clearClipboard();
    }

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
                                                               juce::String(spec.label)); // [RQ-GUI-032]
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

        // Instance selector buttons (radio group). Caption is the reference
        // '.label' ("ENV 1"…), not the bare digit. [RQ-GUI-011]
        for (const auto& spec : selectorSpecs)
        {
            const std::string selectorId = spec.id;
            const int instance = selectorId.back() - '0';
            auto button = std::make_unique<PageSelectorButton>(
                juce::String(spec.label).isNotEmpty() ? juce::String(spec.label)
                                                       : juce::String(instance),
                _controller, selectorId);
            button->setClickingTogglesState(true);
            button->setRadioGroupId(radioGroupId);
            button->setBounds(spec.x, spec.y, spec.width, spec.height);
            button->onClick = [this, instance] { selectInstance(instance, true); };
            if (instance == _activeInstance)
            {
                button->setToggleState(true, juce::dontSendNotification);
            }
            _parent.addAndMakeVisible(*button);
            _selectors.push_back({std::move(button), instance, selectorId});
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
            selector.button->setToggleState(selector.instance == instance, juce::dontSendNotification);
        }
        rebindControlsToActiveInstance();
    }

    void PageFamilyBlock::attachHoverListener(juce::MouseListener* listener)
    {
        for (auto& entry : _controls)
        {
            // Only rotary knobs are modulation destinations.
            if (dynamic_cast<juce::Slider*>(&entry.control->asComponent()) != nullptr)
            {
                entry.control->asComponent().addMouseListener(listener, false);
            }
        }
        for (auto& selector : _selectors)
        {
            selector.button->addMouseListener(listener, false);
        }
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
