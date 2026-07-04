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
#include "midiapp/model/OrderedParameterMap.hpp"

#include <stdexcept>

namespace midiapp::model
{
    void OrderedParameterMap::add(std::unique_ptr<AbstractParameter> parameter)
    {
        if (parameter == nullptr)
        {
            throw std::invalid_argument("parameter cannot be null");
        }
        const auto name = parameter->name();
        if (contains(name))
        {
            throw std::invalid_argument("duplicate parameter name: " + name);
        }
        _indexByName.emplace(name, _entries.size());
        _entries.push_back({name, std::move(parameter)});
    }

    bool OrderedParameterMap::contains(const std::string& name) const
    {
        return _indexByName.contains(name);
    }

    AbstractParameter* OrderedParameterMap::find(const std::string& name)
    {
        const auto found = _indexByName.find(name);
        return found == _indexByName.end() ? nullptr : _entries[found->second].parameter.get();
    }

    const AbstractParameter* OrderedParameterMap::find(const std::string& name) const
    {
        const auto found = _indexByName.find(name);
        return found == _indexByName.end() ? nullptr : _entries[found->second].parameter.get();
    }

    AbstractParameter& OrderedParameterMap::at(const std::string& name)
    {
        if (auto* parameter = find(name))
        {
            return *parameter;
        }
        throw std::out_of_range("no such parameter: " + name);
    }

    const AbstractParameter& OrderedParameterMap::at(const std::string& name) const
    {
        if (const auto* parameter = find(name))
        {
            return *parameter;
        }
        throw std::out_of_range("no such parameter: " + name);
    }

    void OrderedParameterMap::replace(std::unique_ptr<AbstractParameter> parameter)
    {
        if (parameter == nullptr)
        {
            throw std::invalid_argument("parameter cannot be null");
        }
        const auto found = _indexByName.find(parameter->name());
        if (found == _indexByName.end())
        {
            throw std::out_of_range("no such parameter: " + parameter->name());
        }
        _entries[found->second].parameter = std::move(parameter);
    }
}
