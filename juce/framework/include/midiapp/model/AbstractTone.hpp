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

#include "midiapp/model/OrderedParameterMap.hpp"

#include <optional>
#include <set>
#include <string>

namespace midiapp::model
{
    /// Port of MidiApp.MidiController.Model.AbstractTone: the edited sound,
    /// owning its ordered parameter map and MIDI channel. [RQ-FMW-010]
    class AbstractTone
    {
    public:
        static constexpr int DEFAULT_MIDI_CHANNEL = 0;

        virtual ~AbstractTone() = default;

        /// @throws std::out_of_range outside [0, 15] (reference behavior).
        [[nodiscard]] int midiChannel() const { return _midiChannel; }
        void setMidiChannel(int channel);

        [[nodiscard]] virtual std::string toneName() const = 0;
        virtual void setToneName(const std::string& name) = 0;

        /// Derived constructors populate the map (the reference dispatches
        /// InitializeParameterMap from its base constructor; C++ cannot).
        [[nodiscard]] OrderedParameterMap& parameterMap() { return _parameterMap; }
        [[nodiscard]] const OrderedParameterMap& parameterMap() const { return _parameterMap; }

        /// Randomizes every non-excluded parameter within bounds; optional
        /// humanize keeps values near the current ones; two-state parameters
        /// coin-flip min/max. `seed` makes tests deterministic (the reference
        /// seeds from the clock). [RQ-FMW-011]
        virtual void randomizeToneParameters(const std::set<std::string>& excludedParameterNames,
                                             std::optional<float> humanizeRatio,
                                             std::optional<unsigned int> seed = std::nullopt);

        /// result[p] = (1-f)*a[p] + f*b[p] (float math, integer truncation)
        /// for each parameter eligible on `result`. [RQ-FMW-012]
        static void morphTones(const AbstractTone& toneA, const AbstractTone& toneB,
                               AbstractTone& resultTone, float morphingFactor);

        /// Defaults to every parameter; tones narrow it (reference virtual).
        [[nodiscard]] virtual std::set<std::string> eligibleParametersForToneMorphing() const;

        [[nodiscard]] std::string toString() const;

    protected:
        AbstractTone() = default;
        // Tones own unique parameters: movable, never copyable.
        AbstractTone(AbstractTone&&) noexcept = default;
        AbstractTone& operator=(AbstractTone&&) noexcept = default;
        AbstractTone(const AbstractTone&) = delete;
        AbstractTone& operator=(const AbstractTone&) = delete;

    private:
        int _midiChannel = DEFAULT_MIDI_CHANNEL;
        OrderedParameterMap _parameterMap;
    };
}
