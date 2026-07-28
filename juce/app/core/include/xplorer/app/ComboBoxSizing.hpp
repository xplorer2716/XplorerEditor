#pragma once

// Combo-box font-sizing policy (UI-framework-free, headless-tested): decides
// the ONE font size shared by every combo box in the app, so the panel never
// shows several sizes at once (RQ-GUI-047, issue #12 follow-up), and reports
// the boxes that cannot fit their widest label at any legible size.
//
// Text measurement is INJECTED rather than performed here: measuring a string
// needs juce::Font, and this layer is deliberately JUCE-free (ADR-JUC-002) so
// it builds and is unit-tested by the always-run headless CI job. The JUCE
// side (XplorerLookAndFeel) supplies a real measurer; tests supply a fake one.
// [RQ-GUI-047, RQ-DSN-011, ADR-JUC-021 (DEC-JUC-041, DEC-JUC-042, DEC-JUC-043)]

#include <functional>
#include <string>
#include <string_view>
#include <vector>

namespace xplorer::app
{
    /// One combo box's contribution to the shared-size decision: how wide it
    /// is on the logical canvas, and every label it can display.
    struct ComboBoxSizingInput
    {
        std::string controlId;            ///< reference id, e.g. "VCF_MODE"
        int width = 0;                    ///< logical-canvas width from the control table
        std::vector<std::string> labels;  ///< the box's full item list
    };

    /// Every ComboBoxValuedControl of the control table paired with its label
    /// set. Controls whose id resolves to no label set are skipped: they
    /// cannot constrain a text size. [DEC-JUC-041]
    [[nodiscard]] std::vector<ComboBoxSizingInput> collectComboBoxSizingInputs();

    /// Measures a string's width, in the same units as ComboBoxSizingInput::
    /// width, at a given font size. Injected so this module stays JUCE-free.
    using TextWidthMeasurer = std::function<float(std::string_view text, float fontSize)>;

    struct SharedComboBoxFontSize
    {
        /// The size every combo box shall use, clamped to [minSize, baseSize].
        float sharedSize = 0.0F;
        /// Ids of the boxes whose own required size falls below minSize —
        /// i.e. that cannot show their widest label legibly at any size and
        /// must be widened instead (RQ-GUI-047's fallback). Reported from the
        /// UNCLAMPED per-box candidate, so it does not depend on what the
        /// shared size ends up being. [DEC-JUC-043]
        std::vector<std::string> idsBelowFloor;
    };

    /// The largest size at which every input's widest label still fits its own
    /// width: each box's candidate is baseSize shrunk by the ratio its widest
    /// label overflows by (the per-instance formula this replaces), and the
    /// shared size is the smallest of those, clamped to [minSize, baseSize].
    /// `arrowZone` + `labelMargin` are subtracted from each width to get the
    /// text area actually available. [DEC-JUC-042, DEC-JUC-043]
    [[nodiscard]] SharedComboBoxFontSize computeSharedComboBoxFontSize(
        const std::vector<ComboBoxSizingInput>& inputs, float baseSize, float minSize,
        int arrowZone, int labelMargin, const TextWidthMeasurer& measureWidth);
}
