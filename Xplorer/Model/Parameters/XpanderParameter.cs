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

/*
 page select: F0 10 02 0B 20 00 F7 //7
 param      : F0 10 02 0A 00 08 00 00 00 63 00 F7 //12
*/

namespace Xplorer.Model
{
    /// <summary>
    /// An Xpander unsigned parameter
    /// </summary>
    internal class XpanderParameter : AbstractParameter
    {
        private readonly object _xlockObject = new object();

        /// <summary>
        /// Index in Sysex Message where the _value is
        /// </summary>
        public const int SYSEX_VALUE_INDEX = 9; // 0 based

        /// <summary>
        /// Length of Sysex (all the same)
        /// </summary>
        internal const int SYSEX_MESSAGE_LENGTH = 12;

        internal const int SYSEX_BUTTON_ID = 5;

        protected internal SysExMessage _sysexMessage = null; //see ctor

        /// <summary>
        /// Gets or sets the message.
        /// </summary>
        /// <value>
        /// The message.
        /// </value>
        public override SysExMessage Message
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
            _sysexMessage[SYSEX_VALUE_INDEX] = (byte)Value;
        }

        /// <summary>
        /// Value override
        /// </summary>
        public override int Value
        {
            get
            {
                lock (_xlockObject)
                {
                    return base.Value;
                }
            }
            set
            {
                // do not use base lock object since we'll get a race condition
                lock (_xlockObject)
                {
                    base.Value = value;
                }
            }
        }

        /// <summary>
        /// Idem as setting the Value property but w/o setting the Changed flag to true
        /// </summary>
        /// <param name="value"></param>
        public void SetValueUnchanged(int value)
        {
            lock (_xlockObject)
            {
                base.Value = value;
                base.Changed = false;
            }
        }

        // parameter page
        private int _page;

        /// <summary>
        /// Gets or sets the page.
        /// </summary>
        /// <value>
        /// The page.
        /// </value>
        public int Page
        {
            get { return _page; }
            set { _page = value; }
        }

        // subpage
        private int _subPage;

        /// <summary>
        /// Gets or sets the sub page.
        /// </summary>
        /// <value>
        /// The sub page.
        /// </value>
        public int SubPage
        {
            get { return _subPage; }
            set { _subPage = value; }
        }

        private SysExMessage _PageSelectMessage = null;

        /// <summary>
        /// get the page select sysex message
        /// </summary>
        public SysExMessage PageSelectMessage
        {
            get
            {
                const int PAGE_INDEX = 4;
                const int SUBPAGE_INDEX = PAGE_INDEX + 1;
                if (_PageSelectMessage == null)
                {
                    _PageSelectMessage = new SysExMessage(new byte[] {
                        0xF0,0x10,0x02,0x0B,(byte)Page,(byte)SubPage, (byte)SysExType.Continuation});
                }
                else
                {
                    //only update the page/subpage
                    _PageSelectMessage[PAGE_INDEX] = (byte)Page;
                    _PageSelectMessage[SUBPAGE_INDEX] = (byte)SubPage;
                }
                return _PageSelectMessage;
            }
        }

        /// <summary>
        /// since all parameter simulates buttons value, each parameter has its button id for its page/subpage
        /// </summary>
        internal int ButtonID
        {
            get
            {
                return Message[SYSEX_BUTTON_ID];
            }
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="XpanderParameter"/> class.
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
        public XpanderParameter(string name, int page, int subPage, int minValue, int maxValue, int step, SysExMessage message, int value, string label = null)
            : base(name, minValue, maxValue, step, message, value, label)
        {
            _page = page;
            _subPage = subPage;
        }

        #region ICloneable Membres

        /// <summary>
        /// Initializes a new instance of the <see cref="XpanderParameter"/> class.
        /// </summary>
        /// <param name="param">The param.</param>
        protected XpanderParameter(XpanderParameter param)
            : base(param)
        {
            _page = param.Page;
            _subPage = param.SubPage;
        }

        /// <summary>
        /// this must be implemented by inheriters
        /// </summary>
        /// <returns></returns>
        public override object Clone()
        {
            return new XpanderParameter(this);
        }

        #endregion ICloneable Membres
    }
}