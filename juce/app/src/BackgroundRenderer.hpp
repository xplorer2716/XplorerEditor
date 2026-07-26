#pragma once

// Vector background: paints the synth block-diagram background with juce::Graphics
// primitives instead of a stretched bitmap, so it stays crisp at any window
// scale. Geometry is the reference layout measured from the former
// main-background.jpg (owner-validated mockup, ADR-JUC-013-mockup.svg); every
// draw here maps 1:1 to a mockup primitive. [RQ-GUI-037, ADR-JUC-013]

#include "BlockPalette.hpp"

#include <juce_gui_extra/juce_gui_extra.h>

namespace xplorer::app
{
    /// Paints the full background in the logical canvas coordinate space
    /// (LOGICAL_CANVAS_WIDTH x LOGICAL_CANVAS_HEIGHT). Called directly from the
    /// canvas paint() — never buffered to an image, because the canvas
    /// AffineTransform would then rescale the cache and reintroduce blur
    /// (ADR-JUC-013 §3). Pure function of (graphics, palette): the caller
    /// passes the live block palette (MainComponent) or the defaults (splash),
    /// so the painter holds no runtime colour state. [RQ-DSN-095, ADR-JUC-020
    /// (DEC-JUC-036)]
    void paintVectorBackground(juce::Graphics& g, const BlockPalette& palette);
}
