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

#include "midiapp/model/AbstractTone.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace midiapp::model
{
    /// Port of MidiApp ToneException: raised by tone I/O on invalid content.
    class ToneException : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };

    /// Port of IToneReader. [RQ-FMW-020]
    class IToneReader
    {
    public:
        virtual ~IToneReader() = default;

        /// Loads one tone file into an existing tone. @throws ToneException.
        virtual void readTone(const std::string& filename, AbstractTone& tone) = 0;

        /// Enumerates all tones of a bank file as (name, tone) pairs.
        [[nodiscard]] virtual std::vector<std::pair<std::string, std::unique_ptr<AbstractTone>>>
        readTones(const std::string& filename) = 0;
    };

    /// Port of IToneWriter. [RQ-FMW-020]
    class IToneWriter
    {
    public:
        virtual ~IToneWriter() = default;

        /// Writes the tone to a file. @throws ToneException.
        virtual void writeTone(const std::string& filename, const AbstractTone& tone) = 0;
    };
}
