#include "xplorer/model/XpanderTone.hpp"

#include "xplorer/model/XpanderSinglePatch.hpp"

// PortableFormat.hpp, not <format>: see its header comment and
// [RQ-BLD-025, ADR-BLD-004 (DEC-BLD-021)] for why.
#include "midiapp/service/PortableFormat.hpp"

namespace xplorer::model
{
    using midiapp::model::AbstractParameter;
    using midiapp::service::formatStr;
    using xpl::midi::MidiMessage;

    namespace
    {
        /// Parameter-edit frame: F0 10 02 0A 00 <buttonId> 00 00 00 <value> <sign> F7
        MidiMessage makeParameterFrame(std::uint8_t buttonId)
        {
            return MidiMessage::sysEx(std::vector<std::uint8_t>{
                0xF0, 0x10, 0x02, 0x0A, 0x00, buttonId, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF7});
        }

        /// Mod-edit frame: F0 10 02 0F 00 <id> 00 <command> 00 <value> 00 F7
        MidiMessage makeModEditFrame(EnumModulationEditCommands command, std::uint8_t value = 0x00)
        {
            return MidiMessage::sysEx(std::vector<std::uint8_t>{
                0xF0, 0x10, 0x02, 0x0F, 0x00, 0x00, 0x00,
                static_cast<std::uint8_t>(command), 0x00, value, 0x00, 0xF7});
        }

        bool hasFlag(std::uint8_t flags, auto flag)
        {
            const auto mask = static_cast<std::uint8_t>(flag);
            return (flags & mask) == mask;
        }
    }

    // A tone is the editor's live patch buffer: the 227 parameters of one
    // Xpander voice plus its 20-row modulation matrix. One instance exists per
    // controller and is mutated in place -- loading a file, receiving a dump or
    // randomizing all rewrite THIS object rather than replacing it, which is
    // why the UI can hold long-lived bindings to parameter names.
    //
    // "XPLORER" as the initial name is the reference's, and it is what the VFD
    // shows before the first patch arrives from the synth.
    XpanderTone::XpanderTone()
    {
        initializeParameterMap();
        setToneName("XPLORER");
        clearModulationMatrix();
    }

    // Patch names are FIXED-WIDTH on the wire: exactly TONE_NAME_LENGTH
    // characters, space-padded. The padding is not cosmetic -- toByteArray
    // writes the string straight into the dump, so a short name would shift
    // every following byte. [RQ-MOD-023]
    //
    // Note this is the plain model setter. The controller's own setToneName
    // override adds the heavy side effect (full retransmission + resync) that
    // the rename dialog depends on.
    void XpanderTone::setToneName(const std::string& name)
    {
        // Reference: truncate to 8 or pad right with spaces to 8.
        if (name.size() > constants::TONE_NAME_LENGTH)
        {
            _toneName = name.substr(0, constants::TONE_NAME_LENGTH);
        }
        else
        {
            _toneName = name;
            _toneName.resize(constants::TONE_NAME_LENGTH, ' ');
        }
    }

    // Wraps rather than clamps: stepping past patch 99 lands on 0 and vice
    // versa, which is what makes the previous/next buttons cycle endlessly the
    // way the instrument's own do. A clamp would make them stick at the ends.
    // [RQ-MOD-021, RQ-CTL-007]
    void XpanderTone::setCurrentProgramNumber(int programNumber)
    {
        if (programNumber < MIN_PROGRAM_NUMBER)
        {
            _currentProgramNumber = MAX_PROGRAM_NUMBER;
        }
        else if (programNumber > MAX_PROGRAM_NUMBER)
        {
            _currentProgramNumber = MIN_PROGRAM_NUMBER; // reloop
        }
        else
        {
            _currentProgramNumber = programNumber;
        }
    }

    std::set<std::string> XpanderTone::eligibleParametersForToneMorphing() const
    {
        return _parameterNamesForToneMorphing;
    }

    XpanderParameter& XpanderTone::parameterAt(const std::string& name)
    {
        return static_cast<XpanderParameter&>(parameterMap().at(name));
    }

    const XpanderParameter& XpanderTone::parameterAt(const std::string& name) const
    {
        return static_cast<const XpanderParameter&>(parameterMap().at(name));
    }

    XpanderModMatrixParameter& XpanderTone::modMatrixParameterAt(const std::string& name)
    {
        return static_cast<XpanderModMatrixParameter&>(parameterMap().at(name));
    }

    // Matrix rows have no dedicated storage: each row's amount and quantize
    // live in the ordinary parameter map under a generated name. These two
    // helpers are the single place those names are formed, so the map key and
    // every lookup can never drift apart. Entry numbers are 1-based here, as
    // everywhere in the matrix API.
    std::string XpanderTone::amountSourceParameterNameForEntry(int entryNumber)
    {
        return formatStr("MOD_AMNT_SRC_{}", entryNumber);
    }

    std::string XpanderTone::quantizeSourceParameterNameForEntry(int entryNumber)
    {
        return formatStr("MOD_QUANTIZE_{}", entryNumber);
    }

    // --- parameter map ----------------------------------------------------

    // Builds all 227 parameters, in the reference's own order -- and that order
    // is part of the contract, not an implementation detail: the map is
    // ORDERED, and the transmit worker's scan, the morphing filter and the
    // full-tone send all iterate it.
    //
    // Two sources feed this: the literal (non-family) parameters come from the
    // generated XpanderToneFixedParameters.inc, and the repeated families
    // (TRACK x3, ENV x5, LFO x5, RAMP x4, matrix x20) are looped here because
    // they differ only by index. Do not hand-edit the .inc -- regenerate it
    // with juce/tools/generate_fixed_parameters.py. [RQ-MOD-020]
    void XpanderTone::initializeParameterMap()
    {
        auto& map = parameterMap();

        auto addUnsigned = [&map](const char* name, EnumPages page, int subPage, int minValue,
                                  int maxValue, int step, std::uint8_t buttonId, int value,
                                  const char* label)
        {
            map.add(std::make_unique<XpanderParameter>(name, static_cast<int>(page), subPage,
                                                       minValue, maxValue, step,
                                                       makeParameterFrame(buttonId), value, label));
        };
        auto addSigned = [&map](const char* name, EnumPages page, int subPage, int minValue,
                                int maxValue, int step, std::uint8_t buttonId, int value,
                                const char* label)
        {
            map.add(std::make_unique<XpanderSignedParameter>(name, static_cast<int>(page), subPage,
                                                             minValue, maxValue, step,
                                                             makeParameterFrame(buttonId), value, label));
        };

#include "XpanderToneFixedParameters.inc"

        // --- TRACK_X (3x), reference INITIALIZEPARAMETERMAP_TRACK_X ---
        for (int trackNumber = 0; trackNumber < constants::TRACK_COUNT; ++trackNumber)
        {
            const auto page = static_cast<EnumPages>(static_cast<int>(EnumPages::TRACK_1) + trackNumber);
            addUnsigned(formatStr("TRACK_{}_IN", trackNumber + 1).c_str(), page, 0x00,
                        static_cast<int>(EnumModulationSources::KBD),
                        static_cast<int>(EnumModulationSources::LEV2), 1, 0x08, 0, "Input");
            for (int pointNumber = 0; pointNumber < constants::TRACK_POINTS_COUNTS; ++pointNumber)
            {
                addUnsigned(formatStr("TRACK_{}_POINT_{}", trackNumber + 1, pointNumber + 1).c_str(),
                            page, 0x00, 0, 63, 1, static_cast<std::uint8_t>(0x09 + pointNumber), 0,
                            formatStr("Point {}", pointNumber).c_str());
            }
        }

        // --- ENV_X (5x), reference INITIALIZEPARAMETERMAP_ENV_X ---
        for (int envNumber = 0; envNumber < constants::ENV_COUNT; ++envNumber)
        {
            const auto page = static_cast<EnumPages>(static_cast<int>(EnumPages::ENV_1) + envNumber);
            const int n = envNumber + 1;
            addUnsigned(formatStr("ENV_{}_DELAY", n).c_str(), page, 0x00, 0, 63, 1, 0x08, 0, "Del");
            addUnsigned(formatStr("ENV_{}_ATTACK", n).c_str(), page, 0x00, 0, 63, 1, 0x09, 0, "Atck");
            addUnsigned(formatStr("ENV_{}_DECAY", n).c_str(), page, 0x00, 0, 63, 1, 0x0A, 0, "Dec");
            addUnsigned(formatStr("ENV_{}_SUSTAIN", n).c_str(), page, 0x00, 0, 63, 1, 0x0B, 0, "Sust");
            addUnsigned(formatStr("ENV_{}_RELEASE", n).c_str(), page, 0x00, 0, 63, 1, 0x0C, 0, "Rel");
            addUnsigned(formatStr("ENV_{}_VOLUME", n).c_str(), page, 0x00, 0, 63, 1, 0x0D, 0, "Vol");
            addUnsigned(formatStr("ENV_{}_MODE_RESET", n).c_str(), page, 0x01, 0, 1, 1, 0x01, 0, "Reset");
            addUnsigned(formatStr("ENV_{}_MODE_FREERUN", n).c_str(), page, 0x01, 0, 1, 1, 0x02, 0, "Freerun");
            addUnsigned(formatStr("ENV_{}_MODE_DADR", n).c_str(), page, 0x01, 0, 1, 1, 0x03, 0, "DADR");
            addUnsigned(formatStr("ENV_{}_TRIG_SINGLE_MULTI", n).c_str(), page, 0x01, 0, 1, 1, 0x09, 0, "Single/Multi");
            addUnsigned(formatStr("ENV_{}_TRIG_EXTRIG", n).c_str(), page, 0x01, 0, 1, 1, 0x0A, 0, "Extern");
            addUnsigned(formatStr("ENV_{}_TRIG_LFOTRIG", n).c_str(), page, 0x01, 0, 1, 1, 0x0B, 0, "LFO");
            addUnsigned(formatStr("ENV_{}_TRIG_LFO_SOURCE", n).c_str(), page, 0x01,
                        static_cast<int>(EnumLFOTriggerSources::LFO1),
                        static_cast<int>(EnumLFOTriggerSources::VIB), 1, 0x0C, 0, "Source");
            addUnsigned(formatStr("ENV_{}_TRIG_GATED", n).c_str(), page, 0x01, 0, 1, 1, 0x0D, 0, "Gated");
        }

        // --- LFO_X (5x), reference INITIALIZEPARAMETERMAP_LFO_X ---
        for (int lfoNumber = 0; lfoNumber < constants::LFO_COUNT; ++lfoNumber)
        {
            const auto page = static_cast<EnumPages>(static_cast<int>(EnumPages::LFO_1) + lfoNumber);
            const int n = lfoNumber + 1;
            addUnsigned(formatStr("LFO_{}_SPEED", n).c_str(), page, 0x00, 0, 63, 1, 0x08, 0, "Speed");
            addUnsigned(formatStr("LFO_{}_WAVESHAPE", n).c_str(), page, 0x00,
                        static_cast<int>(EnumLFOWaveShapes::TRIANGLE),
                        static_cast<int>(EnumLFOWaveShapes::SAMPLE), 1, 0x09, 0, "Shape");
            addUnsigned(formatStr("LFO_{}_SAMPLE_INPUT", n).c_str(), page, 0x00,
                        static_cast<int>(EnumModulationSources::KBD),
                        static_cast<int>(EnumModulationSources::LEV2), 1, 0x0A, 0, "Sample input");
            addUnsigned(formatStr("LFO_{}_RETRIG", n).c_str(), page, 0x00, 0, 63, 1, 0x0B, 0, "Retrig");
            addUnsigned(formatStr("LFO_{}_AMP", n).c_str(), page, 0x00, 0, 63, 1, 0x0D, 0, "Amp");
            addUnsigned(formatStr("LFO_{}_LAG", n).c_str(), page, 0x01, 0, 1, 1, 0x00, 0, "Lag");
            addUnsigned(formatStr("LFO_{}_RETRIG_MODE", n).c_str(), page, 0x01,
                        static_cast<int>(EnumLFORetrigModes::OFF),
                        static_cast<int>(EnumLFORetrigModes::EXTRIG), 1, 0x1A, 0, "Retrig Mode");
        }

        // --- RAMP_X (4x), reference INITIALIZEPARAMETERMAP_RAMP_X ---
        for (int rampNumber = 0; rampNumber < constants::RAMP_COUNT; ++rampNumber)
        {
            const auto page = static_cast<EnumPages>(static_cast<int>(EnumPages::RAMP_1) + rampNumber);
            const int n = rampNumber + 1;
            addUnsigned(formatStr("RAMP_{}_RATE", n).c_str(), page, 0x00, 0, 63, 1, 0x09, 0, "Rate");
            addUnsigned(formatStr("RAMP_{}_TRIG_SINGLE_MULTI", n).c_str(), page, 0x01, 0, 1, 1, 0x09, 0, "Single/Multi");
            addUnsigned(formatStr("RAMP_{}_TRIG_EXTRIG", n).c_str(), page, 0x01, 0, 1, 1, 0x0A, 0, "Extern");
            addUnsigned(formatStr("RAMP_{}_TRIG_LFOTRIG", n).c_str(), page, 0x01, 0, 1, 1, 0x0B, 0, "LFO");
            addUnsigned(formatStr("RAMP_{}_TRIG_LFO_SOURCE", n).c_str(), page, 0x01,
                        static_cast<int>(EnumLFOTriggerSources::LFO1),
                        static_cast<int>(EnumLFOTriggerSources::VIB), 1, 0x0C, 0, "Source");
            addUnsigned(formatStr("RAMP_{}_TRIG_GATED", n).c_str(), page, 0x01, 0, 1, 1, 0x0D, 0, "Gated");
        }

        // --- MOD MATRIX (x20), reference MOD MATRIX region ---
        const auto& defaultPages = PAGE_SUBPAGE_FOR_MODULATION_DESTINATION[
            static_cast<std::size_t>(EnumModulationDestinations::VCO1_FRQ)];
        for (int entryNumber = 1; entryNumber < constants::MODENTRIES_COUNT + 1; ++entryNumber)
        {
            // page does not matter, changed dynamically with the destination
            map.add(std::make_unique<XpanderModMatrixParameter>(
                amountSourceParameterNameForEntry(entryNumber),
                static_cast<int>(defaultPages.page), defaultPages.subPage,
                ModulationMatrixEntry::MIN_AMOUNT, ModulationMatrixEntry::MAX_AMOUNT, 1,
                makeModEditFrame(EnumModulationEditCommands::SETUNSIGNEDVALUE), 0));
            map.add(std::make_unique<XpanderModMatrixParameter>(
                quantizeSourceParameterNameForEntry(entryNumber),
                static_cast<int>(defaultPages.page), defaultPages.subPage,
                ModulationMatrixEntry::MIN_QUANTIZE, ModulationMatrixEntry::MAX_QUANTIZE, 1,
                makeModEditFrame(EnumModulationEditCommands::SETQUANTIZE), 0));
        }

        // --- parameters eligible for tone morphing (reference filter) ---
        for (const auto& entry : map)
        {
            const auto* asModMatrix = dynamic_cast<const XpanderModMatrixParameter*>(entry.parameter.get());
            if (entry.parameter->minValue() <= 0 && entry.parameter->maxValue() >= 31
                && asModMatrix == nullptr)
            {
                _parameterNamesForToneMorphing.insert(entry.name);
            }
        }
    }

    // --- SysEx serialization -----------------------------------------------

    std::vector<std::uint8_t> XpanderTone::toByteArray() const
    {
        return toSinglePatch().toByteArray(_editingProgramNumber);
    }

    void XpanderTone::fromByteArray(std::span<const std::uint8_t> data)
    {
        XpanderSinglePatch patch;
        patch.fromByteArray(data);
        fromSinglePatch(patch);
    }

    std::string XpanderTone::getNameFromByteArray(std::span<const std::uint8_t> bytes)
    {
        return XpanderSinglePatch::getNameFromByteArray(bytes);
    }

    void XpanderTone::fromSinglePatch(const XpanderSinglePatch& patch)
    {
        // Reset all parameters (mid-range ones to 0) and the matrix.
        for (const auto& entry : parameterMap())
        {
            auto& parameter = *entry.parameter;
            if (parameter.minValue() < 0 && 0 < parameter.maxValue())
            {
                parameter.setValue(0);
            }
            else
            {
                parameter.setValue(parameter.minValue());
            }
        }
        clearModulationMatrix();

        auto setBit = [this](const std::string& name, bool set)
        { parameterAt(name).setValue(set ? 1 : 0); };

        // VCOs
        for (int i = 0; i < constants::VCO_COUNT; ++i)
        {
            const auto& vco = patch.vcos[static_cast<std::size_t>(i)];
            const int n = i + 1;
            parameterAt(formatStr("VCO{}_FREQ", n)).setValue(vco.freq);
            parameterAt(formatStr("VCO{}_DETUNE", n)).setValue(vco.detune);
            parameterAt(formatStr("VCO{}_PW", n)).setValue(vco.pw);
            parameterAt(formatStr("VCO{}_VOLUME", n)).setValue(vco.vol);
            // Waveshape flags: reference sets 1 only when the flag is on
            // (values were reset to 0 above).
            if (hasFlag(vco.wave, EnumVCOWaveFlags::VCOWAVEFLAG_TRI))
                setBit(formatStr("VCO{}_WAVESHAPE_TRI", n), true);
            if (hasFlag(vco.wave, EnumVCOWaveFlags::VCOWAVEFLAG_SAW))
                setBit(formatStr("VCO{}_WAVESHAPE_SAW", n), true);
            if (hasFlag(vco.wave, EnumVCOWaveFlags::VCOWAVEFLAG_PULSE))
                setBit(formatStr("VCO{}_WAVESHAPE_PULSE", n), true);
            if (n == 2)
            {
                if (hasFlag(vco.wave, EnumVCOWaveFlags::VCOWAVEFLAG_NOISE))
                    setBit(formatStr("VCO{}_WAVESHAPE_NOISE", n), true);
                // sync shown as mod on the panel but stored in wave
                setBit(formatStr("VCO{}_WAVE_SYNC", n), hasFlag(vco.wave, EnumVCOWaveFlags::VCOWAVEFLAG_SYNC));
            }
            setBit(formatStr("VCO{}_MOD_KEYB", n), hasFlag(vco.mod, EnumModulationFlags::MODFLAG_KEYBD));
            setBit(formatStr("VCO{}_MOD_LAG", n), hasFlag(vco.mod, EnumModulationFlags::MODFLAG_LAG));
            setBit(formatStr("VCO{}_MOD_LEV1", n), hasFlag(vco.mod, EnumModulationFlags::MODFLAG_LEV_1));
            setBit(formatStr("VCO{}_MOD_VIB", n), hasFlag(vco.mod, EnumModulationFlags::MODFLAG_VIB));
        }

        // VCF/VCA
        parameterAt("VCF_FREQ").setValue(patch.vcf.freq);
        parameterAt("VCF_RES").setValue(patch.vcf.res);
        parameterAt("VCF_MODE").setValue(patch.vcf.fmode);
        parameterAt("VCF_VCA1_VOLUME").setValue(patch.vcf.vca1);
        parameterAt("VCF_VCA2_VOLUME").setValue(patch.vcf.vca2);
        setBit("VCF_MOD_KEYB", hasFlag(patch.vcf.mod, EnumModulationFlags::MODFLAG_KEYBD));
        setBit("VCF_MOD_LAG", hasFlag(patch.vcf.mod, EnumModulationFlags::MODFLAG_LAG));
        setBit("VCF_MOD_LEV1", hasFlag(patch.vcf.mod, EnumModulationFlags::MODFLAG_LEV_1));
        setBit("VCF_MOD_VIB", hasFlag(patch.vcf.mod, EnumModulationFlags::MODFLAG_VIB));

        // FM/LAG
        parameterAt("FM_AMP").setValue(patch.fmAndLag.f_amp);
        parameterAt("FM_DESTINATION").setValue(patch.fmAndLag.f_dest);
        parameterAt("LAG_IN").setValue(patch.fmAndLag.lag_in);
        parameterAt("FMLAG_RATE").setValue(patch.fmAndLag.lag_rate);
        setBit("LAG_MODE_LEGATO", hasFlag(patch.fmAndLag.lag_mode, EnumLagModeFlags::LAGMODE_LEGATO));
        setBit("LAG_TIMING_LINEAR_EXPO", hasFlag(patch.fmAndLag.lag_mode, EnumLagModeFlags::LAGMODE_EXPO));
        setBit("LAG_LINEAR_EQUAL_TIME", hasFlag(patch.fmAndLag.lag_mode, EnumLagModeFlags::LAGMODE_EQUAL_TIME));

        // TRACK
        for (int t = 0; t < constants::TRACK_COUNT; ++t)
        {
            parameterAt(formatStr("TRACK_{}_IN", t + 1)).setValue(patch.tracks[static_cast<std::size_t>(t)].input);
            for (int p = 0; p < constants::TRACK_POINTS_COUNTS; ++p)
            {
                parameterAt(formatStr("TRACK_{}_POINT_{}", t + 1, p + 1))
                    .setValue(patch.tracks[static_cast<std::size_t>(t)].points[static_cast<std::size_t>(p)]);
            }
        }

        // ENV
        for (int e = 0; e < constants::ENV_COUNT; ++e)
        {
            const auto& env = patch.enveloppes[static_cast<std::size_t>(e)];
            const int n = e + 1;
            parameterAt(formatStr("ENV_{}_DELAY", n)).setValue(env.delay);
            parameterAt(formatStr("ENV_{}_ATTACK", n)).setValue(env.attack);
            parameterAt(formatStr("ENV_{}_DECAY", n)).setValue(env.decay);
            parameterAt(formatStr("ENV_{}_SUSTAIN", n)).setValue(env.sustain);
            parameterAt(formatStr("ENV_{}_RELEASE", n)).setValue(env.release);
            parameterAt(formatStr("ENV_{}_VOLUME", n)).setValue(env.amp);
            setBit(formatStr("ENV_{}_MODE_RESET", n), hasFlag(env.flags, EnumEnveloppeModeFlags::ENVMODE_RESET));
            setBit(formatStr("ENV_{}_MODE_FREERUN", n), hasFlag(env.flags, EnumEnveloppeModeFlags::ENVMODE_FREERUN));
            setBit(formatStr("ENV_{}_MODE_DADR", n), hasFlag(env.flags, EnumEnveloppeModeFlags::ENVMODE_DADR));
            setBit(formatStr("ENV_{}_TRIG_SINGLE_MULTI", n), hasFlag(env.flags, EnumEnveloppeModeFlags::ENVMODE_MULTI));
            setBit(formatStr("ENV_{}_TRIG_EXTRIG", n), hasFlag(env.flags, EnumEnveloppeModeFlags::ENVMODE_EXTRIG));
            setBit(formatStr("ENV_{}_TRIG_LFOTRIG", n), hasFlag(env.flags, EnumEnveloppeModeFlags::ENVMODE_LFOTRIG));
            // Reference: ignore a dummy lfotrig value when the flag is off.
            if (parameterAt(formatStr("ENV_{}_TRIG_LFOTRIG", n)).value() == 0)
            {
                parameterAt(formatStr("ENV_{}_TRIG_LFO_SOURCE", n))
                    .setValue(static_cast<int>(EnumLFOTriggerSources::LFO1));
            }
            else
            {
                parameterAt(formatStr("ENV_{}_TRIG_LFO_SOURCE", n)).setValue(env.lfotrig);
            }
            setBit(formatStr("ENV_{}_TRIG_GATED", n), hasFlag(env.flags, EnumEnveloppeModeFlags::ENVMODE_GATED));
        }

        // LFO
        for (int l = 0; l < constants::LFO_COUNT; ++l)
        {
            const auto& lfo = patch.lfos[static_cast<std::size_t>(l)];
            const int n = l + 1;
            parameterAt(formatStr("LFO_{}_SPEED", n)).setValue(lfo.speed);
            parameterAt(formatStr("LFO_{}_WAVESHAPE", n)).setValue(lfo.wave);
            parameterAt(formatStr("LFO_{}_SAMPLE_INPUT", n)).setValue(lfo.sample);
            parameterAt(formatStr("LFO_{}_RETRIG", n)).setValue(lfo.retrig);
            parameterAt(formatStr("LFO_{}_AMP", n)).setValue(lfo.amp);
            parameterAt(formatStr("LFO_{}_LAG", n)).setValue(lfo.lag);
            parameterAt(formatStr("LFO_{}_RETRIG_MODE", n)).setValue(lfo.retrig_mode);
        }

        // RAMP
        for (int r = 0; r < constants::RAMP_COUNT; ++r)
        {
            const auto& ramp = patch.ramps[static_cast<std::size_t>(r)];
            const int n = r + 1;
            parameterAt(formatStr("RAMP_{}_RATE", n)).setValue(ramp.rate);
            setBit(formatStr("RAMP_{}_TRIG_SINGLE_MULTI", n), hasFlag(ramp.flags, EnumRampFlags::RAMPF_MULTI));
            setBit(formatStr("RAMP_{}_TRIG_EXTRIG", n), hasFlag(ramp.flags, EnumRampFlags::RAMPF_EXTRIG));
            setBit(formatStr("RAMP_{}_TRIG_LFOTRIG", n), hasFlag(ramp.flags, EnumRampFlags::RAMPF_LFOTRIG));
            // Reference quirk kept as-is: the guard reads the ENV_n LFOTRIG
            // flag (not RAMP_n) before accepting the ramp's lfotrig value.
            if (parameterAt(formatStr("ENV_{}_TRIG_LFOTRIG", n)).value() == 0)
            {
                parameterAt(formatStr("RAMP_{}_TRIG_LFO_SOURCE", n))
                    .setValue(static_cast<int>(EnumLFOTriggerSources::LFO1));
            }
            else
            {
                parameterAt(formatStr("RAMP_{}_TRIG_LFO_SOURCE", n)).setValue(ramp.lfotrig);
            }
            setBit(formatStr("RAMP_{}_TRIG_GATED", n), hasFlag(ramp.flags, EnumRampFlags::RAMPF_GATED));
        }

        // MODULATION MATRIX: rebuilt through the destination-change flow.
        for (int i = 0; i < constants::MODENTRIES_COUNT; ++i)
        {
            const auto& entry = patch.modulationEntries[static_cast<std::size_t>(i)];
            if (entry.source <= static_cast<int>(EnumModulationSources::LEV2)
                && entry.dest <= static_cast<int>(EnumModulationDestinations::LAG_RATE))
            {
                changeModulationDestination(entry.source, entry.amount, entry.quantize ? 1 : 0,
                                            static_cast<int>(EnumModulationDestinations::VCO1_FRQ),
                                            entry.dest, i + 1, nullptr);
            }
        }

        setToneName(patch.patchName());
    }

    XpanderSinglePatch XpanderTone::toSinglePatch() const
    {
        XpanderSinglePatch patch;
        auto bit = [this](const std::string& name) { return parameterAt(name).value() == 1; };
        auto byteOf = [this](const std::string& name)
        { return static_cast<std::uint8_t>(parameterAt(name).value()); };

        // VCOs
        for (int i = 0; i < constants::VCO_COUNT; ++i)
        {
            auto& vco = patch.vcos[static_cast<std::size_t>(i)];
            const int n = i + 1;
            vco.freq = byteOf(formatStr("VCO{}_FREQ", n));
            vco.detune = static_cast<std::int8_t>(parameterAt(formatStr("VCO{}_DETUNE", n)).value());
            vco.pw = byteOf(formatStr("VCO{}_PW", n));
            vco.vol = byteOf(formatStr("VCO{}_VOLUME", n));
            vco.wave = 0;
            if (bit(formatStr("VCO{}_WAVESHAPE_TRI", n))) vco.wave |= static_cast<std::uint8_t>(EnumVCOWaveFlags::VCOWAVEFLAG_TRI);
            if (bit(formatStr("VCO{}_WAVESHAPE_SAW", n))) vco.wave |= static_cast<std::uint8_t>(EnumVCOWaveFlags::VCOWAVEFLAG_SAW);
            if (bit(formatStr("VCO{}_WAVESHAPE_PULSE", n))) vco.wave |= static_cast<std::uint8_t>(EnumVCOWaveFlags::VCOWAVEFLAG_PULSE);
            if (n == 2)
            {
                if (bit(formatStr("VCO{}_WAVESHAPE_NOISE", n))) vco.wave |= static_cast<std::uint8_t>(EnumVCOWaveFlags::VCOWAVEFLAG_NOISE);
                if (bit(formatStr("VCO{}_WAVE_SYNC", n))) vco.wave |= static_cast<std::uint8_t>(EnumVCOWaveFlags::VCOWAVEFLAG_SYNC);
            }
            vco.mod = 0;
            if (bit(formatStr("VCO{}_MOD_KEYB", n))) vco.mod |= static_cast<std::uint8_t>(EnumModulationFlags::MODFLAG_KEYBD);
            if (bit(formatStr("VCO{}_MOD_LAG", n))) vco.mod |= static_cast<std::uint8_t>(EnumModulationFlags::MODFLAG_LAG);
            if (bit(formatStr("VCO{}_MOD_LEV1", n))) vco.mod |= static_cast<std::uint8_t>(EnumModulationFlags::MODFLAG_LEV_1);
            if (bit(formatStr("VCO{}_MOD_VIB", n))) vco.mod |= static_cast<std::uint8_t>(EnumModulationFlags::MODFLAG_VIB);
        }

        // VCF/VCA
        patch.vcf.freq = byteOf("VCF_FREQ");
        patch.vcf.res = byteOf("VCF_RES");
        patch.vcf.fmode = byteOf("VCF_MODE");
        patch.vcf.vca1 = byteOf("VCF_VCA1_VOLUME");
        patch.vcf.vca2 = byteOf("VCF_VCA2_VOLUME");
        patch.vcf.mod = 0;
        if (bit("VCF_MOD_KEYB")) patch.vcf.mod |= static_cast<std::uint8_t>(EnumModulationFlags::MODFLAG_KEYBD);
        if (bit("VCF_MOD_LAG")) patch.vcf.mod |= static_cast<std::uint8_t>(EnumModulationFlags::MODFLAG_LAG);
        if (bit("VCF_MOD_LEV1")) patch.vcf.mod |= static_cast<std::uint8_t>(EnumModulationFlags::MODFLAG_LEV_1);
        if (bit("VCF_MOD_VIB")) patch.vcf.mod |= static_cast<std::uint8_t>(EnumModulationFlags::MODFLAG_VIB);

        // FM/LAG
        patch.fmAndLag.f_amp = byteOf("FM_AMP");
        patch.fmAndLag.f_dest = byteOf("FM_DESTINATION");
        patch.fmAndLag.lag_in = byteOf("LAG_IN");
        patch.fmAndLag.lag_rate = byteOf("FMLAG_RATE");
        patch.fmAndLag.lag_mode = 0;
        if (bit("LAG_MODE_LEGATO")) patch.fmAndLag.lag_mode |= static_cast<std::uint8_t>(EnumLagModeFlags::LAGMODE_LEGATO);
        if (bit("LAG_TIMING_LINEAR_EXPO")) patch.fmAndLag.lag_mode |= static_cast<std::uint8_t>(EnumLagModeFlags::LAGMODE_EXPO);
        if (bit("LAG_LINEAR_EQUAL_TIME")) patch.fmAndLag.lag_mode |= static_cast<std::uint8_t>(EnumLagModeFlags::LAGMODE_EQUAL_TIME);

        // TRACK
        for (int t = 0; t < constants::TRACK_COUNT; ++t)
        {
            patch.tracks[static_cast<std::size_t>(t)].input = byteOf(formatStr("TRACK_{}_IN", t + 1));
            for (int p = 0; p < constants::TRACK_POINTS_COUNTS; ++p)
            {
                patch.tracks[static_cast<std::size_t>(t)].points[static_cast<std::size_t>(p)] =
                    byteOf(formatStr("TRACK_{}_POINT_{}", t + 1, p + 1));
            }
        }

        // ENV
        for (int e = 0; e < constants::ENV_COUNT; ++e)
        {
            auto& env = patch.enveloppes[static_cast<std::size_t>(e)];
            const int n = e + 1;
            env.delay = byteOf(formatStr("ENV_{}_DELAY", n));
            env.attack = byteOf(formatStr("ENV_{}_ATTACK", n));
            env.decay = byteOf(formatStr("ENV_{}_DECAY", n));
            env.sustain = byteOf(formatStr("ENV_{}_SUSTAIN", n));
            env.release = byteOf(formatStr("ENV_{}_RELEASE", n));
            env.amp = byteOf(formatStr("ENV_{}_VOLUME", n));
            env.flags = 0;
            if (bit(formatStr("ENV_{}_MODE_RESET", n))) env.flags |= static_cast<std::uint8_t>(EnumEnveloppeModeFlags::ENVMODE_RESET);
            if (bit(formatStr("ENV_{}_MODE_FREERUN", n))) env.flags |= static_cast<std::uint8_t>(EnumEnveloppeModeFlags::ENVMODE_FREERUN);
            if (bit(formatStr("ENV_{}_MODE_DADR", n))) env.flags |= static_cast<std::uint8_t>(EnumEnveloppeModeFlags::ENVMODE_DADR);
            if (bit(formatStr("ENV_{}_TRIG_SINGLE_MULTI", n))) env.flags |= static_cast<std::uint8_t>(EnumEnveloppeModeFlags::ENVMODE_MULTI);
            if (bit(formatStr("ENV_{}_TRIG_EXTRIG", n))) env.flags |= static_cast<std::uint8_t>(EnumEnveloppeModeFlags::ENVMODE_EXTRIG);
            if (bit(formatStr("ENV_{}_TRIG_LFOTRIG", n))) env.flags |= static_cast<std::uint8_t>(EnumEnveloppeModeFlags::ENVMODE_LFOTRIG);
            env.lfotrig = byteOf(formatStr("ENV_{}_TRIG_LFO_SOURCE", n));
            if (bit(formatStr("ENV_{}_TRIG_GATED", n))) env.flags |= static_cast<std::uint8_t>(EnumEnveloppeModeFlags::ENVMODE_GATED);
        }

        // LFO
        for (int l = 0; l < constants::LFO_COUNT; ++l)
        {
            auto& lfo = patch.lfos[static_cast<std::size_t>(l)];
            const int n = l + 1;
            lfo.speed = byteOf(formatStr("LFO_{}_SPEED", n));
            lfo.wave = byteOf(formatStr("LFO_{}_WAVESHAPE", n));
            lfo.sample = byteOf(formatStr("LFO_{}_SAMPLE_INPUT", n));
            lfo.retrig = byteOf(formatStr("LFO_{}_RETRIG", n));
            lfo.amp = byteOf(formatStr("LFO_{}_AMP", n));
            lfo.lag = byteOf(formatStr("LFO_{}_LAG", n));
            lfo.retrig_mode = byteOf(formatStr("LFO_{}_RETRIG_MODE", n));
        }

        // RAMP
        for (int r = 0; r < constants::RAMP_COUNT; ++r)
        {
            auto& ramp = patch.ramps[static_cast<std::size_t>(r)];
            const int n = r + 1;
            ramp.rate = byteOf(formatStr("RAMP_{}_RATE", n));
            ramp.flags = 0;
            if (bit(formatStr("RAMP_{}_TRIG_SINGLE_MULTI", n))) ramp.flags |= static_cast<std::uint8_t>(EnumRampFlags::RAMPF_MULTI);
            if (bit(formatStr("RAMP_{}_TRIG_EXTRIG", n))) ramp.flags |= static_cast<std::uint8_t>(EnumRampFlags::RAMPF_EXTRIG);
            if (bit(formatStr("RAMP_{}_TRIG_LFOTRIG", n))) ramp.flags |= static_cast<std::uint8_t>(EnumRampFlags::RAMPF_LFOTRIG);
            ramp.lfotrig = byteOf(formatStr("RAMP_{}_TRIG_LFO_SOURCE", n));
            if (bit(formatStr("RAMP_{}_TRIG_GATED", n))) ramp.flags |= static_cast<std::uint8_t>(EnumRampFlags::RAMPF_GATED);
        }

        // MODULATION MATRIX
        for (int i = 0; i < constants::MODENTRIES_COUNT; ++i)
        {
            const auto& entry = _modulationMatrix[static_cast<std::size_t>(i)];
            auto& target = patch.modulationEntries[static_cast<std::size_t>(i)];
            if (entry.source == EnumModulationSourcesModMatrix::NONE)
            {
                target.source = XpanderSinglePatch::UNUSED_ENTRY_SOURCE_VALUE;
                target.dest = XpanderSinglePatch::UNUSED_ENTRY_DEST_VALUE;
            }
            else
            {
                target.source = static_cast<std::uint8_t>(entry.source);
                target.dest = static_cast<std::uint8_t>(entry.destination);
            }
            target.amount = static_cast<std::int8_t>(entry.amount());
            target.quantize = entry.quantize() == 1;
        }

        patch.setPatchName(toneName());
        return patch;
    }

    // --- randomizer helpers -------------------------------------------------

    // Spreads the two oscillators apart so a random patch sounds like an
    // analogue instrument rather than a single stiff tone. The two magnitudes
    // are the reference's: a barely-there 1 for "digital", a wide 10 for
    // "analog". Symmetric around centre, so the perceived pitch does not move.
    // [RQ-CTL-050]
    void XpanderTone::detune(bool detuneAnalog)
    {
        const int detuneValue = detuneAnalog ? 10 : 1;
        parameterAt("VCO1_DETUNE").setValue(-detuneValue);
        parameterAt("VCO2_DETUNE").setValue(detuneValue);
    }

    /// Tunes the two oscillators to the interval the randomizer strategy names,
    /// VCO2 above VCO1. `Free` is the only value that leaves both pitches as the
    /// randomizer left them.
    ///
    /// `Octave` diverges from the reference implementation, which has no `case`
    /// for it and so lets it behave exactly like `Free` — an option that never
    /// did anything. [RQ-BUG-001, ADR-BUG-001 (DEC-BUG-001, DEC-BUG-003,
    /// DEC-BUG-004)]
    void XpanderTone::defineVCOFrequenciesTuning(EnumRandomVCOFreq tuning)
    {
        // Shared term of the compound intervals: a ninth is a second an octave
        // up, an eleventh a fourth, a thirteenth a sixth.
        constexpr int SEMITONES_PER_OCTAVE = 12;
        // Not an interval, despite holding the same value: the reference parks
        // both oscillators on this mid-range pitch for "same note". Kept
        // separate so a later edit to one cannot silently move the other.
        constexpr int SAME_NOTE_SEMITONE = 12;

        auto& vco1 = parameterAt("VCO1_FREQ");
        auto& vco2 = parameterAt("VCO2_FREQ");
        switch (tuning)
        {
            case EnumRandomVCOFreq::SameNote: vco1.setValue(SAME_NOTE_SEMITONE); vco2.setValue(vco1.value()); break;
            case EnumRandomVCOFreq::Third:    vco1.setValue(0);  vco2.setValue(4); break;
            case EnumRandomVCOFreq::Fifth:    vco1.setValue(0);  vco2.setValue(7); break;
            case EnumRandomVCOFreq::Seventh:  vco1.setValue(0);  vco2.setValue(11); break;
            case EnumRandomVCOFreq::Octave:   vco1.setValue(0);  vco2.setValue(SEMITONES_PER_OCTAVE); break;
            case EnumRandomVCOFreq::Ninth:    vco1.setValue(0);  vco2.setValue(SEMITONES_PER_OCTAVE + 2); break;
            case EnumRandomVCOFreq::Eleventh: vco1.setValue(0);  vco2.setValue(SEMITONES_PER_OCTAVE + 5); break;
            case EnumRandomVCOFreq::Thirteenth: vco1.setValue(0); vco2.setValue(SEMITONES_PER_OCTAVE + 9); break;
            case EnumRandomVCOFreq::Free:
                break;
        }
    }

    // The randomizer's audibility guarantee. A fully random matrix usually
    // leaves nothing routed to the amplifier, so the patch is silent -- which
    // is why the user can ask for ENV2 to be forced onto VCA2 and pick the
    // envelope shape. The four shapes below are the reference's DADSR + volume
    // presets, not tunable values.
    //
    // EnumRandomVCAEnv::Free returns early: the reference asserts on it rather
    // than handling it, so there is deliberately no shape to apply.
    // [RQ-MOD-033, RQ-CTL-050]
    void XpanderTone::forceEnv2ModVca2AfterRandomizeMatrix(EnumRandomVCAEnv enveloppe)
    {
        struct EnvelopeShape { int values[6]; }; // DADSR + volume
        EnvelopeShape shape{};
        switch (enveloppe)
        {
            case EnumRandomVCAEnv::Organ: shape = {{0, 63, 63, 63, 0, 63}}; break;
            case EnumRandomVCAEnv::String: shape = {{0, 30, 63, 63, 32, 63}}; break;
            case EnumRandomVCAEnv::Percusive: shape = {{0, 0, 63, 10, 0, 63}}; break;
            case EnumRandomVCAEnv::PercusiveWithRelease: shape = {{0, 0, 63, 0, 32, 63}}; break;
            case EnumRandomVCAEnv::Free: return; // unsupported in the reference (Debug.Fail)
        }
        parameterAt("ENV_2_DELAY").setValue(shape.values[0]);
        parameterAt("ENV_2_ATTACK").setValue(shape.values[1]);
        parameterAt("ENV_2_DECAY").setValue(shape.values[2]);
        parameterAt("ENV_2_SUSTAIN").setValue(shape.values[3]);
        parameterAt("ENV_2_RELEASE").setValue(shape.values[4]);
        parameterAt("ENV_2_VOLUME").setValue(shape.values[5]);
        for (const auto* name : {"ENV_2_MODE_RESET", "ENV_2_MODE_FREERUN", "ENV_2_MODE_DADR",
                                 "ENV_2_TRIG_SINGLE_MULTI", "ENV_2_TRIG_EXTRIG", "ENV_2_TRIG_LFOTRIG",
                                 "ENV_2_TRIG_LFO_SOURCE", "ENV_2_TRIG_GATED", "VCF_VCA2_VOLUME"})
        {
            parameterAt(name).setValue(0);
        }

        // Reset every modulation targeting VCA2_VOL, then route ENV2 -> VCA2.
        for (auto& entry : _modulationMatrix)
        {
            if (entry.destination == EnumModulationDestinations::VCA2_VOL)
            {
                entry.destination = EnumModulationDestinations::VCO1_FRQ;
                entry.source = EnumModulationSourcesModMatrix::NONE;
                entry.idSource = UNDEFINED_MODULATION_SOURCE_NUMBER;
                entry.setAmount(0);
                entry.setQuantize(0);
            }
        }
        const int nextIdSource = getNextAvailableModIdSourceForDest(EnumModulationDestinations::VCA2_VOL);
        int nextEntry = getNextAvailableModEntry();
        if (nextEntry == NO_AVAILABLE_MOD_ENTRY)
        {
            // whole tone will be resent: replacing the first slot is enough
            nextEntry = 0;
        }
        auto& target = _modulationMatrix[static_cast<std::size_t>(nextEntry)];
        target.idSource = nextIdSource;
        target.source = EnumModulationSourcesModMatrix::ENV2;
        target.setAmount(63);
        target.destination = EnumModulationDestinations::VCA2_VOL;
        target.setQuantize(0);
    }
}
