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

using System.Collections.Generic;

namespace Xplorer.View.PatchVisualization
{
    /// <summary>
    /// Intermediate model for patch visualization, serializable to JSON
    /// </summary>
    internal sealed class PatchVisualizationModel
    {
        public string PatchName { get; set; }
        public int ProgramNumber { get; set; }
        public List<ModuleModel> Modules { get; set; } = [];
        public List<ModulationRouting> Modulations { get; set; } = [];
    }

    /// <summary>
    /// A synthesizer module (VCO, VCF, VCA, LFO, ENV, TRACK, RAMP, FM/LAG)
    /// </summary>
    internal sealed class ModuleModel
    {
        public string Id { get; set; }
        public string Name { get; set; }
        public string Type { get; set; }
        public int Index { get; set; }
        public List<ModuleParameter> Parameters { get; set; } = [];
    }

    /// <summary>
    /// A single parameter within a module
    /// </summary>
    internal sealed class ModuleParameter
    {
        public string Name { get; set; }
        public string DisplayValue { get; set; }
        public int RawValue { get; set; }
        public bool IsModulated { get; set; }
    }

    /// <summary>
    /// An active modulation routing from the modulation matrix
    /// </summary>
    internal sealed class ModulationRouting
    {
        public string Source { get; set; }
        public string Destination { get; set; }
        public string SourceModuleId { get; set; }
        public string DestModuleId { get; set; }
        public int Amount { get; set; }
        public int Quantize { get; set; }
    }
}
