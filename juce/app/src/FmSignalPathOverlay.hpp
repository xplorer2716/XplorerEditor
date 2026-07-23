#pragma once

// A mouse-transparent overlay painted above the static vector background: it
// re-strokes only the FM-bus branch that routes to the currently selected FM
// destination (VCO1 or VCF) in the knob-LED accent colour, so the active signal
// route is self-evident (comparable to Arturia Matrix-12 V active-path lighting).
// Branch geometry is single-sourced in FmSignalPath.hpp (shared with
// BackgroundRenderer); the accent colour is read live from the effective
// XplorerLookAndFeel (no cached copy), so a settings colour change applies
// immediately. [RQ-GUI-039, ADR-JUC-016 (DEC-JUC-017/018/019), ADR-JUC-011]

#include <juce_gui_extra/juce_gui_extra.h>

namespace xplorer::app
{
    class FmSignalPathOverlay final : public juce::Component
    {
    public:
        FmSignalPathOverlay();

        /// destination: 0 = VCO1, 1 = VCF (model::EnumFMDestinationTypes).
        /// Repaints only when the value actually changes.
        void setDestination(int destination);

        void paint(juce::Graphics& g) override;

    private:
        int _destination = 0;
    };
}
