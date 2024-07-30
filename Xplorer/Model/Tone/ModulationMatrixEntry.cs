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
using System;
using Xplorer.Common;

namespace Xplorer.Model
{
    /// <summary>
    /// A modulation matrix entry
    /// </summary>
    internal class ModulationMatrixEntry
    {
        /// <summary>
        /// minimal _value for Amount
        /// </summary>
        public static readonly int MIN_AMOUNT = -63;

        /// <summary>
        /// max _value for Amount
        /// </summary>
        public static readonly int MAX_AMOUNT = 63;

        /// <summary>
        /// minimal _value for quantize
        /// </summary>
        public static readonly int MIN_QUANTIZE = 0;

        /// <summary>
        /// max _value for quantize
        /// </summary>
        public static readonly int MAX_QUANTIZE = 1;

        private int _amount;
        private int _quantize;

        /// <summary>
        /// Gets or sets the destination.
        /// </summary>
        /// <value>
        /// The destination.
        /// </value>
        internal XpanderConstants.EnumModulationDestinations Destination { get; set; }

        /// <summary>
        /// Gets or sets the source.
        /// </summary>
        /// <value>
        /// The source.
        /// </value>
        internal XpanderConstants.EnumModulationSourcesModMatrix Source { get; set; }

        // id of the source for the mod destination
        public int IdSource { get; set; }

        /// <summary>
        /// Gets or sets the amount.
        /// </summary>
        /// <value>
        /// The amount.
        /// </value>
        public int Amount
        {
            get { return _amount; }
            set
            {
                if (value < MIN_AMOUNT)
                {
                    _amount = MIN_AMOUNT;
                }
                else if (value > MAX_AMOUNT)
                {
                    _amount = MAX_AMOUNT;
                }
                else
                {
                    _amount = value;
                }
            }
        }

        /// <summary>
        /// Gets or sets the quantize.
        /// </summary>
        /// <value>
        /// The quantize.
        /// </value>
        public int Quantize
        {
            get { return _quantize; }
            set
            {
                if (value < MIN_QUANTIZE)
                {
                    _quantize = MIN_QUANTIZE;
                }
                else if (value > MAX_QUANTIZE)
                {
                    _quantize = MAX_QUANTIZE;
                }
                else
                {
                    _quantize = value;
                }
            }
        }

        /// <summary>
        /// Resets the Entry. Keep the destination as is, because it can reflect
        /// the actual state in the UI (eg: set NONE to VCF_FREQ, keep VCF_FREQ displayed)
        /// </summary>
        internal void Reset()
        {
            IdSource = -1;
            Source = XpanderConstants.EnumModulationSourcesModMatrix.NONE;
            Amount = 0;
            Quantize = 0;
        }

        /// <summary>
        /// Returns a <see cref="System.String"/> that represents this instance.
        /// </summary>
        /// <returns>
        /// A <see cref="System.String"/> that represents this instance.
        /// </returns>
        public override string ToString()
        {
            return string.Format(
                $"idsrc: {IdSource.ToString().PadLeft(2)}," +
                $" src: {Enum.GetName(XpanderConstants.ModulationSourcesModMatrixType, Source).PadLeft(4)}," +
                $" dest: {Enum.GetName(XpanderConstants.ModulationDestinationType, Destination).PadLeft(8)}," +
                $" amnt:{Amount.ToString().PadLeft(3)}," +
                $" qtz:{Quantize}" +
                $" {(Source == XpanderConstants.EnumModulationSourcesModMatrix.NONE ? "*FREE*" : string.Empty)}"
                );
        }
    }
}