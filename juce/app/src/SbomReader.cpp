#include "SbomReader.hpp"

#include <algorithm>
#include <utility>

namespace xplorer::app
{
    namespace
    {
        // Fixed by RQ-BLD-014 so the application and whatever produces the
        // document agree without configuration. [DEC-ABT-002]
        constexpr const char* SBOM_FILE_NAME = "xplorer.sbom.spdx.json";

        // SPDX encodes "no value" as these two string literals rather than by
        // omitting the field. Treated as absent at this boundary so they can
        // never reach the screen as text. [DEC-ABT-003]
        constexpr const char* SPDX_NO_ASSERTION = "NOASSERTION";
        constexpr const char* SPDX_NONE = "NONE";

        constexpr const char* KEY_SPDX_VERSION = "spdxVersion";
        constexpr const char* KEY_PACKAGES = "packages";
        constexpr const char* KEY_DOCUMENT_DESCRIBES = "documentDescribes";
        constexpr const char* KEY_SPDX_ID = "SPDXID";
        constexpr const char* KEY_NAME = "name";
        constexpr const char* KEY_VERSION_INFO = "versionInfo";
        constexpr const char* KEY_LICENSE_CONCLUDED = "licenseConcluded";
        constexpr const char* KEY_LICENSE_DECLARED = "licenseDeclared";
        constexpr const char* KEY_HOMEPAGE = "homepage";
        constexpr const char* KEY_DOWNLOAD_LOCATION = "downloadLocation";

        // Returned by value: var::getProperty hands back a reference to its own
        // default argument when the key is missing, which would dangle if the
        // caller held on to it past the full expression.
        [[nodiscard]] juce::var propertyOf(const juce::var& object, const char* key)
        {
            return object.getProperty(juce::Identifier{key}, juce::var{});
        }

        /** A field's value when it carries one -- empty when the key is absent,
            blank, or set to an SPDX no-value sentinel. [DEC-ABT-003] */
        [[nodiscard]] juce::String meaningfulString(const juce::var& object, const char* key)
        {
            const auto value = propertyOf(object, key).toString().trim();
            if (value == SPDX_NO_ASSERTION || value == SPDX_NONE)
            {
                return {};
            }
            return value;
        }

        /** The first of two keys carrying a meaningful value -- the licence and
            website fallbacks of DEC-ABT-003 (concluded before declared, homepage
            before download location). */
        [[nodiscard]] juce::String firstMeaningfulString(const juce::var& object, const char* preferredKey,
                                                         const char* fallbackKey)
        {
            auto value = meaningfulString(object, preferredKey);
            if (value.isEmpty())
            {
                value = meaningfulString(object, fallbackKey);
            }
            return value;
        }

        /** SPDXIDs the document names as its own subject -- the product itself,
            which is not one of its dependencies. [DEC-ABT-004] */
        [[nodiscard]] juce::StringArray describedPackageIds(const juce::var& document)
        {
            juce::StringArray ids;
            const auto describes = propertyOf(document, KEY_DOCUMENT_DESCRIBES);
            if (const auto* array = describes.getArray())
            {
                for (const auto& id : *array)
                {
                    ids.add(id.toString());
                }
            }
            return ids;
        }
    }

    SbomResult readSbom(const juce::File& sbomFile)
    {
        if (!sbomFile.existsAsFile())
        {
            return {SbomStatus::FileNotFound, {}};
        }

        juce::FileInputStream stream{sbomFile};
        if (!stream.openedOk())
        {
            return {SbomStatus::Unreadable, {}};
        }

        juce::var document;
        if (juce::JSON::parse(stream.readEntireStreamAsString(), document).failed())
        {
            return {SbomStatus::InvalidJson, {}};
        }

        // Parseable, but is it the kind of document we were promised? Anything
        // without an SPDX version and a package array is reported as such rather
        // than rendered as an empty list. [DEC-ABT-005]
        const auto packages = propertyOf(document, KEY_PACKAGES);
        if (!document.hasProperty(juce::Identifier{KEY_SPDX_VERSION}) || !packages.isArray())
        {
            return {SbomStatus::NotSpdxOrEmpty, {}};
        }

        const auto described = describedPackageIds(document);

        std::vector<SbomEntry> entries;
        for (const auto& package : *packages.getArray())
        {
            const auto id = propertyOf(package, KEY_SPDX_ID).toString();
            if (id.isNotEmpty() && described.contains(id))
            {
                continue;
            }

            SbomEntry entry;
            entry.name = meaningfulString(package, KEY_NAME);
            // A nameless package can be neither displayed nor ordered, and the
            // name is the one field SPDX makes mandatory -- so its absence means
            // this array element is not a package description at all.
            if (entry.name.isEmpty())
            {
                continue;
            }
            entry.version = meaningfulString(package, KEY_VERSION_INFO);
            entry.license = firstMeaningfulString(package, KEY_LICENSE_CONCLUDED, KEY_LICENSE_DECLARED);
            entry.website = firstMeaningfulString(package, KEY_HOMEPAGE, KEY_DOWNLOAD_LOCATION);
            entries.push_back(std::move(entry));
        }

        if (entries.empty())
        {
            return {SbomStatus::NotSpdxOrEmpty, {}};
        }

        // Alphabetical regardless of document order: the file's order reflects
        // how it was generated, which is not something the reader should expose.
        // [RQ-GUI-057]
        std::sort(entries.begin(), entries.end(), [](const SbomEntry& a, const SbomEntry& b) {
            return a.name.compareIgnoreCase(b.name) < 0;
        });

        return {SbomStatus::Loaded, std::move(entries)};
    }

    juce::File defaultSbomFile()
    {
        return juce::File::getSpecialLocation(juce::File::currentExecutableFile)
            .getSiblingFile(SBOM_FILE_NAME);
    }
}
