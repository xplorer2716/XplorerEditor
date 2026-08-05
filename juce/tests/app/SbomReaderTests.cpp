#include <catch2/catch_test_macros.hpp>

#include "SbomReader.hpp"

// The SPDX reader behind About > Dependencies: field mapping and its fallbacks,
// SPDX no-value sentinels, documentDescribes filtering, alphabetical ordering,
// and every failure result.
//
// Fixtures are written to temporary files by the tests themselves and deleted
// with them: no SBOM fixture is committed (owner instruction), so none can drift
// away from the case it claims to cover.
// [RQ-GUI-057; ADR-ABT-001 (DEC-ABT-003, DEC-ABT-004, DEC-ABT-005, DEC-ABT-006)]

using namespace xplorer::app;

namespace
{
    /** A temp .json file carrying `content`, removed when the test leaves scope. */
    class TemporarySbom final
    {
    public:
        explicit TemporarySbom(const juce::String& content)
            : _file(juce::File::createTempFile(".spdx.json"))
        {
            _file.replaceWithText(content);
        }

        ~TemporarySbom() { _file.deleteFile(); }

        TemporarySbom(const TemporarySbom&) = delete;
        TemporarySbom& operator=(const TemporarySbom&) = delete;

        [[nodiscard]] const juce::File& file() const { return _file; }

    private:
        juce::File _file;
    };

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
        const TemporarySbom sbom{spdxDocument(
            R"([{"SPDXID":"SPDXRef-z","name":"Zlib","versionInfo":"1.3"},)"
            R"({"SPDXID":"SPDXRef-c","name":"catch2","versionInfo":"3.9.1"},)"
            R"({"SPDXID":"SPDXRef-j","name":"JUCE","versionInfo":"8.0.9"}])")};

        WHEN("it is read")
        {
            const auto result = readSbom(sbom.file());

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
        const TemporarySbom sbom{spdxDocument(
            R"([{"name":"Foo","licenseConcluded":"NOASSERTION","licenseDeclared":"MIT"}])")};

        WHEN("it is read")
        {
            const auto result = readSbom(sbom.file());

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
        const TemporarySbom sbom{spdxDocument(
            R"([{"name":"Foo","licenseConcluded":"NOASSERTION","licenseDeclared":"NONE"}])")};

        WHEN("it is read")
        {
            const auto result = readSbom(sbom.file());

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
        const TemporarySbom sbom{spdxDocument(
            R"([{"name":"Foo","downloadLocation":"https://example.com/foo"}])")};

        WHEN("it is read")
        {
            const auto result = readSbom(sbom.file());

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
        const TemporarySbom sbom{spdxDocument(
            R"([{"name":"Foo","homepage":"https://foo.example","downloadLocation":"https://dl.example"}])")};

        WHEN("it is read")
        {
            const auto result = readSbom(sbom.file());

            THEN("the homepage wins")
            {
                REQUIRE(result.entries.size() == 1);
                CHECK(result.entries[0].website == "https://foo.example");
            }
        }
    }

    GIVEN("a package whose download location is the NOASSERTION sentinel")
    {
        const TemporarySbom sbom{spdxDocument(
            R"([{"name":"Foo","downloadLocation":"NOASSERTION"}])")};

        WHEN("it is read")
        {
            const auto result = readSbom(sbom.file());

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
        const TemporarySbom sbom{spdxDocument(
            R"([{"SPDXID":"SPDXRef-Package-Xplorer","name":"Xplorer","versionInfo":"0.1.0"},)"
            R"({"SPDXID":"SPDXRef-Package-JUCE","name":"JUCE","versionInfo":"8.0.9"}])",
            R"(["SPDXRef-Package-Xplorer"])")};

        WHEN("it is read")
        {
            const auto result = readSbom(sbom.file());

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
        const TemporarySbom sbom{spdxDocument(
            R"([{"SPDXID":"SPDXRef-Package-Xplorer","name":"Xplorer"}])",
            R"(["SPDXRef-Package-Xplorer"])")};

        WHEN("it is read")
        {
            THEN("it reports having no dependency information, not an empty success")
            {
                CHECK(readSbom(sbom.file()).status == SbomStatus::NotSpdxOrEmpty);
            }
        }
    }
}

SCENARIO("An unusable SBOM is reported, never silently substituted", "[RQ-GUI-057]")
{
    GIVEN("a path where no file exists")
    {
        const auto missing = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                 .getChildFile("xplorer-no-such-sbom.spdx.json");
        REQUIRE(!missing.existsAsFile());

        WHEN("it is read")
        {
            const auto result = readSbom(missing);

            THEN("the absence is reported and no entry is invented")
            {
                CHECK(result.status == SbomStatus::FileNotFound);
                CHECK(result.entries.empty());
            }
        }
    }

    GIVEN("a file containing malformed JSON")
    {
        const TemporarySbom sbom{R"({"spdxVersion":"SPDX-2.3","packages":[)"};

        WHEN("it is read")
        {
            const auto result = readSbom(sbom.file());

            THEN("it is reported as unparseable")
            {
                CHECK(result.status == SbomStatus::InvalidJson);
                CHECK(result.entries.empty());
            }
        }
    }

    GIVEN("valid JSON that is not an SPDX document")
    {
        const TemporarySbom sbom{R"({"hello":"world"})"};

        WHEN("it is read")
        {
            THEN("it is reported as not being an SPDX document")
            {
                CHECK(readSbom(sbom.file()).status == SbomStatus::NotSpdxOrEmpty);
            }
        }
    }

    GIVEN("an SPDX document with an empty package list")
    {
        const TemporarySbom sbom{spdxDocument("[]")};

        WHEN("it is read")
        {
            THEN("it is reported as carrying no dependency information")
            {
                CHECK(readSbom(sbom.file()).status == SbomStatus::NotSpdxOrEmpty);
            }
        }
    }

    GIVEN("an SPDX document whose package list holds no named package")
    {
        const TemporarySbom sbom{spdxDocument(R"([{"SPDXID":"SPDXRef-x","versionInfo":"1.0"}])")};

        WHEN("it is read")
        {
            THEN("it is reported as carrying no dependency information")
            {
                CHECK(readSbom(sbom.file()).status == SbomStatus::NotSpdxOrEmpty);
            }
        }
    }
}

SCENARIO("The shipped SBOM is looked for beside the executable", "[RQ-BLD-014]")
{
    GIVEN("the running executable")
    {
        WHEN("the default SBOM location is resolved")
        {
            const auto sbom = defaultSbomFile();

            THEN("it is the agreed file name, in the executable's own directory")
            {
                CHECK(sbom.getFileName() == "xplorer.sbom.spdx.json");
                CHECK(sbom.getParentDirectory()
                      == juce::File::getSpecialLocation(juce::File::currentExecutableFile)
                             .getParentDirectory());
            }
        }
    }
}
