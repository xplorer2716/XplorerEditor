/*
Xplorer - A real-time editor for the Oberheim Xpander and Matrix-12 synths
Copyright (C) 2012-2024 Pascal Schmitt

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

using Sanford.Multimedia.Midi;
using System;
using Xplorer.Common;

namespace Xplorer.Model
{
    /// <summary>
    /// A mod matrix parameter (amount, quantize)
    /// </summary>
    internal sealed class XpanderModMatrixParameter : XpanderParameter
    {
        // index of IDSource in the sysex message
        private static readonly int ID_SOURCE_INDEX = 5;

        private static readonly int MODULATION_CMD_INDEX = 7;
        private static readonly int MODULATION_SOURCE_INDEX = 9; // 2 bytes for cmd, next 2 bytes for source, but always in the val_lo byte

        /// <summary>
        /// Initializes a new instance of the <see cref="XpanderModMatrixParameter"/> class.
        /// </summary>
        /// <param name="name">The name.</param>
        /// <param name="page">The page.</param>
        /// <param name="subPage">The sub page.</param>
        /// <param name="minValue">The min value.</param>
        /// <param name="maxValue">The max value.</param>
        /// <param name="step">The step.</param>
        /// <param name="message">The message.</param>
        /// <param name="value">The value.</param>
        public XpanderModMatrixParameter(string name, int page, int subPage, int minValue, int maxValue, int step, SysExMessage message, int value)
            : base(name, page, subPage, minValue, maxValue, step, message, value)
        {
            // to avoid auto send of mod src...
            this.Changed = false;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="XpanderModMatrixParameter"/> class.
        /// </summary>
        /// <param name="param">The param.</param>
        private XpanderModMatrixParameter(XpanderParameter param)
            : base(param)
        {
        }

        /// <summary>
        /// Clone implementation
        /// </summary>
        /// <returns></returns>
        public override object Clone()
        {
            return new XpanderModMatrixParameter(this);
        }

        /// <summary>
        /// Defines the IdSource of the parameter
        /// </summary>
        /// <value>
        /// The ID source.
        /// </value>
        public int IDSource
        {
            set
            {
                this.Message[ID_SOURCE_INDEX] = (byte)value;
            }
            get
            {
                return this.Message[ID_SOURCE_INDEX];
            }
        }

        /// <summary>
        /// for debug purpose
        /// </summary>
        /// <returns>
        /// A <see cref="System.String"/> that represents this instance.
        /// </returns>
        public override string ToString()
        {
            XpanderConstants.EnumModulationSources source = (XpanderConstants.EnumModulationSources)this.Message[MODULATION_SOURCE_INDEX];
            XpanderConstants.EnumModulationDestinations destination = XpanderConstants.ModulationDestinationForPageSubPage(this.Page, this.SubPage);
            XpanderConstants.EnumModulationEditCommands command = (XpanderConstants.EnumModulationEditCommands)this.Message[MODULATION_CMD_INDEX];

            string message;
            if (command == XpanderConstants.EnumModulationEditCommands.ADDSOURCE || command == XpanderConstants.EnumModulationEditCommands.CHANGESOURCE)
            {
                message = string.Format("page={0}, subpage={1}, cmd={2}, id={3}, src={4}, dest={5}, value={6}, msg={7}",
                   Enum.GetName(XpanderConstants.PagesType, (XpanderConstants.EnumPages)this.Page),
                   this.SubPage,
                   Enum.GetName(XpanderConstants.ModulationEditCommandsType, command),
                   IDSource,
                   Enum.GetName(XpanderConstants.ModulationSourcesType, source),
                   Enum.GetName(XpanderConstants.ModulationDestinationType, destination),
                   this.Value, this.Message.ToString());
            }
            else
            {
                message = string.Format("page={0}, subpage={1}, cmd={2}, id={3},  dest={4}, value={5}, msg={6}",
                   Enum.GetName(XpanderConstants.PagesType, (XpanderConstants.EnumPages)this.Page),
                   this.SubPage,
                   Enum.GetName(XpanderConstants.ModulationEditCommandsType, command),
                   IDSource,
                   Enum.GetName(XpanderConstants.ModulationDestinationType, destination),
                   this.Value, this.Message.ToString());
            }

            return message;
        }
    }
}