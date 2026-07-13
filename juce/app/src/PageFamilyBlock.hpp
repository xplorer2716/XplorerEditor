#pragma once

// JUCE view for one page family (ENV/LFO/RAMP/TRACK): a shared set of
// controls plus instance-selector buttons. Selecting an instance retargets
// every control to that instance's parameters, refreshes their values, and
// asks the controller to select the matching synth page. Reverse: a synth
// page-change activates the selector. [RQ-GUI-010..012, ADR-006]

#include "BoundControls.hpp"

#include "xplorer/app/ControlTable.hpp"
#include "xplorer/app/PageFamilyModel.hpp"
#include "xplorer/controller/XpanderController.hpp"

#include <juce_gui_extra/juce_gui_extra.h>

#include <memory>
#include <string>
#include <vector>

namespace xplorer::app
{
    class PageFamilyBlock
    {
    public:
        PageFamilyBlock(juce::Component& parent, ParameterBindingRegistry& registry,
                        controller::XpanderController& controller,
                        const PageFamilyDescriptor& descriptor,
                        const std::vector<ControlSpec>& controlSpecs,
                        const std::vector<ControlSpec>& selectorSpecs, int radioGroupId);

        /// Activates instance (1-based) without sending a page update to the
        /// synth — used when the synth itself drove the page change. [RQ-GUI-012]
        void setActiveInstanceFromSynth(int instance);

        [[nodiscard]] int activeInstance() const { return _activeInstance; }
        [[nodiscard]] const std::string& familyPrefix() const { return _descriptor.controlTagPrefix; }

        /// Selector button components + their id (e.g. "ENV_1"), for the
        /// application to attach modulation-highlight hover hooks. [RQ-GUI-018]
        struct Selector
        {
            std::unique_ptr<juce::TextButton> button;
            int instance;       ///< 1-based
            std::string id;     ///< reference id, e.g. "ENV_1"
        };
        [[nodiscard]] const std::vector<Selector>& selectors() const { return _selectors; }

        /// Attaches a hover listener to this block's rotary knobs and selector
        /// buttons (modulation highlight). [RQ-GUI-018]
        void attachHoverListener(juce::MouseListener* listener);

    private:
        struct FamilyControl
        {
            std::string controlTag; ///< the "_X_" tag
            std::unique_ptr<BoundControl> control;
        };

        void selectInstance(int instance, bool notifySynth);
        void rebindControlsToActiveInstance();

        juce::Component& _parent;
        ParameterBindingRegistry& _registry;
        controller::XpanderController& _controller;
        PageFamilyDescriptor _descriptor;
        int _activeInstance = 1;

        std::vector<FamilyControl> _controls;
        std::vector<Selector> _selectors;
    };
}
