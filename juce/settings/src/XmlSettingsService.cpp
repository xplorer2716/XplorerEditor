// XML persistence schema-compatible with the .NET XmlSerializer output of
// the reference AllUsersSettingsService, so existing xplorer.users.config
// files import unchanged. [RQ-SET-001, RQ-SET-004, RQ-SET-006]
#include "xplorer/settings/SettingsService.hpp"

#include <juce_core/juce_core.h>

#include <array>
#include <optional>
#include <utility>

namespace xplorer::settings
{
    using namespace xplorer::model;

    namespace
    {
        // --- enum <-> .NET name tables (XmlSerializer writes enum names;
        //     [Flags] values are space-separated) ---------------------------

        template <typename Enum>
        struct EnumName
        {
            Enum value;
            const char* name;
        };

        constexpr auto VCO2_NAMES = std::to_array<EnumName<EnumRandomVCO2>>({
            {EnumRandomVCO2::EnableFM, "EnableFM"},
            {EnumRandomVCO2::EnableNoise, "EnableNoise"},
            {EnumRandomVCO2::EnableSync, "EnableSync"},
        });
        constexpr auto VCOFREQ_NAMES = std::to_array<EnumName<EnumRandomVCOFreq>>({
            {EnumRandomVCOFreq::Free, "Free"}, {EnumRandomVCOFreq::SameNote, "SameNote"},
            {EnumRandomVCOFreq::Third, "Third"}, {EnumRandomVCOFreq::Fifth, "Fifth"},
            {EnumRandomVCOFreq::Seventh, "Seventh"}, {EnumRandomVCOFreq::Octave, "Octave"},
            {EnumRandomVCOFreq::Ninth, "Ninth"}, {EnumRandomVCOFreq::Eleventh, "Eleventh"},
            {EnumRandomVCOFreq::Thirteenth, "Thirteenth"},
        });
        constexpr auto VCODETUNE_NAMES = std::to_array<EnumName<EnumRandomVCODetune>>({
            {EnumRandomVCODetune::Free, "Free"},
            {EnumRandomVCODetune::Digital, "Digital"},
            {EnumRandomVCODetune::Analog, "Analog"},
        });
        constexpr auto VCAENV_NAMES = std::to_array<EnumName<EnumRandomVCAEnv>>({
            {EnumRandomVCAEnv::Free, "Free"}, {EnumRandomVCAEnv::Organ, "Organ"},
            {EnumRandomVCAEnv::String, "String"}, {EnumRandomVCAEnv::Percusive, "Percusive"},
            {EnumRandomVCAEnv::PercusiveWithRelease, "PercusiveWithRelease"},
        });
        constexpr auto MODMATRIX_NAMES = std::to_array<EnumName<EnumRandomModMatrix>>({
            {EnumRandomModMatrix::EnableAmount, "EnableAmount"},
            {EnumRandomModMatrix::EnableSourcesAndDestinations, "EnableSourcesAndDestinations"},
            {EnumRandomModMatrix::EnableQuantize, "EnableQuantize"},
        });

        template <typename Enum, std::size_t N>
        std::optional<Enum> parseEnum(const std::array<EnumName<Enum>, N>& table, const juce::String& text)
        {
            for (const auto& entry : table)
            {
                if (text == entry.name)
                {
                    return entry.value;
                }
            }
            return std::nullopt;
        }

        template <typename Enum, std::size_t N>
        juce::String enumToString(const std::array<EnumName<Enum>, N>& table, Enum value)
        {
            for (const auto& entry : table)
            {
                if (value == entry.value)
                {
                    return entry.name;
                }
            }
            return {};
        }

        template <typename Enum, std::size_t N>
        std::optional<Enum> parseFlags(const std::array<EnumName<Enum>, N>& table, const juce::String& text)
        {
            int combined = 0;
            auto tokens = juce::StringArray::fromTokens(text, " ", "");
            tokens.removeEmptyStrings();
            for (const auto& token : tokens)
            {
                const auto value = parseEnum(table, token);
                if (!value.has_value())
                {
                    return std::nullopt;
                }
                combined |= static_cast<int>(*value);
            }
            return static_cast<Enum>(combined);
        }

        template <typename Enum, std::size_t N>
        juce::String flagsToString(const std::array<EnumName<Enum>, N>& table, Enum value)
        {
            juce::StringArray names;
            for (const auto& entry : table)
            {
                if ((static_cast<int>(value) & static_cast<int>(entry.value)) != 0)
                {
                    names.add(entry.name);
                }
            }
            return names.joinIntoString(" ");
        }

        // Per-block colour-override element names, indexed like
        // UiConfiguration::blockColours (BlockId order). Only set entries are
        // written; a missing element reads back as "unset". [RQ-SET-007,
        // ADR-JUC-020 (DEC-JUC-039)]
        constexpr std::array<const char*, AllUsersSettings::UiConfiguration::BLOCK_COLOUR_COUNT> BLOCK_COLOUR_ELEMENTS{
            "BlockColorVco", "BlockColorLag", "BlockColorTrack", "BlockColorVcf",
            "BlockColorEnv", "BlockColorLfo", "BlockColorRamp", "BlockColorMatrix",
        };

        // --- element helpers ----------------------------------------------
        //
        // All four return std::optional, and the empty case means "the element
        // is not in the file" -- never "the value is zero". That distinction is
        // what makes the schema tolerant in both directions: a file written by
        // an older version simply lacks the newer elements and every reader
        // falls back to its default (see the value_or() calls in
        // parseSettings), while an element this version no longer reads is
        // ignored rather than rejected. Both are relied upon -- by settings
        // files predating a feature, and by files imported from the archived
        // .NET implementation. [RQ-SET-006, RQ-SET-007]

        std::optional<juce::String> childText(const juce::XmlElement& parent, const char* name)
        {
            const auto* child = parent.getChildByName(name);
            return child == nullptr ? std::nullopt
                                    : std::make_optional(child->getAllSubText());
        }

        std::optional<int> childInt(const juce::XmlElement& parent, const char* name)
        {
            const auto text = childText(parent, name);
            return text.has_value() ? std::make_optional(text->getIntValue()) : std::nullopt;
        }

        // Case-sensitive comparison against "true" on purpose: that is exactly
        // what .NET's XmlSerializer emits, and anything else -- including
        // "True" -- is therefore treated as false rather than silently
        // accepted. addChildText below writes the same lowercase form back.
        std::optional<bool> childBool(const juce::XmlElement& parent, const char* name)
        {
            const auto text = childText(parent, name);
            if (!text.has_value())
            {
                return std::nullopt;
            }
            return *text == "true";
        }

        void addChildText(juce::XmlElement& parent, const char* name, const juce::String& value)
        {
            auto* child = parent.createNewChildElement(name);
            child->addTextElement(value);
        }

        // Prefers the per-machine directory (ProgramData on Windows), but
        // falls back to the per-user one when it cannot be created — Linux
        // (/opt) and macOS (/Library) are root-owned and this project ships
        // no installer to grant a standard user write access there. Reuses
        // the same createDirectory() call save() makes, so the check and the
        // later write can never disagree. [RQ-SET-001, ADR-SET-001 (DEC-SET-001)]
        juce::File resolveSettingsDirectory(const std::string& preferredDirectory,
                                             const std::string& fallbackDirectory)
        {
            juce::File preferred{juce::String(preferredDirectory)};
            if (fallbackDirectory.empty() || preferred.createDirectory().wasOk())
            {
                return preferred;
            }
            return juce::File(juce::String(fallbackDirectory));
        }

        // --- load ----------------------------------------------------------

        std::optional<AllUsersSettings> parseSettings(const juce::XmlElement& root)
        {
            // Reference: a file whose sections are missing (legacy version)
            // is rejected and replaced by defaults.
            const auto* midi = root.getChildByName("MidiConfig");
            const auto* ui = root.getChildByName("UiConfig");
            const auto* random = root.getChildByName("RandomizerConfig");
            if (midi == nullptr || ui == nullptr || random == nullptr)
            {
                return std::nullopt;
            }

            AllUsersSettings settings;
            auto& midiConfig = settings.midiConfig;
            midiConfig.automationInputDeviceName = childText(*midi, "AutomationInputDeviceName").value_or("").toStdString();
            midiConfig.synthInputDeviceName = childText(*midi, "SynthInputDeviceName").value_or("").toStdString();
            midiConfig.synthOutputDeviceName = childText(*midi, "SynthOutputDeviceName").value_or("").toStdString();
            midiConfig.sysexTransmitDelay = childInt(*midi, "SysexTransmitDelay").value_or(0);
            midiConfig.midiChannel = childInt(*midi, "MidiChannel").value_or(0);
            midiConfig.editingProgramNumber = childInt(*midi, "EditingProgramNumber").value_or(0);
            midiConfig.smartAllNotesOff = childBool(*midi, "SmartAllNotesOff").value_or(false);
            midiConfig.synthTypeIsMatrix12 = childBool(*midi, "SynthTypeIsMatrix12").value_or(false);
            if (const auto* table = midi->getChildByName("AutomationTable"))
            {
                for (const auto* entry : table->getChildWithTagNameIterator("string"))
                {
                    midiConfig.automationTable.push_back(entry->getAllSubText().toStdString());
                }
            }

            settings.uiConfig.knobLedBorderColor = childInt(*ui, "KnobLedBorderColor").value_or(0);
            for (std::size_t i = 0; i < AllUsersSettings::UiConfiguration::BLOCK_COLOUR_COUNT; ++i)
            {
                settings.uiConfig.blockColours[i] = childInt(*ui, BLOCK_COLOUR_ELEMENTS[i]);
            }

            auto& randomConfig = settings.randomizerConfig;
            const auto vco2 = parseFlags(VCO2_NAMES, childText(*random, "VCO2FmNoiseSync").value_or(""));
            const auto freq = parseEnum(VCOFREQ_NAMES, childText(*random, "VCOFreq").value_or("Free"));
            const auto detune = parseEnum(VCODETUNE_NAMES, childText(*random, "VCODetune").value_or("Free"));
            const auto env = parseEnum(VCAENV_NAMES, childText(*random, "VCA2Env").value_or("Free"));
            const auto matrix = parseFlags(MODMATRIX_NAMES, childText(*random, "ModulationMatrix").value_or(""));
            if (!vco2 || !freq || !detune || !env || !matrix)
            {
                return std::nullopt; // .NET deserialization would throw
            }
            randomConfig.vco2FmNoiseSync = *vco2;
            randomConfig.vcoFreq = *freq;
            randomConfig.vcoDetune = *detune;
            randomConfig.vca2Env = *env;
            randomConfig.modulationMatrix = *matrix;

            return settings;
        }

        // --- save ------------------------------------------------------------

        std::unique_ptr<juce::XmlElement> settingsToXml(const AllUsersSettings& settings)
        {
            auto root = std::make_unique<juce::XmlElement>("AllUsersSettings");
            root->setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
            root->setAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");

            auto* midi = root->createNewChildElement("MidiConfig");
            const auto& midiConfig = settings.midiConfig;
            addChildText(*midi, "AutomationInputDeviceName", midiConfig.automationInputDeviceName);
            addChildText(*midi, "SynthInputDeviceName", midiConfig.synthInputDeviceName);
            addChildText(*midi, "SynthOutputDeviceName", midiConfig.synthOutputDeviceName);
            addChildText(*midi, "SysexTransmitDelay", juce::String(midiConfig.sysexTransmitDelay));
            addChildText(*midi, "MidiChannel", juce::String(midiConfig.midiChannel));
            addChildText(*midi, "EditingProgramNumber", juce::String(midiConfig.editingProgramNumber));
            addChildText(*midi, "SmartAllNotesOff", midiConfig.smartAllNotesOff ? "true" : "false");
            addChildText(*midi, "SynthTypeIsMatrix12", midiConfig.synthTypeIsMatrix12 ? "true" : "false");
            auto* table = midi->createNewChildElement("AutomationTable");
            for (const auto& entry : midiConfig.automationTable)
            {
                addChildText(*table, "string", entry);
            }

            auto* ui = root->createNewChildElement("UiConfig");
            addChildText(*ui, "KnobLedBorderColor", juce::String(settings.uiConfig.knobLedBorderColor));
            for (std::size_t i = 0; i < AllUsersSettings::UiConfiguration::BLOCK_COLOUR_COUNT; ++i)
            {
                if (settings.uiConfig.blockColours[i].has_value())
                {
                    addChildText(*ui, BLOCK_COLOUR_ELEMENTS[i], juce::String(*settings.uiConfig.blockColours[i]));
                }
            }

            auto* random = root->createNewChildElement("RandomizerConfig");
            const auto& randomConfig = settings.randomizerConfig;
            addChildText(*random, "VCO2FmNoiseSync", flagsToString(VCO2_NAMES, randomConfig.vco2FmNoiseSync));
            addChildText(*random, "VCOFreq", enumToString(VCOFREQ_NAMES, randomConfig.vcoFreq));
            addChildText(*random, "VCODetune", enumToString(VCODETUNE_NAMES, randomConfig.vcoDetune));
            addChildText(*random, "VCA2Env", enumToString(VCAENV_NAMES, randomConfig.vca2Env));
            addChildText(*random, "ModulationMatrix", flagsToString(MODMATRIX_NAMES, randomConfig.modulationMatrix));

            return root;
        }
    }

    // Pimpl, so juce::File and juce::XmlElement stay out of the public header
    // and the settings library imposes no JUCE dependency on its consumers.
    //
    // The whole settings file is cached in memory after the first read. Reads
    // are frequent (the controller consults settings on nearly every MIDI
    // operation) and the file is only ever written by this process, so a
    // re-read per access would be pure cost. The cache is refreshed on save and
    // dropped on reset.
    struct XmlSettingsService::Impl
    {
        juce::File file;
        std::optional<AllUsersSettings> cache;

        // Returns nullopt for every failure mode alike -- absent file,
        // unparseable XML, wrong root element, missing sections -- because the
        // caller's response is the same in all of them: fall back to defaults.
        // Distinguishing them would add no behaviour.
        std::optional<AllUsersSettings> load() const
        {
            if (!file.existsAsFile())
            {
                return std::nullopt;
            }
            const auto root = juce::XmlDocument::parse(file);
            if (root == nullptr || !root->hasTagName("AllUsersSettings"))
            {
                return std::nullopt;
            }
            return parseSettings(*root);
        }

        void save(const AllUsersSettings& settings)
        {
            file.getParentDirectory().createDirectory();
            settingsToXml(settings)->writeTo(file);
        }
    };

    XmlSettingsService::XmlSettingsService(std::string preferredDirectory, std::string fallbackDirectory)
        : _impl(std::make_unique<Impl>())
    {
        _impl->file = resolveSettingsDirectory(preferredDirectory, fallbackDirectory)
                          .getChildFile(SETTINGS_FILE_NAME);
    }

    XmlSettingsService::~XmlSettingsService() = default;

    // Lazy load with a three-step fallback: read the file; if that fails write
    // the defaults and read them back; if THAT fails too (read-only
    // filesystem), keep the defaults in memory for this run.
    //
    // Returns const: see the contract on the interface declaration. Mutating
    // the cache in place would be invisible to disk, so the type refuses it.
    // [RQ-BUG-004, ADR-BUG-003 (DEC-BUG-010)]
    const AllUsersSettings& XmlSettingsService::allUsersSettings()
    {
        if (!_impl->cache.has_value())
        {
            _impl->cache = _impl->load();
            if (!_impl->cache.has_value())
            {
                // Missing, unreadable or legacy-partial file: persist the
                // defaults and reload, as the reference does. [RQ-SET-004]
                _impl->save(defaultAllUsersSettings());
                _impl->cache = _impl->load();
            }
            if (!_impl->cache.has_value())
            {
                // Persisting failed even on the fallback directory (e.g. a
                // read-only filesystem): continue with defaults held in
                // memory only, rather than dereference the empty cache.
                // [RQ-SET-004, ADR-SET-001 (DEC-SET-001)]
                _impl->cache = defaultAllUsersSettings();
            }
        }
        return *_impl->cache;
    }

    // The cache is updated from the ARGUMENT, not by re-reading the file just
    // written. Deliberate: a re-read would be slower and would silently mask a
    // failed write by leaving the previous values in place, where this way the
    // caller's own values are what everyone subsequently sees.
    void XmlSettingsService::saveSettings(const AllUsersSettings& settings)
    {
        _impl->save(settings);
        _impl->cache = settings;
    }

    // Clears the cache rather than filling it with the defaults just written,
    // so the next read goes through load() and reflects what actually reached
    // the disk. [RQ-SET-004]
    void XmlSettingsService::resetSettings()
    {
        _impl->save(defaultAllUsersSettings());
        _impl->cache.reset();
    }

    std::string XmlSettingsService::settingsFilePath() const
    {
        return _impl->file.getFullPathName().toStdString();
    }
}
