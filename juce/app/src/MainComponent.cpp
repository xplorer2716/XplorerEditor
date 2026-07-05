#include "MainComponent.hpp"

#include "BinaryData.h"
#include "xplorer/app/ControlMetadata.hpp"
#include "xplorer/app/ControlTable.hpp"

#include <juce_core/juce_core.h>

namespace xplorer::app
{
    namespace
    {
        // Fixed (non page-family) blocks handled in TASK-JUCE-063.
        bool isFixedBlockTag(const std::string& tag)
        {
            if (tag.empty() || tag.find("_X") != std::string::npos)
            {
                return false;
            }
            for (const auto* prefix : {"VCO1", "VCO2", "FM", "LAG", "VCF"})
            {
                if (tag.rfind(prefix, 0) == 0)
                {
                    return true;
                }
            }
            return false;
        }

        juce::String settingsDirectory()
        {
            return juce::File::getSpecialLocation(juce::File::commonApplicationDataDirectory)
                .getChildFile("Xplorer")
                .getChildFile("Xplorer")
                .getFullPathName();
        }
    }

    MainComponent::MainComponent()
    {
        _background = juce::ImageCache::getFromMemory(BinaryData::mainbackground_jpg,
                                                      BinaryData::mainbackground_jpgSize);
        _dispatcher = std::make_shared<JuceEventDispatcher>();
        _settingsService = std::make_unique<settings::XmlSettingsService>(
            settingsDirectory().toStdString());
        _controller = std::make_unique<controller::XpanderController>(
            _backend, *_settingsService, _dispatcher, "XPLORER");
        _registry = std::make_unique<ParameterBindingRegistry>(*_controller);

        // Route controller parameter changes to the registry (UI refresh). [RQ-GUI-003]
        _controller->setAutomationParameterChangeHandler(
            [this](const std::string& name, int value) { _registry->onParameterChanged(name, value); });
        _controller->setFullToneChangeHandler(
            [this](const controller::FullToneChangeEvent&) { _registry->refreshAllFromModel(); });
        _controller->setPageChangeHandler(
            [this](const controller::PageChangeEvent& event) { onSynthPageChanged(event); });

        setSize(LOGICAL_CANVAS_WIDTH, LOGICAL_CANVAS_HEIGHT);
        placeFixedBlockControls();
        createPageFamilyBlocks();
        _registry->refreshAllFromModel(); // seed all controls with the current tone
    }

    MainComponent::~MainComponent() = default;

    void MainComponent::placeFixedBlockControls()
    {
        for (const auto& spec : controlTable())
        {
            const std::string tag = spec.tag;
            if (!isFixedBlockTag(tag))
            {
                continue;
            }
            const auto* parameter = _controller->getParameter(tag);
            std::unique_ptr<juce::Component> component;
            IBoundControl* bound = nullptr;

            switch (spec.kind)
            {
                case ControlKind::KnobControl:
                {
                    if (parameter == nullptr)
                    {
                        continue;
                    }
                    auto knob = std::make_unique<BoundKnob>(*_registry, tag, parameter->minValue(),
                                                            parameter->maxValue(), parameter->step());
                    bound = knob.get();
                    component = std::move(knob);
                    break;
                }
                case ControlKind::CheckBoxValuedControl:
                {
                    if (parameter == nullptr)
                    {
                        continue;
                    }
                    auto check = std::make_unique<BoundCheckBox>(*_registry, tag, juce::String());
                    bound = check.get();
                    component = std::move(check);
                    break;
                }
                case ControlKind::ComboBoxValuedControl:
                {
                    std::vector<std::pair<std::string, int>> options;
                    const auto labels = comboLabelsForControl(tag);
                    for (std::size_t i = 0; i < labels.size(); ++i)
                    {
                        options.emplace_back(labels[i], static_cast<int>(i));
                    }
                    if (options.empty())
                    {
                        continue;
                    }
                    auto combo = std::make_unique<BoundComboBox>(*_registry, tag, options);
                    bound = combo.get();
                    component = std::move(combo);
                    break;
                }
                case ControlKind::RadioButtonPanel:
                {
                    const auto options = radioPanelOptions(tag);
                    if (options.empty())
                    {
                        continue;
                    }
                    auto combo = std::make_unique<BoundComboBox>(*_registry, tag, options);
                    bound = combo.get();
                    component = std::move(combo);
                    break;
                }
                default:
                    continue;
            }

            component->setBounds(spec.x, spec.y, spec.width, spec.height);
            addAndMakeVisible(*component);
            if (bound != nullptr)
            {
                _registry->bind(tag, *bound);
            }
            _controls.push_back(std::move(component));
        }
    }

    void MainComponent::createPageFamilyBlocks()
    {
        int radioGroup = 100;
        for (const auto& family : pageFamilies())
        {
            std::vector<ControlSpec> controlSpecs;
            std::vector<ControlSpec> selectorSpecs;
            const std::string selectorPrefix =
                family.controlTagPrefix.substr(0, family.controlTagPrefix.size() - 1); // "ENV_"
            for (const auto& spec : controlTable())
            {
                const std::string tag = spec.tag;
                const std::string id = spec.id;
                if (tag.rfind(family.controlTagPrefix, 0) == 0)
                {
                    controlSpecs.push_back(spec); // "ENV_X_*"
                }
                else if (spec.kind == ControlKind::RadioButton
                         && id.rfind(selectorPrefix, 0) == 0
                         && id.size() == selectorPrefix.size() + 1
                         && id.back() >= '1' && id.back() <= '9')
                {
                    selectorSpecs.push_back(spec); // "ENV_1".."ENV_5"
                }
            }
            _familyBlocks.push_back(std::make_unique<PageFamilyBlock>(
                *this, *_registry, *_controller, family, controlSpecs, selectorSpecs, radioGroup++));
        }
    }

    void MainComponent::onSynthPageChanged(const controller::PageChangeEvent& event)
    {
        // Map the synth page to a family + instance and activate the selector. [RQ-GUI-012]
        struct Range { model::EnumPages first; model::EnumPages last; const char* prefix; };
        static const Range ranges[] = {
            {model::EnumPages::ENV_1, model::EnumPages::ENV_5, "ENV_X"},
            {model::EnumPages::LFO_1, model::EnumPages::LFO_5, "LFO_X"},
            {model::EnumPages::RAMP_1, model::EnumPages::RAMP_4, "RAMP_X"},
            {model::EnumPages::TRACK_1, model::EnumPages::TRACK_3, "TRACK_X"},
        };
        const int page = static_cast<int>(event.page);
        for (const auto& range : ranges)
        {
            if (page >= static_cast<int>(range.first) && page <= static_cast<int>(range.last))
            {
                const int instance = page - static_cast<int>(range.first) + 1;
                for (auto& block : _familyBlocks)
                {
                    if (block->familyPrefix() == range.prefix)
                    {
                        block->setActiveInstanceFromSynth(instance);
                    }
                }
                return;
            }
        }
    }

    void MainComponent::paint(juce::Graphics& g)
    {
        g.fillAll(juce::Colours::black);
        if (_background.isValid())
        {
            g.drawImage(_background, getLocalBounds().toFloat()); // [RQ-GUI-007]
        }
    }

    ScaledCanvasComponent::ScaledCanvasComponent()
    {
        addAndMakeVisible(_canvas);
        setSize(LOGICAL_CANVAS_WIDTH, LOGICAL_CANVAS_HEIGHT);
    }

    void ScaledCanvasComponent::resized()
    {
        // Uniform scale, aspect ratio preserved, canvas centered. [RQ-GUI-005]
        const auto scale = juce::jmin(static_cast<float>(getWidth()) / LOGICAL_CANVAS_WIDTH,
                                      static_cast<float>(getHeight()) / LOGICAL_CANVAS_HEIGHT);
        const auto scaledWidth = LOGICAL_CANVAS_WIDTH * scale;
        const auto scaledHeight = LOGICAL_CANVAS_HEIGHT * scale;
        _canvas.setTransform(juce::AffineTransform::scale(scale).translated(
            (static_cast<float>(getWidth()) - scaledWidth) * 0.5F,
            (static_cast<float>(getHeight()) - scaledHeight) * 0.5F));
    }

    void ScaledCanvasComponent::paint(juce::Graphics& g)
    {
        g.fillAll(juce::Colours::black); // letterbox bars when aspect differs
    }
}
