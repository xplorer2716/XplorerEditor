#include <catch2/catch_test_macros.hpp>

#include "SbomReader.hpp"

#include <algorithm>

// The SPDX reader behind About > Dependencies: field mapping and its fallbacks,
// SPDX no-value sentinels, documentDescribes filtering, alphabetical ordering,
// and every failure result. Reads an in-memory juce::String directly -- no
// fixture file, committed or temporary -- since the production reader no
// longer touches the filesystem at all (ADR-BLD-005: the SBOM is embedded
// BinaryData, parsed straight from memory).
// [RQ-GUI-057; ADR-ABT-001 (DEC-ABT-003, DEC-ABT-004, DEC-ABT-005, DEC-ABT-006);
// ADR-BLD-005 (DEC-BLD-027, DEC-BLD-028)]

using namespace xplorer::app;

namespace
{
    /** A minimal but schema-shaped SPDX document wrapping `packagesJson`. */
    juce::String spdxDocument(const juce::String& packagesJson,
                              const juce::String& documentDescribesJson = "[]")
    {
        return R"({"spdxVersion":"SPDX-2.3","SPDXID":"SPDXRef-DOCUMENT","name":"test",)"
               "\"documentDescribes\":" + documentDescribesJson + ",\"packages\":" + packagesJson + "}";
    }
}

SCENARIO("The dependency list is read from an SPDX document", "[RQ-GUI-057]")
{
    GIVEN("a document whose packages are not in alphabetical order")
    {
        const auto sbom = spdxDocument(
            R"([{"SPDXID":"SPDXRef-z","name":"Zlib","versionInfo":"1.3"},)"
            R"({"SPDXID":"SPDXRef-c","name":"catch2","versionInfo":"3.9.1"},)"
            R"({"SPDXID":"SPDXRef-j","name":"JUCE","versionInfo":"8.0.9"}])");

        WHEN("it is read")
        {
            const auto result = readSbom(sbom);

            THEN("every package is returned, ordered alphabetically and case-insensitively")
            {
                REQUIRE(result.status == SbomStatus::Loaded);
                REQUIRE(result.entries.size() == 3);
                CHECK(result.entries[0].name == "catch2");
                CHECK(result.entries[1].name == "JUCE");
                CHECK(result.entries[2].name == "Zlib");
            }

            THEN("each entry carries its own version")
            {
                REQUIRE(result.entries.size() == 3);
                CHECK(result.entries[1].version == "8.0.9");
            }
        }
    }

    GIVEN("a package whose concluded licence is NOASSERTION but whose declared licence is set")
    {
        const auto sbom = spdxDocument(
            R"([{"name":"Foo","licenseConcluded":"NOASSERTION","licenseDeclared":"MIT"}])");

        WHEN("it is read")
        {
            const auto result = readSbom(sbom);

            THEN("the declared licence is used")
            {
                REQUIRE(result.status == SbomStatus::Loaded);
                REQUIRE(result.entries.size() == 1);
                CHECK(result.entries[0].license == "MIT");
            }
        }
    }

    GIVEN("a package whose every licence field is an SPDX no-value sentinel")
    {
        const auto sbom = spdxDocument(
            R"([{"name":"Foo","licenseConcluded":"NOASSERTION","licenseDeclared":"NONE"}])");

        WHEN("it is read")
        {
            const auto result = readSbom(sbom);

            THEN("its licence is empty -- the sentinel never becomes display text")
            {
                REQUIRE(result.status == SbomStatus::Loaded);
                REQUIRE(result.entries.size() == 1);
                CHECK(result.entries[0].license.isEmpty());
            }
        }
    }

    GIVEN("a package with no homepage but a usable download location")
    {
        const auto sbom = spdxDocument(
            R"([{"name":"Foo","downloadLocation":"https://example.com/foo"}])");

        WHEN("it is read")
        {
            const auto result = readSbom(sbom);

            THEN("the download location becomes its website")
            {
                REQUIRE(result.status == SbomStatus::Loaded);
                REQUIRE(result.entries.size() == 1);
                CHECK(result.entries[0].website == "https://example.com/foo");
            }
        }
    }

    GIVEN("a package with a homepage and a different download location")
    {
        const auto sbom = spdxDocument(
            R"([{"name":"Foo","homepage":"https://foo.example","downloadLocation":"https://dl.example"}])");

        WHEN("it is read")
        {
            const auto result = readSbom(sbom);

            THEN("the homepage wins")
            {
                REQUIRE(result.entries.size() == 1);
                CHECK(result.entries[0].website == "https://foo.example");
            }
        }
    }

    GIVEN("a package whose download location is the NOASSERTION sentinel")
    {
        const auto sbom = spdxDocument(R"([{"name":"Foo","downloadLocation":"NOASSERTION"}])");

        WHEN("it is read")
        {
            const auto result = readSbom(sbom);

            THEN("its website is empty rather than the sentinel")
            {
                REQUIRE(result.entries.size() == 1);
                CHECK(result.entries[0].website.isEmpty());
            }
        }
    }
}

SCENARIO("The document's own subject is not one of its dependencies", "[RQ-GUI-057]")
{
    GIVEN("a document describing itself, alongside a real dependency")
    {
        const auto sbom = spdxDocument(
            R"([{"SPDXID":"SPDXRef-Package-Xplorer","name":"Xplorer","versionInfo":"0.1.0"},)"
            R"({"SPDXID":"SPDXRef-Package-JUCE","name":"JUCE","versionInfo":"8.0.9"}])",
            R"(["SPDXRef-Package-Xplorer"])");

        WHEN("it is read")
        {
            const auto result = readSbom(sbom);

            THEN("the described package is filtered out and the dependency remains")
            {
                REQUIRE(result.status == SbomStatus::Loaded);
                REQUIRE(result.entries.size() == 1);
                CHECK(result.entries[0].name == "JUCE");
            }
        }
    }

    GIVEN("a document whose only package is the one it describes")
    {
        const auto sbom = spdxDocument(
            R"([{"SPDXID":"SPDXRef-Package-Xplorer","name":"Xplorer"}])",
            R"(["SPDXRef-Package-Xplorer"])");

        WHEN("it is read")
        {
            THEN("it reports having no dependency information, not an empty success")
            {
                CHECK(readSbom(sbom).status == SbomStatus::NotSpdxOrEmpty);
            }
        }
    }
}

SCENARIO("An unusable SBOM is reported, never silently substituted", "[RQ-GUI-057]")
{
    GIVEN("malformed JSON")
    {
        const juce::String sbom = R"({"spdxVersion":"SPDX-2.3","packages":[)";

        WHEN("it is read")
        {
            const auto result = readSbom(sbom);

            THEN("it is reported as unparseable")
            {
                CHECK(result.status == SbomStatus::InvalidJson);
                CHECK(result.entries.empty());
            }
        }
    }

    GIVEN("valid JSON that is not an SPDX document")
    {
        const juce::String sbom = R"({"hello":"world"})";

        WHEN("it is read")
        {
            THEN("it is reported as not being an SPDX document")
            {
                CHECK(readSbom(sbom).status == SbomStatus::NotSpdxOrEmpty);
            }
        }
    }

    GIVEN("an SPDX document with an empty package list")
    {
        const auto sbom = spdxDocument("[]");

        WHEN("it is read")
        {
            THEN("it is reported as carrying no dependency information")
            {
                CHECK(readSbom(sbom).status == SbomStatus::NotSpdxOrEmpty);
            }
        }
    }

    GIVEN("an SPDX document whose package list holds no named package")
    {
        const auto sbom = spdxDocument(R"([{"SPDXID":"SPDXRef-x","versionInfo":"1.0"}])");

        WHEN("it is read")
        {
            THEN("it is reported as carrying no dependency information")
            {
                CHECK(readSbom(sbom).status == SbomStatus::NotSpdxOrEmpty);
            }
        }
    }
}

SCENARIO("The embedded SBOM the build itself produced is readable with no file at all",
         "[RQ-GUI-057][ADR-BLD-005]")
{
    GIVEN("the executable's own embedded BinaryData")
    {
        WHEN("it is read")
        {
            const auto result = readEmbeddedSbom();

            THEN("it loads, and the component this build always ships is present")
            {
                REQUIRE(result.status == SbomStatus::Loaded);
                const auto hasJuce = std::any_of(result.entries.begin(), result.entries.end(),
                                                 [](const SbomEntry& e) { return e.name == "JUCE"; });
                CHECK(hasJuce);
            }
        }
    }
}
