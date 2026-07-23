#include "FmSignalPathOverlay.hpp"

#include "DesignTokens.hpp"
#include "FmSignalPath.hpp"
#include "XplorerLookAndFeel.hpp"

#include "xplorer/model/XpanderConstants.hpp"

namespace xplorer::app
{
    FmSignalPathOverlay::FmSignalPathOverlay()
    {
        // Purely decorative: never intercept clicks meant for the controls below
        // it in the z-order. [ADR-JUC-016 (DEC-JUC-017)]
        setInterceptsMouseClicks(false, false);
    }

    void FmSignalPathOverlay::setDestination(int destination)
    {
        if (destination == _destination)
        {
            return;
        }
        _destination = destination;
        repaint();
    }

    void FmSignalPathOverlay::paint(juce::Graphics& g)
    {
        // Accent = the knob LED colour, the single runtime source of truth
        // (ADR-JUC-011); read live so a settings colour change applies with no
        // cached copy, exactly like the modulation-matrix highlight. Falls back
        // to the neutral diagram colour if the skin is not our LookAndFeel.
        juce::Colour accent = tokens::semantic::diagramFrame;
        if (auto* laf = dynamic_cast<XplorerLookAndFeel*>(&getLookAndFeel()))
        {
            accent = laf->ledColour();
        }
        g.setColour(accent);

        // Same stroke metrics as the drawn diagram lines so the highlight
        // overlaps the neutral branch exactly. [RQ-GUI-037, DEC-JUC-018]
        const juce::PathStrokeType stroke{
            tokens::semantic::strokeLine, juce::PathStrokeType::curved, juce::PathStrokeType::rounded};

        const auto strokeSeg = [&](const FmSeg& s)
        {
            juce::Path p;
            p.startNewSubPath(s.x1, s.y1);
            p.lineTo(s.x2, s.y2);
            g.strokePath(p, stroke);
        };

        if (_destination == static_cast<int>(model::EnumFMDestinationTypes::FMDESTFM_VCF))
        {
            for (const auto& s : FM_BRANCH_VCF)
            {
                strokeSeg(s);
            }
            juce::Path hop;
            hop.startNewSubPath(FM_VCF_HOP_ENDS.x1, FM_VCF_HOP_ENDS.y1);
            hop.quadraticTo(FM_VCF_HOP_CTRL_X, FM_VCF_HOP_CTRL_Y, FM_VCF_HOP_ENDS.x2, FM_VCF_HOP_ENDS.y2);
            g.strokePath(hop, stroke);
        }
        else
        {
            for (const auto& s : FM_BRANCH_VCO1)
            {
                strokeSeg(s);
            }
        }
    }
}
