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

#include "xpl/midi/MidiPorts.hpp"

#include <memory>

namespace xpl::midi
{
    /// MidiBackend implementation over juce::MidiInput / juce::MidiOutput.
    /// This header stays JUCE-free ([RQ-MID-040]); all JUCE types live in the
    /// implementation, compiled into the xpl_midi_juce library. [ADR-JUC-004]
    ///
    /// Devices are addressed by display name; when several devices share a
    /// name the first enumerated one is used, mirroring the reference
    /// behavior of matching on MidiInCaps/MidiOutCaps name. [RQ-MID-001..002]
    class JuceMidiBackend final : public MidiBackend
    {
    public:
        JuceMidiBackend();
        ~JuceMidiBackend() override;

        [[nodiscard]] std::vector<std::string> inputDeviceNames() const override;
        [[nodiscard]] std::vector<std::string> outputDeviceNames() const override;
        [[nodiscard]] std::unique_ptr<MidiInputPort> openInput(const std::string& deviceName) override;
        [[nodiscard]] std::unique_ptr<MidiOutputPort> openOutput(const std::string& deviceName) override;

    private:
        struct Impl;
        std::unique_ptr<Impl> _impl;
    };
}
