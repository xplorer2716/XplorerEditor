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
using MidiApp.MidiController.Model;
using System.Collections;
using System.Threading;
using Xplorer.Model;

namespace Xplorer.Controller
{
    /// <summary>
    /// Application Controller - old school worker thread
    /// </summary>
    internal partial class XpanderController : AbstractController
    {
        /// <summary>
        /// WorkerThreadProc override
        /// </summary>
        protected override void WorkerThreadProc()
        {
            try
            {
                while (true)
                {
                    //wait for transmission delay to elapse
                    Thread.Sleep(this.ParameterTransmitDelay);

                    // iterate thru each parameter of the edited tone
                    // if value of parameter has changed since last scan, enqueue the parameter to send
                    foreach (DictionaryEntry entry in Tone.ParameterMap)
                    {
                        AbstractParameter parameter = (AbstractParameter)entry.Value;
                        if (parameter.Changed)
                        {
                            // this value is taken in account
                            parameter.Changed = false;
                            // enqueue a copy of the parameter, to keep an image of it's current state
                            // value can change between now and the time when it'll be really sent.
                            AbstractParameter clone = (AbstractParameter)parameter.Clone();
                            EnQueueParameter(clone);
                        }
                    }

                    // dequeue the oldest parameter and send it
                    AbstractParameter paramToSend;
                    if (DequeueParameter(out paramToSend))
                    {
                        if (VerifySynthOutputDevice())
                        {
                            // send page select only if needed
                            int lastPageSelected, lastSubPageSelected;
                            _pageSubPageHelper.GetPageSubPage(out lastPageSelected, out lastSubPageSelected);
                            if ((lastPageSelected != ((XpanderParameter)paramToSend).Page) ||
                                 (lastSubPageSelected != ((XpanderParameter)paramToSend).SubPage))
                            {
                                // send it synchronously right now
                                SendDataToSynthOutputDevice(((XpanderParameter)paramToSend).PageSelectMessage);
                                _pageSubPageHelper.UpdatePageSubPage(((XpanderParameter)paramToSend).Page, ((XpanderParameter)paramToSend).SubPage);
                                Thread.Sleep(this.ParameterTransmitDelay);
                            }

                            SendDataToSynthOutputDevice(paramToSend.Message);
                        }
                    } // dequeue
                } // while
            }//try
            catch (ThreadAbortException)
            {
                //thread abortion (call to StopWorkerThread())
            }
        }
    }
}