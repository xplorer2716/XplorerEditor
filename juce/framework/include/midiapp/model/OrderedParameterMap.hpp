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

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace midiapp::model
{
    /// Insertion-ordered parameter map keyed by unique parameter name.
    /// Typed replacement for the reference's non-generic OrderedDictionary
    /// (architecture-analysis §8.2 item 6). [RQ-FMW-010]
    class OrderedParameterMap
    {
    public:
        struct Entry
        {
            std::string name;
            std::unique_ptr<AbstractParameter> parameter;
        };

        /// @throws std::invalid_argument on duplicate name or null parameter.
        void add(std::unique_ptr<AbstractParameter> parameter);

        [[nodiscard]] bool contains(const std::string& name) const;

        /// nullptr when absent.
        [[nodiscard]] AbstractParameter* find(const std::string& name);
        [[nodiscard]] const AbstractParameter* find(const std::string& name) const;

        /// @throws std::out_of_range when absent.
        [[nodiscard]] AbstractParameter& at(const std::string& name);
        [[nodiscard]] const AbstractParameter& at(const std::string& name) const;

        /// Replaces the parameter stored under its own name (used by tone
        /// morphing, which clones then overwrites). @throws when absent.
        void replace(std::unique_ptr<AbstractParameter> parameter);

        [[nodiscard]] std::size_t size() const { return _entries.size(); }
        [[nodiscard]] bool empty() const { return _entries.empty(); }

        /// Iteration in insertion order.
        [[nodiscard]] auto begin() const { return _entries.begin(); }
        [[nodiscard]] auto end() const { return _entries.end(); }

    private:
        std::vector<Entry> _entries;
        std::unordered_map<std::string, std::size_t> _indexByName;
    };
}
