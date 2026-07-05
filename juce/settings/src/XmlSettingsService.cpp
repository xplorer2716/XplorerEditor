// XML persistence schema-compatible with the .NET XmlSerializer output of
// the reference AllUsersSettingsService, so existing xplorer.users.config
// files import unchanged. [RQ-SET-001, RQ-SET-004, RQ-SET-006]
#include "xplorer/settings/SettingsService.hpp"

#include <juce_core/juce_core.h>

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

        constexpr EnumName<EnumRandomVCO2> VCO2_NAMES[]{
            {EnumRandomVCO2::EnableFM, "EnableFM"},
            {EnumRandomVCO2::EnableNoise, "EnableNoise"},
            {EnumRandomVCO2::EnableSync, "EnableSync"},
        };
        constexpr EnumName<EnumRandomVCOFreq> VCOFREQ_NAMES[]{
            {EnumRandomVCOFreq::Free, "Free"}, {EnumRandomVCOFreq::SameNote, "SameNote"},
            {EnumRandomVCOFreq::Third, "Third"}, {EnumRandomVCOFreq::Fifth, "Fifth"},
            {EnumRandomVCOFreq::Seventh, "Seventh"}, {EnumRandomVCOFreq::Octave, "Octave"},
            {EnumRandomVCOFreq::Ninth, "Ninth"}, {EnumRandomVCOFreq::Eleventh, "Eleventh"},
            {EnumRandomVCOFreq::Thirteenth, "Thirteenth"},
        };
        constexpr EnumName<EnumRandomVCODetune> VCODETUNE_NAMES[]{
            {EnumRandomVCODetune::Free, "Free"},
            {EnumRandomVCODetune::Digital, "Digital"},
            {EnumRandomVCODetune::Analog, "Analog"},
        };
        constexpr EnumName<EnumRandomVCAEnv> VCAENV_NAMES[]{
            {EnumRandomVCAEnv::Free, "Free"}, {EnumRandomVCAEnv::Organ, "Organ"},
            {EnumRandomVCAEnv::String, "String"}, {EnumRandomVCAEnv::Percusive, "Percusive"},
            {EnumRandomVCAEnv::PercusiveWithRelease, "PercusiveWithRelease"},
        };
        constexpr EnumName<EnumRandomModMatrix> MODMATRIX_NAMES[]{
            {EnumRandomModMatrix::EnableAmount, "EnableAmount"},
            {EnumRandomModMatrix::EnableSourcesAndDestinations, "EnableSourcesAndDestinations"},
            {EnumRandomModMatrix::EnableQuantize, "EnableQuantize"},
        };

        template <typename Enum, std::size_t N>
        std::optional<Enum> parseEnum(const EnumName<Enum> (&table)[N], const juce::String& text)
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
        juce::String enumToString(const EnumName<Enum> (&table)[N], Enum value)
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
        std::optional<Enum> parseFlags(const EnumName<Enum> (&table)[N], const juce::String& text)
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
        juce::String flagsToString(const EnumName<Enum> (&table)[N], Enum value)
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

        // --- element helpers ----------------------------------------------

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
            settings.uiConfig.knobMovementIsLinear = childBool(*ui, "KnobMovementIsLinear").value_or(false);
            settings.uiConfig.knobStyleIsStandard = childBool(*ui, "KnobStyleIsStandard").value_or(false);

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
            addChildText(*ui, "KnobMovementIsLinear", settings.uiConfig.knobMovementIsLinear ? "true" : "false");
            addChildText(*ui, "KnobStyleIsStandard", settings.uiConfig.knobStyleIsStandard ? "true" : "false");

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

    struct XmlSettingsService::Impl
    {
        juce::File file;
        std::optional<AllUsersSettings> cache;

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

    XmlSettingsService::XmlSettingsService(std::string settingsDirectory)
        : _impl(std::make_unique<Impl>())
    {
        _impl->file = juce::File(juce::String(settingsDirectory)).getChildFile(SETTINGS_FILE_NAME);
    }

    XmlSettingsService::~XmlSettingsService() = default;

    AllUsersSettings& XmlSettingsService::allUsersSettings()
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
        }
        return *_impl->cache;
    }

    void XmlSettingsService::saveSettings(const AllUsersSettings& settings)
    {
        _impl->save(settings);
        _impl->cache = settings;
    }

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
