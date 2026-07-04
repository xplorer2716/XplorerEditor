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
#include "midiapp/model/AbstractTone.hpp"

#include <chrono>
#include <random>
#include <sstream>
#include <stdexcept>

namespace midiapp::model
{
    namespace
    {
        /// Faithful port of the reference GetNextRandomValueForParameter,
        /// including its quirks (see comments); the value setter clamps the
        /// result anyway. [RQ-FMW-011]
        int nextRandomValueForParameter(std::mt19937& randomizer, const AbstractParameter& parameter,
                                        std::optional<float> humanizeRatio)
        {
            const int minValue = parameter.minValue();
            const int maxValue = parameter.maxValue();
            const bool twoState = (maxValue - minValue) == 1;
            auto coinFlip = [&randomizer]
            {
                return std::uniform_real_distribution<double>(0.0, 1.0)(randomizer) > 0.5;
            };

            if (!humanizeRatio.has_value())
            {
                if (twoState)
                {
                    return coinFlip() ? maxValue : minValue;
                }
                return std::uniform_int_distribution<int>(minValue, maxValue)(randomizer);
            }

            if (twoState)
            {
                return coinFlip() ? maxValue : minValue;
            }

            // Reference: rand.Next(0,1) is always 0, so the "add" branch never
            // triggers; kept as-is for behavioral fidelity.
            const bool addValue = false;
            const int current = parameter.value();
            const int spread = static_cast<int>(static_cast<float>(current) * humanizeRatio.value());
            int low = current - spread;
            int high = current + spread;
            if (low > high)
            {
                std::swap(low, high); // negative currents invert the range
            }
            int value = std::uniform_int_distribution<int>(low, high)(randomizer);
            if (addValue)
            {
                value = value > maxValue ? maxValue : value;
            }
            else
            {
                value = value < minValue ? minValue : value;
            }
            return value;
        }
    }

    void AbstractTone::setMidiChannel(int channel)
    {
        if (channel < 0 || channel > 15)
        {
            throw std::out_of_range("MIDI channel out of range.");
        }
        _midiChannel = channel;
    }

    void AbstractTone::randomizeToneParameters(const std::set<std::string>& excludedParameterNames,
                                               std::optional<float> humanizeRatio,
                                               std::optional<unsigned int> seed)
    {
        std::mt19937 randomizer(seed.value_or(static_cast<unsigned int>(
            std::chrono::steady_clock::now().time_since_epoch().count())));

        for (const auto& entry : _parameterMap)
        {
            if (!excludedParameterNames.contains(entry.name))
            {
                entry.parameter->setValue(
                    nextRandomValueForParameter(randomizer, *entry.parameter, humanizeRatio));
            }
        }
    }

    void AbstractTone::morphTones(const AbstractTone& toneA, const AbstractTone& toneB,
                                  AbstractTone& resultTone, float morphingFactor)
    {
        for (const auto& parameterName : resultTone.eligibleParametersForToneMorphing())
        {
            const auto& parameterA = toneA.parameterMap().at(parameterName);
            const auto& parameterB = toneB.parameterMap().at(parameterName);
            auto resultParameter = parameterA.clone();
            resultParameter->setValue(static_cast<int>(
                (1.0F - morphingFactor) * static_cast<float>(parameterA.value())
                + morphingFactor * static_cast<float>(parameterB.value())));
            resultTone.parameterMap().replace(std::move(resultParameter));
        }
    }

    std::set<std::string> AbstractTone::eligibleParametersForToneMorphing() const
    {
        std::set<std::string> names;
        for (const auto& entry : _parameterMap)
        {
            names.insert(entry.name);
        }
        return names;
    }

    std::string AbstractTone::toString() const
    {
        std::ostringstream stream;
        stream << "ToneName: " << toneName() << " - MIDIChannel: " << _midiChannel << "\r\n";
        for (const auto& entry : _parameterMap)
        {
            stream << entry.name << "\t:" << entry.parameter->value() << "\r\n";
        }
        return stream.str();
    }
}
