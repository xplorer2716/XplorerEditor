#include "ShortcutIcons.hpp"

#include "DesignTokens.hpp"

#include <array>
#include <cmath>

// Icon geometry for the eight shortcut buttons. Every coordinate below is a
// FRACTION of `r`, the artwork radius, so the whole set scales with the key and
// nothing needs re-tuning if shortcutButtonSize moves. The shapes follow the
// owner's mockup, with the two departures RQ-GUI-064 records: `LoadFile` is a
// folder (absent from the mockup, and a third floppy would be indistinguishable
// from Save and Store at this size), and `MidiSettings` keeps the DIN rather
// than a gear, on owner instruction.
// [RQ-GUI-064, ADR-GUI-001 (DEC-GUI-001-A, DEC-GUI-001-C)]

namespace xplorer::app
{
    namespace
    {
        /// Artwork radius as a fraction of the key's side: the icon occupies the
        /// middle 60% and the remaining margin is what keeps eight adjacent keys
        /// from reading as one striped band.
        constexpr float ICON_RADIUS_RATIO = 0.30F;

        /// Dot radius for die pips and DIN pins, as a fraction of the key side.
        /// A dot is filled, so it carries far more ink than a stroke of the same
        /// nominal width — hence its own ratio rather than reusing the stroke.
        constexpr float DOT_RADIUS_RATIO = 0.064F;

        void addPolyline(juce::Path& p, std::initializer_list<juce::Point<float>> points,
                         bool close = false)
        {
            bool first = true;
            for (const auto& pt : points)
            {
                if (first) { p.startNewSubPath(pt); first = false; }
                else       { p.lineTo(pt); }
            }
            if (close)
            {
                p.closeSubPath();
            }
        }

        void addDot(juce::Path& p, float x, float y, float radius)
        {
            p.addEllipse(x - radius, y - radius, radius * 2.0F, radius * 2.0F);
        }

        /// A floppy disk: body, shutter at the top, label panel at the bottom.
        /// Save and Store share it — only the colour differs, which is what makes
        /// "write to file" and "write to synth" read as the same act on different
        /// targets. [RQ-GUI-064]
        void addFloppy(juce::Path& p, float cx, float cy, float r)
        {
            p.addRoundedRectangle(cx - r, cy - r, r * 2.0F, r * 2.0F, r * 0.14F);
            p.addRectangle(cx - r * 0.42F, cy - r, r * 0.84F, r * 0.70F);
            p.addRectangle(cx - r * 0.66F, cy + r * 0.16F, r * 1.32F, r * 0.84F);
        }

        /// A DIN-5 socket seen face on: the circle is drawn with a gap at twelve
        /// o'clock which the keyway bridges, and the five pins sit on the lower
        /// arc. Pins and keyway both matter — a bare circle with dots reads as a
        /// face, which is what the first draft did (owner review, 2026-08-10).
        void addDinSocket(juce::Path& stroked, juce::Path& filled,
                          float cx, float cy, float r, float dot)
        {
            const float notchHalfWidth = r * 0.30F;
            const float notchDepth = r * 0.40F;
            // Half-angle the keyway subtends, so the arc stops exactly where the
            // notch walls rise — derived, not eyeballed, or the two disagree at
            // any size but the one they were tuned at.
            const float gap = std::asin(notchHalfWidth / r);

            stroked.addCentredArc(cx, cy, r, r, 0.0F, gap,
                                  juce::MathConstants<float>::twoPi - gap, true);
            addPolyline(stroked, {{cx - notchHalfWidth, cy - r},
                                  {cx - notchHalfWidth, cy - r + notchDepth},
                                  {cx + notchHalfWidth, cy - r + notchDepth},
                                  {cx + notchHalfWidth, cy - r}});

            // Measured clockwise from twelve o'clock, symmetric about six.
            for (const float degrees : {82.0F, 133.0F, 180.0F, 227.0F, 278.0F})
            {
                const float a = juce::degreesToRadians(degrees);
                addDot(filled, cx + r * 0.58F * std::sin(a), cy - r * 0.58F * std::cos(a), dot);
            }
        }
    }

    ShortcutIconPaths shortcutIconPaths(ShortcutIcon icon, float boxSize)
    {
        ShortcutIconPaths paths;
        const float cx = boxSize * 0.5F;
        const float cy = boxSize * 0.5F;
        const float r = boxSize * ICON_RADIUS_RATIO;
        const float dot = boxSize * DOT_RADIUS_RATIO;

        switch (icon)
        {
            case ShortcutIcon::PreviousProgram:
                addPolyline(paths.stroked, {{cx + r * 0.72F, cy - r},
                                            {cx - r * 0.80F, cy},
                                            {cx + r * 0.72F, cy + r}}, true);
                break;

            case ShortcutIcon::NextProgram:
                addPolyline(paths.stroked, {{cx - r * 0.72F, cy - r},
                                            {cx + r * 0.80F, cy},
                                            {cx - r * 0.72F, cy + r}}, true);
                break;

            case ShortcutIcon::GoToProgram:
                addPolyline(paths.stroked, {{cx - r * 1.15F, cy}, {cx + r * 0.60F, cy}});
                addPolyline(paths.stroked, {{cx + r * 0.02F, cy - r * 0.62F},
                                            {cx + r * 0.62F, cy},
                                            {cx + r * 0.02F, cy + r * 0.62F}});
                addPolyline(paths.stroked, {{cx + r * 0.95F, cy - r * 0.95F},
                                            {cx + r * 0.95F, cy + r * 0.95F}});
                break;

            case ShortcutIcon::Randomise:
                paths.stroked.addRoundedRectangle(cx - r, cy - r, r * 2.0F, r * 2.0F, r * 0.24F);
                for (const auto& pip : std::array<juce::Point<float>, 5>{
                         {{-0.5F, -0.5F}, {0.5F, -0.5F}, {0.0F, 0.0F}, {-0.5F, 0.5F}, {0.5F, 0.5F}}})
                {
                    addDot(paths.filled, cx + pip.x * r, cy + pip.y * r, dot);
                }
                break;

            case ShortcutIcon::LoadFile:
                // Back panel with its tab, then the front panel as a parallelogram
                // leaning left as it descends — the perspective that says "open".
                addPolyline(paths.stroked, {{cx - r * 1.02F, cy + r * 0.86F},
                                            {cx - r * 1.02F, cy - r * 0.86F},
                                            {cx - r * 0.34F, cy - r * 0.86F},
                                            {cx - r * 0.16F, cy - r * 0.56F},
                                            {cx + r * 0.62F, cy - r * 0.56F},
                                            {cx + r * 0.62F, cy - r * 0.16F}});
                addPolyline(paths.stroked, {{cx - r * 0.72F, cy - r * 0.16F},
                                            {cx + r * 1.05F, cy - r * 0.16F},
                                            {cx + r * 0.80F, cy + r * 0.86F},
                                            {cx - r * 1.02F, cy + r * 0.86F}}, true);
                break;

            case ShortcutIcon::SaveFile:
            case ShortcutIcon::StoreToSynth:
                addFloppy(paths.stroked, cx, cy, r);
                break;

            case ShortcutIcon::MidiSettings:
                addDinSocket(paths.stroked, paths.filled, cx, cy, r, dot);
                break;
        }

        return paths;
    }

    juce::PathStrokeType shortcutIconStroke(float boxSize)
    {
        const float width = tokens::component::shortcutIconStroke
                            * (boxSize / static_cast<float>(tokens::component::shortcutButtonSize));
        return juce::PathStrokeType{width, juce::PathStrokeType::curved,
                                    juce::PathStrokeType::rounded};
    }

    void paintShortcutButton(juce::Graphics& g, juce::Rectangle<float> bounds,
                             ShortcutIcon icon, juce::Colour accent,
                             bool isHovered, bool isDown)
    {
        const float radius = tokens::semantic::radiusControl;
        const auto border = tokens::semantic::diagramFrame
                                .withAlpha(tokens::component::shortcutButtonBorderAlpha);

        // Down inverts the key rather than darkening it: at 29 px a brightness
        // step alone is easy to miss, and the row gives no other feedback that a
        // one-shot action fired. [ADR-JUC-017]
        const auto fill = isDown ? border.withAlpha(1.0F)
                                 : tokens::semantic::surfaceRecessed;

        // Hover lights the key in the SAME accent every other control uses — the
        // LookAndFeel's ledColour, brightened by hoverBrighten, exactly as a knob
        // ring, a tick box border and a radio do. It comes in as a parameter, not
        // a token: the accent is user-themeable (ADR-JUC-020), and a frozen
        // colour would leave these eight keys behind when everything around them
        // followed the user's choice.
        //
        // StoreToSynth keeps its own red ink: it is the one key that writes to
        // the hardware, and recolouring it on hover would spend the
        // destructive-action signal to buy a hover signal. Its OUTLINE still
        // lights like every other key, so the hover is never ambiguous.
        // [RQ-GUI-067, ADR-JUC-011, ADR-JUC-017, ADR-GUI-001 (DEC-GUI-001-F)]
        const auto highlight = accent.brighter(tokens::semantic::hoverBrighten);
        const bool isDestructive = icon == ShortcutIcon::StoreToSynth;
        auto ink = isDestructive ? tokens::semantic::indicatorSynthOut
                                 : tokens::semantic::diagramCaption;
        if (isDown)
        {
            ink = tokens::semantic::surfaceRecessed;
        }
        else if (isHovered)
        {
            ink = isDestructive ? ink.brighter(tokens::semantic::hoverBrighten) : highlight;
        }

        g.setColour(fill);
        g.fillRoundedRectangle(bounds, radius);
        g.setColour(isHovered ? highlight : border);
        g.drawRoundedRectangle(bounds.reduced(tokens::semantic::strokeBorder * 0.5F), radius,
                               tokens::semantic::strokeBorder);

        const auto paths = shortcutIconPaths(icon, bounds.getWidth());
        const auto toBounds = juce::AffineTransform::translation(bounds.getX(), bounds.getY());
        g.setColour(ink);
        g.strokePath(paths.stroked, shortcutIconStroke(bounds.getWidth()), toBounds);
        g.fillPath(paths.filled, toBounds);
    }
}
