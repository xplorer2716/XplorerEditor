#pragma once

// Port of Xplorer.Model parameter types. Wire formats (verbatim from the
// reference comments):
//   page select : F0 10 02 0B <page> <subpage> F7                  (7 bytes)
//   param edit  : F0 10 02 0A 00 <buttonId> 00 00 00 <value> <sign> F7 (12 bytes)
// [RQ-MOD-010..013]

#include "midiapp/model/AbstractParameter.hpp"

#include <memory>
#include <string>

namespace xplorer::model
{
    /// An Xpander unsigned parameter. [RQ-MOD-010]
    class XpanderParameter : public midiapp::model::AbstractParameter
    {
    public:
        static constexpr int SYSEX_VALUE_INDEX = 9;   // 0 based
        static constexpr int SYSEX_MESSAGE_LENGTH = 12;
        static constexpr int SYSEX_BUTTON_ID = 5;

        XpanderParameter(const std::string& name, int page, int subPage, int minValue, int maxValue,
                         int step, xpl::midi::MidiMessage message, int value, const std::string& label = {});

        [[nodiscard]] int page() const { return _page; }
        void setPage(int page) { _page = page; }
        [[nodiscard]] int subPage() const { return _subPage; }
        void setSubPage(int subPage) { _subPage = subPage; }

        /// F0 10 02 0B <page> <subpage> F7, rebuilt from current page/subpage.
        [[nodiscard]] xpl::midi::MidiMessage pageSelectMessage() const;

        /// Every parameter simulates a panel button; its id lives in the
        /// message. (reference ButtonID)
        [[nodiscard]] int buttonId() const;

        /// Sets the value through the normal quantize/clamp path but leaves
        /// the changed flag clear (reference SetValueUnchanged) — used when
        /// mirroring edits coming *from* the synth.
        void setValueUnchanged(int value);

        [[nodiscard]] std::unique_ptr<AbstractParameter> clone() const override;

    protected:
        XpanderParameter(const XpanderParameter& other) = default;

        void updateMessageFromValue() override;

    private:
        int _page = 0;
        int _subPage = 0;
    };

    /// Signed variant (e.g. VCO detune): negative values encode as
    /// 0x80-|v| with the sign byte set. [RQ-MOD-011]
    class XpanderSignedParameter final : public XpanderParameter
    {
    public:
        XpanderSignedParameter(const std::string& name, int page, int subPage, int minValue,
                               int maxValue, int step, xpl::midi::MidiMessage message, int value,
                               const std::string& label = {});

        [[nodiscard]] std::unique_ptr<AbstractParameter> clone() const override;

    private:
        XpanderSignedParameter(const XpanderSignedParameter& other) = default;

        void updateMessageFromValue() override;
    };

    /// Modulation-matrix parameter (amount, quantize): carries the mod-edit
    /// command frame; construction leaves the changed flag clear to avoid
    /// auto-sending mod sources. [RQ-MOD-012]
    class XpanderModMatrixParameter final : public XpanderParameter
    {
    public:
        static constexpr int ID_SOURCE_INDEX = 5;
        static constexpr int MODULATION_CMD_INDEX = 7;
        static constexpr int MODULATION_SOURCE_INDEX = 9;

        XpanderModMatrixParameter(const std::string& name, int page, int subPage, int minValue,
                                  int maxValue, int step, xpl::midi::MidiMessage message, int value);

        [[nodiscard]] int idSource() const;
        void setIdSource(int idSource);

        [[nodiscard]] std::unique_ptr<AbstractParameter> clone() const override;

    private:
        XpanderModMatrixParameter(const XpanderModMatrixParameter& other) = default;
    };

    /// A full single-patch dump wrapped as a parameter so it rides the same
    /// transmit queue; value/min/max are meaningless. Always constructed
    /// changed so it is sent immediately. [RQ-MOD-013]
    class XpanderFullToneParameter final : public midiapp::model::AbstractParameter
    {
    public:
        explicit XpanderFullToneParameter(xpl::midi::MidiMessage message);

        [[nodiscard]] std::unique_ptr<AbstractParameter> clone() const override;

    private:
        XpanderFullToneParameter(const XpanderFullToneParameter& other);

        void updateMessageFromValue() override {} // value is meaningless here
    };
}
