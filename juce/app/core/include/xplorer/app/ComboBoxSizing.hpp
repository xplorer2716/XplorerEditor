#pragma once

// Combo-box fit verification (UI-framework-free, headless-tested).
//
// Both the text size and the control widths are settled design values
// (RQ-GUI-047): one fixed size for every combo box, widths grouped by displayed
// value list. Nothing is computed here — this module answers the one question
// that keeps those values honest as the code evolves: does every label of every
// value list still fit its control? Its answer feeds the headless tests and the
// development-build assertion of RQ-GUI-048.
//
// Text measurement is INJECTED rather than performed here: measuring a string
// needs juce::Font, and this layer is deliberately JUCE-free (ADR-JUC-002) so
// it builds and is unit-tested by the always-run headless CI job. The JUCE
// side (XplorerLookAndFeel) supplies a real measurer; tests supply a fake one.
// [RQ-GUI-047, RQ-GUI-048, RQ-DSN-096, ADR-JUC-022 (DEC-JUC-050)]

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
    /// cannot constrain a text size. [ADR-JUC-022 (DEC-JUC-050)]
    [[nodiscard]] std::vector<ComboBoxSizingInput> collectComboBoxSizingInputs();

    /// Measures a string's width, in the same units as ComboBoxSizingInput::
    /// width, at a given font size. Injected so this module stays JUCE-free.
    using TextWidthMeasurer = std::function<float(std::string_view text, float fontSize)>;

    /// Ids of the combo boxes whose widest label does NOT fit at `fontSize`.
    /// Empty means every label of every value list fits its control.
    ///
    /// Nothing here computes a size or a width: both are settled design values
    /// (RQ-GUI-047). This is the verification side of that decision, shared by
    /// the headless tests and the development-build assertion (RQ-GUI-048), so
    /// a later label, width or typeface edit fails loudly instead of silently
    /// ellipsizing. `arrowZone` + 2*`labelBorder` is the overhead subtracted
    /// from each control's width to get the text room actually available.
    /// [RQ-GUI-047, RQ-GUI-048, ADR-JUC-022 (DEC-JUC-050)]
    [[nodiscard]] std::vector<std::string> comboBoxesOverflowingAt(
        const std::vector<ComboBoxSizingInput>& inputs, float fontSize, int arrowZone,
        int labelBorder, const TextWidthMeasurer& measureWidth);
}
