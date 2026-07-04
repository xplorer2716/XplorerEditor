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
#include "midiapp/controller/DualDictionary.hpp"

#include <algorithm>

namespace midiapp::controller
{
    void DualDictionary::add(const std::string& parameterName, int ccNumber)
    {
        const std::lock_guard lock(_mutex);
        if (const auto previous = _ccByName.find(parameterName); previous != _ccByName.end())
        {
            auto& names = _namesByCc[previous->second];
            std::erase(names, parameterName);
        }
        _ccByName[parameterName] = ccNumber;
        _namesByCc[ccNumber].push_back(parameterName);
    }

    void DualDictionary::remove(const std::string& parameterName)
    {
        const std::lock_guard lock(_mutex);
        const auto found = _ccByName.find(parameterName);
        if (found == _ccByName.end())
        {
            return;
        }
        auto& names = _namesByCc[found->second];
        std::erase(names, parameterName);
        _ccByName.erase(found);
    }

    void DualDictionary::clear()
    {
        const std::lock_guard lock(_mutex);
        _ccByName.clear();
        _namesByCc.clear();
    }

    std::optional<int> DualDictionary::ccNumberFor(const std::string& parameterName) const
    {
        const std::lock_guard lock(_mutex);
        const auto found = _ccByName.find(parameterName);
        return found == _ccByName.end() ? std::nullopt : std::make_optional(found->second);
    }

    std::vector<std::string> DualDictionary::parameterNamesFor(int ccNumber) const
    {
        const std::lock_guard lock(_mutex);
        const auto found = _namesByCc.find(ccNumber);
        return found == _namesByCc.end() ? std::vector<std::string>{} : found->second;
    }

    std::size_t DualDictionary::size() const
    {
        const std::lock_guard lock(_mutex);
        return _ccByName.size();
    }

    std::vector<std::pair<std::string, int>> DualDictionary::entries() const
    {
        const std::lock_guard lock(_mutex);
        std::vector<std::pair<std::string, int>> result;
        result.reserve(_ccByName.size());
        for (const auto& [name, cc] : _ccByName)
        {
            result.emplace_back(name, cc);
        }
        return result;
    }
}
