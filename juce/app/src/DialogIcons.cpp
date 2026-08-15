#include "DialogIcons.hpp"

#include "DesignTokens.hpp"

// Geometry is a fraction of `r`, the glyph radius, so both icons scale with
// dialogIconSize and nothing needs re-tuning if that token moves. [RQ-GUI-070]

namespace xplorer::app
{
    namespace
    {
        constexpr float GLYPH_RADIUS_RATIO = 0.42F;  // glyph radius as a fraction of the badge side
        constexpr int GEAR_TEETH = 8;
        constexpr float GEAR_ROOT_RATIO = 0.60F;   // hub/tooth-root radius, as a fraction of r
        constexpr float GEAR_TOOTH_HALF_ANGLE = juce::MathConstants<float>::pi / GEAR_TEETH * 0.42F;
        constexpr float GEAR_HOLE_RATIO = 0.32F;   // centre hole radius, as a fraction of r

        void fillCircle(juce::Graphics& g, juce::Colour colour, float cx, float cy, float r)
        {
            g.setColour(colour);
            g.fillEllipse(cx - r, cy - r, r * 2.0F, r * 2.0F);
        }

        /// Hub + eight teeth as one path (nonzero winding unions the wedges with
        /// the hub disc), then the centre hole is punched by painting the badge
        /// colour back over it rather than fighting Path winding rules for a
        /// subtraction — simpler, and the badge is already known opaque.
        void paintGear(juce::Graphics& g, juce::Colour badge, juce::Colour glyph, float cx, float cy, float r)
        {
            juce::Path gear;
            const float rootR = r * GEAR_ROOT_RATIO;
            gear.addEllipse(cx - rootR, cy - rootR, rootR * 2.0F, rootR * 2.0F);
            for (int i = 0; i < GEAR_TEETH; ++i)
            {
                const float centre = juce::MathConstants<float>::twoPi * static_cast<float>(i) / GEAR_TEETH;
                gear.addPieSegment(cx - r, cy - r, r * 2.0F, r * 2.0F,
                                   centre - GEAR_TOOTH_HALF_ANGLE, centre + GEAR_TOOTH_HALF_ANGLE,
                                   rootR / r);
            }
            g.setColour(glyph);
            g.fillPath(gear);

            const float holeR = r * GEAR_HOLE_RATIO;
            fillCircle(g, badge, cx, cy, holeR);
        }

        /// A dot and a rounded stem — the classic "info" glyph, built rather
        /// than set in a font so it stays geometry a test can read, like every
        /// other icon in this app. [ADR-GUI-001 (DEC-GUI-001-A)]
        void paintInfo(juce::Graphics& g, juce::Colour glyph, float cx, float cy, float r)
        {
            const float dotR = r * 0.14F;
            fillCircle(g, glyph, cx, cy - r * 0.52F, dotR);

            const float stemW = r * 0.28F;
            const float stemTop = cy - r * 0.16F;
            const float stemH = r * 1.02F;
            g.setColour(glyph);
            g.fillRoundedRectangle(cx - stemW * 0.5F, stemTop, stemW, stemH, stemW * 0.5F);
        }
    }

    juce::Image dialogTitleBarIcon(DialogIcon icon, int pixelSize)
    {
        juce::Image image(juce::Image::ARGB, pixelSize, pixelSize, true);
        juce::Graphics g(image);

        const float size = static_cast<float>(pixelSize);
        const float cx = size * 0.5F;
        const float cy = size * 0.5F;
        const auto badge = tokens::component::dialogIconAccent;
        const auto glyph = tokens::semantic::textPrimary;

        fillCircle(g, badge, cx, cy, size * 0.5F);

        const float r = size * GLYPH_RADIUS_RATIO;
        switch (icon)
        {
            case DialogIcon::Settings:
                paintGear(g, badge, glyph, cx, cy, r);
                break;
            case DialogIcon::About:
                paintInfo(g, glyph, cx, cy, r);
                break;
        }

        return image;
    }

    void applyDialogTitleBarIcon(juce::DocumentWindow& window, DialogIcon icon)
    {
        const auto image = dialogTitleBarIcon(icon, tokens::component::dialogIconSize);

        // The JUCE-drawn title bar. Inert while useNativeTitleBar is on, kept so
        // the glyph follows if that ever changes. See the header for why this
        // call alone was not enough. [RQ-GUI-072]
        window.setIcon(image);

        // The OS chrome — this is the one that shows. The window is on the
        // desktop by now (launchAsync() has made it visible) so the peer exists;
        // asserted rather than quietly skipped, because an icon that silently
        // fails to appear is exactly the defect being fixed here.
        auto* peer = window.getPeer();
        jassert(peer != nullptr);
        if (peer != nullptr)
        {
            peer->setIcon(image);
        }
    }
}
