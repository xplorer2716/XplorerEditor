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
#include "midiapp/model/AbstractParameter.hpp"

#include <sstream>
#include <stdexcept>

namespace midiapp::model
{
    AbstractParameter::AbstractParameter(std::string name, int minValue, int maxValue, int step,
                                         xpl::midi::MidiMessage message, std::string label)
        : _name(std::move(name)),
          _label(std::move(label)),
          _minValue(minValue),
          _maxValue(maxValue),
          _messageBytes(message.bytes().begin(), message.bytes().end())
    {
        setStep(step);
    }

    AbstractParameter::AbstractParameter(const AbstractParameter& other)
    {
        const std::lock_guard lock(other._mutex);
        _name = other._name;
        _label = other._label;
        _minValue = other._minValue;
        _maxValue = other._maxValue;
        _step = other._step;
        _value = other._value;
        _changed = other._changed;
        _messageBytes = other._messageBytes;
    }

    std::string AbstractParameter::name() const
    {
        const std::lock_guard lock(_mutex);
        return _name;
    }

    std::string AbstractParameter::label() const
    {
        const std::lock_guard lock(_mutex);
        return _label;
    }

    void AbstractParameter::setLabel(const std::string& label)
    {
        const std::lock_guard lock(_mutex);
        _label = label;
    }

    int AbstractParameter::minValue() const
    {
        const std::lock_guard lock(_mutex);
        return _minValue;
    }

    int AbstractParameter::maxValue() const
    {
        const std::lock_guard lock(_mutex);
        return _maxValue;
    }

    int AbstractParameter::step() const
    {
        const std::lock_guard lock(_mutex);
        return _step;
    }

    void AbstractParameter::setStep(int step)
    {
        const std::lock_guard lock(_mutex);
        if (step == 0)
        {
            throw std::invalid_argument("Step can not be equal to 0");
        }
        _step = step;
    }

    int AbstractParameter::value() const
    {
        const std::lock_guard lock(_mutex);
        return _value;
    }

    void AbstractParameter::setValue(int value)
    {
        const std::lock_guard lock(_mutex);
        setValueLocked(value);
    }

    void AbstractParameter::initializeValue(int value)
    {
        const std::lock_guard lock(_mutex);
        setValueLocked(value);
    }

    void AbstractParameter::setValueLocked(int value)
    {
        // Reference: integer division keeps only the integral part, then
        // clamp; only a differing result stores, flags and rebuilds. [RQ-FMW-002]
        int computedValue = value / _step;
        computedValue *= _step;

        if (computedValue < _minValue)
        {
            computedValue = _minValue;
        }
        else if (computedValue > _maxValue)
        {
            computedValue = _maxValue;
        }

        if (computedValue != _value)
        {
            _value = computedValue;
            _changed = true;
            updateMessageFromValue();
        }
    }

    bool AbstractParameter::changed() const
    {
        const std::lock_guard lock(_mutex);
        return _changed;
    }

    void AbstractParameter::setChanged(bool changed)
    {
        const std::lock_guard lock(_mutex);
        _changed = changed;
    }

    xpl::midi::MidiMessage AbstractParameter::message() const
    {
        const std::lock_guard lock(_mutex);
        return xpl::midi::MidiMessage::fromRawBytes(_messageBytes);
    }

    std::string AbstractParameter::toString() const
    {
        const std::lock_guard lock(_mutex);
        std::ostringstream stream;
        stream << "Name:" << _name << " ,Min:" << _minValue << " ,Max:" << _maxValue
               << " ,Step:" << _step << " ,VALUE:" << _value;
        return stream.str();
    }
}
