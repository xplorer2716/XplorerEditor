#include "xplorer/model/XpanderParameter.hpp"

#include <cmath>
#include <cstdint>
#include <vector>

namespace xplorer::model
{
    using xpl::midi::MidiMessage;

    // --- XpanderParameter -------------------------------------------------

    XpanderParameter::XpanderParameter(const std::string& name, int page, int subPage, int minValue,
                                       int maxValue, int step, MidiMessage message, int value,
                                       const std::string& label)
        : AbstractParameter(name, minValue, maxValue, step, std::move(message), label),
          _page(page),
          _subPage(subPage)
    {
        initializeValue(value);
    }

    void XpanderParameter::updateMessageFromValue()
    {
        rawMessageBytes()[SYSEX_VALUE_INDEX] = static_cast<std::uint8_t>(rawValue());
    }

    MidiMessage XpanderParameter::pageSelectMessage() const
    {
        return MidiMessage::sysEx(std::vector<std::uint8_t>{
            0xF0, 0x10, 0x02, 0x0B, static_cast<std::uint8_t>(_page),
            static_cast<std::uint8_t>(_subPage), 0xF7});
    }

    int XpanderParameter::buttonId() const
    {
        return message()[SYSEX_BUTTON_ID];
    }

    void XpanderParameter::setValueUnchanged(int value)
    {
        setValue(value);
        setChanged(false);
    }

    std::unique_ptr<midiapp::model::AbstractParameter> XpanderParameter::clone() const
    {
        return std::unique_ptr<AbstractParameter>(new XpanderParameter(*this));
    }

    // --- XpanderSignedParameter -------------------------------------------

    XpanderSignedParameter::XpanderSignedParameter(const std::string& name, int page, int subPage,
                                                   int minValue, int maxValue, int step,
                                                   MidiMessage message, int value,
                                                   const std::string& label)
        : XpanderParameter(name, page, subPage, minValue, maxValue, step, std::move(message), 0, label)
    {
        // The base constructor already ran initializeValue(0) with *this*
        // vtable not yet active; re-apply through the final override.
        initializeValue(value);
    }

    void XpanderSignedParameter::updateMessageFromValue()
    {
        std::uint8_t byteValue = 0;
        if (rawValue() < 0)
        {
            byteValue = static_cast<std::uint8_t>(0x80 - std::abs(rawValue()));
            rawMessageBytes()[SYSEX_VALUE_INDEX + 1] = 0x01;
        }
        else
        {
            byteValue = static_cast<std::uint8_t>(rawValue());
            rawMessageBytes()[SYSEX_VALUE_INDEX + 1] = 0x00;
        }
        rawMessageBytes()[SYSEX_VALUE_INDEX] = byteValue;
    }

    std::unique_ptr<midiapp::model::AbstractParameter> XpanderSignedParameter::clone() const
    {
        return std::unique_ptr<AbstractParameter>(new XpanderSignedParameter(*this));
    }

    // --- XpanderModMatrixParameter ------------------------------------------

    XpanderModMatrixParameter::XpanderModMatrixParameter(const std::string& name, int page,
                                                         int subPage, int minValue, int maxValue,
                                                         int step, MidiMessage message, int value)
        : XpanderParameter(name, page, subPage, minValue, maxValue, step, std::move(message), value)
    {
        setChanged(false); // avoid auto-send of mod sources (reference ctor)
    }

    int XpanderModMatrixParameter::idSource() const
    {
        return message()[ID_SOURCE_INDEX];
    }

    void XpanderModMatrixParameter::setIdSource(int idSource)
    {
        rawMessageBytes()[ID_SOURCE_INDEX] = static_cast<std::uint8_t>(idSource);
    }

    std::unique_ptr<midiapp::model::AbstractParameter> XpanderModMatrixParameter::clone() const
    {
        return std::unique_ptr<AbstractParameter>(new XpanderModMatrixParameter(*this));
    }

    // --- XpanderFullToneParameter -------------------------------------------

    XpanderFullToneParameter::XpanderFullToneParameter(MidiMessage message)
        : AbstractParameter("FullTone", 0, 1, 1, std::move(message))
    {
        initializeValue(0);
        setChanged(true); // sent as soon as the worker scans it
    }

    XpanderFullToneParameter::XpanderFullToneParameter(const XpanderFullToneParameter& other)
        : AbstractParameter(other)
    {
        setChanged(true); // clones re-arm transmission (reference clone ctor)
    }

    std::unique_ptr<midiapp::model::AbstractParameter> XpanderFullToneParameter::clone() const
    {
        return std::unique_ptr<AbstractParameter>(new XpanderFullToneParameter(*this));
    }
}
