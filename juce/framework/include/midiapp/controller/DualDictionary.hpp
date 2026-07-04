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

#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

namespace midiapp::controller
{
    /// Port of StringIntDualDictionary: the CC automation table. A parameter
    /// name maps to exactly one CC number; a CC number maps to several
    /// parameter names. Thread safe. [RQ-FMW-032]
    class DualDictionary
    {
    public:
        /// Adds or reassigns a (parameterName -> ccNumber) association; the
        /// name is first removed from its previous CC list (reference Add
        /// calls Remove).
        void add(const std::string& parameterName, int ccNumber);

        /// Removes the association of a parameter name, if any.
        void remove(const std::string& parameterName);

        void clear();

        /// CC number for a parameter name (reference returns int.MinValue
        /// when absent; typed as optional here).
        [[nodiscard]] std::optional<int> ccNumberFor(const std::string& parameterName) const;

        /// Parameter names automated by a CC number; empty when none.
        [[nodiscard]] std::vector<std::string> parameterNamesFor(int ccNumber) const;

        [[nodiscard]] std::size_t size() const;

        /// Snapshot of all (name, cc) pairs, for settings persistence.
        [[nodiscard]] std::vector<std::pair<std::string, int>> entries() const;

    private:
        mutable std::mutex _mutex;
        std::map<std::string, int> _ccByName;
        std::map<int, std::vector<std::string>> _namesByCc;
    };
}
