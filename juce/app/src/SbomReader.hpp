#pragma once

// Reader for the SPDX Software Bill of Materials embedded in the executable as
// BinaryData. Turns that document into the list the About > Dependencies window
// displays -- and into an explicit reason when it cannot.
//
// This unit is deliberately free of any juce::Component: it is pure data in,
// pure data out, so every rule below (field fallbacks, SPDX no-value sentinels,
// documentDescribes filtering, ordering) is testable without a display.
// [RQ-GUI-057, RQ-BLD-014; ADR-ABT-001 (DEC-ABT-003, DEC-ABT-004, DEC-ABT-005,
// DEC-ABT-006); ADR-BLD-005 (DEC-BLD-027, DEC-BLD-028)]

#include <juce_core/juce_core.h>

#include <vector>

namespace xplorer::app
{
    /** One third-party component, as the window shows it. Any field the SBOM
        leaves absent -- or marks with an SPDX no-value sentinel -- arrives here
        as an empty string, never as the sentinel text. [DEC-ABT-003] */
    struct SbomEntry
    {
        juce::String name;
        juce::String version;
        juce::String license;
        juce::String website;
    };

    /** Why a read produced no list. Every failure is a value the window renders
        as an explanation -- there is no assertion, no silent empty list and no
        built-in fallback inventory, which would reinstate exactly the hard-coded
        list this feature exists to abolish. Embedding the document (ADR-BLD-005)
        makes it impossible to be missing or unreadable, so only the content
        itself can still be malformed. [DEC-ABT-005] */
    enum class SbomStatus
    {
        Loaded,          ///< At least one dependency was read.
        InvalidJson,     ///< Read, but not parseable as JSON.
        NotSpdxOrEmpty   ///< Valid JSON, but not an SPDX document with dependencies.
    };

    struct SbomResult
    {
        SbomStatus status = SbomStatus::NotSpdxOrEmpty;
        /** Non-empty only when `status == Loaded`; sorted by name,
            case-insensitively, whatever order the document used. */
        std::vector<SbomEntry> entries;
    };

    /** Parses an SPDX 2.3 JSON document already held in memory. Never throws;
        every failure mode is reported through `SbomResult::status`.
        [RQ-GUI-057] */
    [[nodiscard]] SbomResult readSbom(const juce::String& jsonText);

    /** The SBOM the running executable was built with, read straight from its
        own BinaryData -- no file, no path, nothing that can go missing between
        build and launch. [ADR-BLD-005 (DEC-BLD-028)] */
    [[nodiscard]] SbomResult readEmbeddedSbom();
}
