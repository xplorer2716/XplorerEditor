/*
Xplorer - A real-time editor for the Oberheim Xpander and Matrix-12 synths
Copyright (C) 2012-2026 Pascal Schmitt

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include "midiapp/model/AbstractParameter.hpp"
#include "midiapp/model/AbstractTone.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace midiapp::testing
{
    /// Minimal concrete parameter for framework tests: a 6-byte SysEx frame
    /// whose 5th byte carries the (biased) value.
    class TestParameter final : public midiapp::model::AbstractParameter
    {
    public:
        static constexpr std::size_t VALUE_INDEX = 4;

        TestParameter(const std::string& name, int minValue, int maxValue, int step, int value)
            : AbstractParameter(name, minValue, maxValue, step,
                                xpl::midi::MidiMessage::sysEx(std::vector<std::uint8_t>{0xF0, 0x10, 0x02, 0x00, 0x00, 0xF7}),
                                {})
        {
            initializeValue(value);
        }

        TestParameter(const TestParameter& other) = default;

        [[nodiscard]] std::unique_ptr<AbstractParameter> clone() const override
        {
            return std::unique_ptr<AbstractParameter>(new TestParameter(*this));
        }

    private:
        void updateMessageFromValue() override
        {
            // Two's-complement low byte, masked to 7 bits so negative test
            // ranges still fit a SysEx data byte.
            rawMessageBytes()[VALUE_INDEX] = static_cast<std::uint8_t>(rawValue() & 0x7F);
        }
    };

    /// Minimal concrete tone: name stored as a plain string, parameters
    /// added by tests.
    class TestTone final : public midiapp::model::AbstractTone
    {
    public:
        [[nodiscard]] std::string toneName() const override { return _name; }
        void setToneName(const std::string& name) override { _name = name; }

        void addParameter(const std::string& name, int minValue, int maxValue, int step, int value)
        {
            parameterMap().add(std::make_unique<TestParameter>(name, minValue, maxValue, step, value));
        }

    private:
        std::string _name;
    };
}
