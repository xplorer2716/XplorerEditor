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
using MidiApp.MidiController.Controller;
using Xplorer.Model;

namespace Xplorer.Controller
{
    /// <summary>
    /// page clipboard to enable some page copy/paste
    /// </summary>
    internal partial class XpanderController : AbstractController
    {
        private string _clipboardSource = null;

        /// <summary>
        /// get/set the clip board source
        /// </summary>
        /// <value>
        /// The clipboard source.
        /// </value>
        internal string ClipboardSource
        {
            get
            {
                return _clipboardSource;
            }
            set
            {
                _clipboardSource = value;
            }
        }

        //
        /// <summary>
        /// returns true if clipboard can be pasted to specified destination
        /// </summary>
        /// <param name="destination"></param>
        /// <returns></returns>
        internal bool CanClipboardPasteTo(string destination)
        {
            bool canPaste = false;
            const int COMPARELENGTH = 4;
            if (_clipboardSource.Length != 0)
            {
                if (_clipboardSource.Substring(0, COMPARELENGTH) == destination.Substring(0, COMPARELENGTH) && (destination != _clipboardSource))
                {
                    canPaste = true;
                }
            }
            else
            {
                canPaste = false;
            }
            return canPaste;
        }

        //
        /// <summary>
        /// clear the clipboard
        /// </summary>
        internal void ClearClipboard()
        {
            _clipboardSource = string.Empty;
        }

        //
        /// <summary>
        /// Determines whether [is clipboard empty].
        /// </summary>
        /// <returns>
        ///   <c>true</c> if [is clipboard empty]; otherwise, <c>false</c>.
        /// </returns>
        internal bool IsClipboardEmpty()
        {
            return _clipboardSource.Length == 0;
        }

        //
        /// <summary>
        /// paste clipboard to page destination
        /// </summary>
        /// <param name="destination">The destination.</param>
        internal void PasteClipboardTo(string destination)
        {
            // check for preconditions
            if (!CanClipboardPasteTo(destination)) return;

            // source and destination number
            string sourceNumber, destinationNumber;

            int numberPosition = 0;
            // since source and dest are compatible, extracting the number follows the same rules
            if (destination.StartsWith("ENV_") || destination.StartsWith("LFO_"))
            {
                numberPosition = 4;
                destinationNumber = destination.Substring(numberPosition, 1);
                sourceNumber = _clipboardSource.Substring(numberPosition, 1);
            }
            else if (destination.StartsWith("TRACK_"))
            {
                numberPosition = 6;
                destinationNumber = destination.Substring(numberPosition, 1);
                sourceNumber = _clipboardSource.Substring(numberPosition, 1);
            }
            else if (destination.StartsWith("RAMP_"))
            {
                numberPosition = 5;
                destinationNumber = destination.Substring(numberPosition, 1);
                sourceNumber = _clipboardSource.Substring(numberPosition, 1);
            }
            else
            {
                return;
            }

            // iterates thru the parameters
            // replace each dest parameter with source value
            foreach (XpanderParameter destinationParameter in Tone.ParameterMap.Values)
            {
                if (destinationParameter.Name.StartsWith(destination))
                {
                    string sourceParameterName = string.Format("{0}{1}{2}", destinationParameter.Name.Substring(0, numberPosition), sourceNumber, destinationParameter.Name.Substring(numberPosition + 1));
                    XpanderParameter SourceParameter = (XpanderParameter)Tone.ParameterMap[sourceParameterName];
                    destinationParameter.Value = SourceParameter.Value;
                    //simulate automation parameter change to update UI
                    NotifyAutomationParameterChangeEvent(new ParameterChangeEventArgs(destinationParameter.Name, destinationParameter.Value));
                }
            }
        }
    }
}