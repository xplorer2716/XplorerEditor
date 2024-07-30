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
using MidiApp.MidiController.Model;
using Sanford.Multimedia.Midi;
using System.Diagnostics;

namespace Xplorer.Model
{
    /// <summary>
    /// A full tone sysex as a parameter to respect the queueing mechanism
    /// </summary>
    /// <remarks>This is not an XpanderParamter since page/subpage means nothing here</remarks>
    internal sealed class XpanderFullToneParameter : AbstractParameter
    {
        // local lock object
        private readonly object _xlockObject = new object();

        private SysExMessage _sysexMessage = null; //see ctor

        /// <summary>
        /// Gets or sets the message.
        /// </summary>
        /// <value>
        /// The message.
        /// </value>
        public override Sanford.Multimedia.Midi.SysExMessage Message
        {
            get
            {
                lock (_xlockObject)
                {
                    return _sysexMessage;
                }
            }
            set
            {
                lock (_xlockObject)
                {
                    Debug.Assert(value != null);
                    _sysexMessage = value;
                }
            }
        }

        /// <summary>
        /// Updates the message from value.
        /// </summary>
        protected override void UpdateMessageFromValue()
        {
            // do nothing since value, min & max means nothing here
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="XpanderFullToneParameter" /> class.
        /// </summary>
        /// <param name="message">The message.</param>
        public XpanderFullToneParameter(SysExMessage message)
            : base("FullTone", 0, 1, 1, message, 0)
        {
            Changed = true;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="XpanderFullToneParameter"/> class.
        /// </summary>
        /// <param name="param">The param.</param>
        private XpanderFullToneParameter(XpanderFullToneParameter param)
            : base(param)
        {
            Changed = true;
        }

        /// <summary>
        /// this must be implementer by inheriters
        /// </summary>
        /// <returns></returns>
        public override object Clone()
        {
            return new XpanderFullToneParameter(this);
        }
    }
}