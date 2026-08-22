// A page-family block: one set of on-screen controls that edits any ONE of
// several identical parameter groups -- ENV 1-5, LFO 1-5, RAMP 1-4, TRACK 1-3.
// The instrument works the same way, which is why the editor does.
//
// The mechanism is a TAG INDIRECTION. The generated control table gives these
// controls a family tag with a placeholder, e.g. "ENV_X_ATTACK". Nothing is
// bound to that tag: at any moment the block resolves it against the active
// instance (resolveControlTag, headless, in xpl_app_core) to a concrete
// parameter name such as "ENV_3_ATTACK", and binds THAT. Switching instance
// means rebinding the same widgets to different parameters -- no widget is
// created or destroyed, and no per-instance UI code exists.
//
// Two ways the active instance changes, and they must not fight:
//   * selectInstance()          -- the user clicked a selector here, so the
//                                  synth is told to follow (page select).
//   * setActiveInstanceFromSynth() -- the synth changed page on its own panel,
//                                  so the UI follows and sends NOTHING back.
// The asymmetry is the echo guard: whichever side initiated does not get told
// its own news. [RQ-GUI-011, RQ-CTL-023, RQ-CTL-028]
#include "PageFamilyBlock.hpp"

#include "DesignTokens.hpp"
#include "XplorerLookAndFeel.hpp"

#include "xplorer/app/ControlMetadata.hpp"

namespace xplorer::app
{
    namespace
    {
        // Copy/Paste Page context-menu item ids (reference "toolStripPageMenuItemCopy"/
        // "...Paste", .resx text "Copy Page"/"Paste Page"). [RQ-GUI-027, issue #13]
        constexpr int COPY_PAGE_MENU_ITEM_ID = 1;
        constexpr int PASTE_PAGE_MENU_ITEM_ID = 2;

        /// Identity of the block a page family drives, keyed by the
        /// descriptor's control-tag prefix. The four page families are exactly
        /// ENV/LFO/RAMP/TRACK (pageFamilies()); the colour itself is resolved
        /// from the live palette at paint time. [RQ-GUI-045, RQ-DSN-095,
        /// ADR-JUC-020 (DEC-JUC-037)]
        BlockId blockIdForFamily(const std::string& controlTagPrefix)
        {
            if (controlTagPrefix == "ENV_X")   { return BlockId::Env; }
            if (controlTagPrefix == "LFO_X")   { return BlockId::Lfo; }
            if (controlTagPrefix == "RAMP_X")  { return BlockId::Ramp; }
            jassert(controlTagPrefix == "TRACK_X"); // a fifth family needs a mapping here
            return BlockId::Track;
        }
    }

    PageSelectorButton::PageSelectorButton(const juce::String& text,
                                           controller::XpanderController& controller, std::string id,
                                           BlockId blockId)
        : juce::TextButton(text), _controller(controller), _id(std::move(id)), _blockId(blockId)
    {
        setWantsKeyboardFocus(true); // so Ctrl+C/Ctrl+V reach keyPressed while focused
    }

    void PageSelectorButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool)
    {
        // Border: always the pure block hue -- the permanent "I belong to this
        // block" cue. Background: the block's own fill expression
        // (hue at blockFillAlpha, identical to BackgroundRenderer's box fill)
        // only while this instance is active. Hover and keyboard focus are
        // reproduced here because painting ourselves bypasses LookAndFeel_V4's
        // stock feedback. The hue comes from the live LookAndFeel palette
        // (no cached copy). [RQ-GUI-045, RQ-GUI-041, RQ-GUI-042, ADR-JUC-019,
        // ADR-JUC-020 (DEC-JUC-037)]
        const auto* laf = dynamic_cast<const XplorerLookAndFeel*>(&getLookAndFeel());
        const juce::Colour blockColour =
            blockColourOf(laf != nullptr ? laf->blockPalette() : defaultBlockPalette(), _blockId);
        const auto bounds = getLocalBounds().toFloat().reduced(tokens::semantic::strokeBorder * 0.5F);
        const float corner = tokens::semantic::radiusControl;
        const bool hovered = isEnabled() && shouldDrawButtonAsHighlighted;

        if (getToggleState())
        {
            auto fill = blockColour.withAlpha(tokens::component::blockFillAlpha);
            if (hovered)
            {
                fill = fill.brighter(tokens::semantic::hoverBrighten);
            }
            g.setColour(fill);
            g.fillRoundedRectangle(bounds, corner);
        }
        else if (hovered)
        {
            g.setColour(tokens::semantic::surfaceRecessed.brighter(tokens::semantic::hoverBrighten));
            g.fillRoundedRectangle(bounds, corner);
        }

        g.setColour(hovered ? blockColour.brighter(tokens::semantic::hoverBrighten) : blockColour);
        g.drawRoundedRectangle(bounds, corner, tokens::semantic::strokeBorder);

        getLookAndFeel().drawButtonText(g, *this, shouldDrawButtonAsHighlighted, false);
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
                case ControlKind::RadioButtonPanel:
                {
                    // The ENV X / RAMP X SINGLE-MULTI trigger panels. Same widget
                    // as the fixed-block path (MainComponent), keyed on the shared
                    // "_X_" tag like the combo above and bound to the concrete
                    // instance -- so the block needs no radio-specific code at all
                    // for instance switching. Was missing entirely, so both panels
                    // fell through to nullptr and never appeared (issue #31).
                    // [RQ-GUI-053, RQ-GUI-010, ADR-JUC-016 (DEC-JUC-085)]
                    const auto options = radioPanelOptions(spec.tag);
                    if (options.empty())
                    {
                        return nullptr;
                    }
                    return std::make_unique<BoundRadioGroup>(registry, concreteParameterName, options);
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
                _controller, selectorId, blockIdForFamily(_descriptor.controlTagPrefix));
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
            // Send the page-select for the instance just chosen (not
            // whatever page was previously tracked). [RQ-GUI-011, RQ-CTL-028]
            _controller.sendPageUpdate(pageForInstance(_descriptor, instance), 0);
        }
        rebindControlsToActiveInstance();
    }

    // Inbound counterpart of selectInstance: the user turned the page on the
    // instrument itself. Same state change, but deliberately WITHOUT a page
    // select back to the synth -- and the selector buttons are updated with
    // juce::dontSendNotification so their onClick does not fire and re-enter
    // selectInstance. Both omissions are the echo guard; either one restored
    // would bounce the change back at the instrument. [RQ-GUI-011, RQ-CTL-023]
    //
    // Out-of-range values are ignored rather than clamped: they mean the synth
    // reported a page this family does not cover, which is not this block's.
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

    // Hovering a control highlights the matching row of the modulation matrix.
    // Only knobs qualify, because on this instrument only continuous parameters
    // can be modulation destinations -- hence the dynamic_cast filter rather
    // than attaching to everything. The selectors are included so hovering the
    // instance buttons highlights the whole block. [RQ-GUI-018, ADR-JUC-010]
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

    // The heart of the family mechanism. For every control: drop the old
    // binding, resolve the family tag against the active instance, adopt the
    // concrete name, rebind, and seed the widget with the new parameter's
    // current value.
    //
    // The unbind MUST come first. The registry is keyed by parameter name, so
    // binding the new name before releasing the old one would leave the block's
    // previous instance still registered and receiving updates through a widget
    // that no longer represents it.
    //
    // A parameter that does not resolve leaves the widget bound but unseeded --
    // it keeps its last displayed value rather than jumping to zero.
    // [RQ-GUI-011]
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
