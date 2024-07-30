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

namespace Xplorer.Model
{
    /// <summary>
    /// an Xpander signed parameter (ex: VCO1 & 2  detune)
    /// </summary>
    internal sealed class XpanderSignedParameter : XpanderParameter
    {
        /// <summary>
        /// Updates the message from value.
        /// </summary>
        protected override void UpdateMessageFromValue()
        {
            byte byteValue = 0;
            if (Value < 0)
            {
                byteValue = (byte)(0x80 - System.Math.Abs(Value));
                _sysexMessage[SYSEX_VALUE_INDEX + 1] = 0x01;
            }
            else
            {
                byteValue = (byte)Value;
                _sysexMessage[SYSEX_VALUE_INDEX + 1] = 0x00;
            }

            _sysexMessage[SYSEX_VALUE_INDEX] = byteValue;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="XpanderSignedParameter" /> class.
        /// </summary>
        /// <param name="name">The name.</param>
        /// <param name="page">The page.</param>
        /// <param name="subPage">The sub page.</param>
        /// <param name="minValue">The min value.</param>
        /// <param name="maxValue">The max value.</param>
        /// <param name="step">The step.</param>
        /// <param name="message">The message.</param>
        /// <param name="value">The value.</param>
        /// <param name="label">The label.</param>
        public XpanderSignedParameter(string name, int page, int subPage, int minValue, int maxValue, int step, SysExMessage message, int value, string label)
            : base(name, page, subPage, minValue, maxValue, step, message, value, label)
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="XpanderSignedParameter"/> class.
        /// </summary>
        /// <param name="param">The param.</param>
        private XpanderSignedParameter(XpanderSignedParameter param)
            : base(param)
        {
        }

        /// <summary>
        /// Clones this instance.
        /// </summary>
        /// <returns></returns>
        public override object Clone()
        {
            return new XpanderSignedParameter(this);
        }
    }
}