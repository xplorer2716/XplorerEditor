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

#include "xpl/midi/MidiMessage.hpp"

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace midiapp::model
{
    /// Port of MidiApp.MidiController.Model.AbstractParameter: a named,
    /// bounded, stepped integer parameter carrying the SysEx message that
    /// transmits its value. Thread safe. [RQ-FMW-001..003]
    class AbstractParameter
    {
    public:
        virtual ~AbstractParameter() = default;

        [[nodiscard]] std::string name() const;
        [[nodiscard]] std::string label() const;
        void setLabel(const std::string& label);

        [[nodiscard]] int minValue() const;
        [[nodiscard]] int maxValue() const;

        /// @throws std::invalid_argument when step is 0 (reference throws).
        [[nodiscard]] int step() const;
        void setStep(int step);

        /// Quantizes to step (integer division, truncation toward zero),
        /// clamps to [min, max]; when the result differs from the current
        /// value: stores it, sets the changed flag and regenerates the
        /// message. [RQ-FMW-002]
        [[nodiscard]] int value() const;
        void setValue(int value);

        /// Set when a new different value was stored; cleared by the client
        /// (the transmit worker). [RQ-FMW-002]
        [[nodiscard]] bool changed() const;
        void setChanged(bool changed);

        /// The SysEx message transmitting the current value. [RQ-FMW-001]
        [[nodiscard]] xpl::midi::MidiMessage message() const;

        /// Deep copy, including the message bytes. [RQ-FMW-003]
        [[nodiscard]] virtual std::unique_ptr<AbstractParameter> clone() const = 0;

        [[nodiscard]] std::string toString() const;

    protected:
        /// Derived constructors MUST call initializeValue(value) as their
        /// last step: the reference base constructor virtual-dispatches
        /// UpdateMessageFromValue, which a C++ base constructor cannot do.
        AbstractParameter(std::string name, int minValue, int maxValue, int step,
                          xpl::midi::MidiMessage message, std::string label = {});

        AbstractParameter(const AbstractParameter& other);
        AbstractParameter& operator=(const AbstractParameter&) = delete;

        /// Applies the initial value through the same quantize/clamp path as
        /// setValue (reference constructor sets Value through the property).
        void initializeValue(int value);

        /// Regenerates the message bytes from the current value. Called with
        /// the parameter lock held; implementations use the raw accessors
        /// below and must not call public locked methods.
        virtual void updateMessageFromValue() = 0;

        [[nodiscard]] int rawValue() const { return _value; }
        [[nodiscard]] std::vector<std::uint8_t>& rawMessageBytes() { return _messageBytes; }

    private:
        void setValueLocked(int value);

        mutable std::mutex _mutex;
        std::string _name;
        std::string _label;
        int _minValue = 0;
        int _maxValue = 0;
        int _step = 1;
        int _value = 0;
        bool _changed = false;
        std::vector<std::uint8_t> _messageBytes;
    };
}
