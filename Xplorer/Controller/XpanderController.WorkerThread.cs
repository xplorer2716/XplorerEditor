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
using MidiApp.MidiController.Controller;
using MidiApp.MidiController.Model;
using System.Threading;
using Xplorer.Model;

namespace Xplorer.Controller
{
    /// <summary>
    /// Application Controller - worker thread
    /// </summary>
    internal partial class XpanderController : AbstractController
    {
        /// <summary>
        /// WorkerThreadProc override.
        /// Cooperative shutdown via <see cref="AbstractController.IsWorkerThreadStopRequested"/>;
        /// Thread.Abort() is not supported on .NET 5+ so ThreadAbortException is never raised.
        /// </summary>
        protected override void WorkerThreadProc()
        {
            while (!IsWorkerThreadStopRequested)
            {
                Thread.Sleep(ParameterTransmitDelay);
                ScanAndEnqueueChangedParameters();
                TrySendNextParameter();
            }
        }

        /// <summary>
        /// Scans all tone parameters and enqueues a clone of each one whose value has changed
        /// since the last scan cycle.
        /// </summary>
        private void ScanAndEnqueueChangedParameters()
        {
            foreach (AbstractParameter parameter in Tone.ParameterMap.Values)
            {
                if (!parameter.Changed)
                    continue;

                // Mark as taken into account before cloning so a concurrent
                // UI update on the same parameter is not silently dropped.
                parameter.Changed = false;
                EnQueueParameter((AbstractParameter)parameter.Clone());
            }
        }

        /// <summary>
        /// Dequeues the oldest pending parameter and sends it to the synth output device,
        /// issuing a page-select message first when the target page or sub-page has changed.
        /// </summary>
        private void TrySendNextParameter()
        {
            if (!DequeueParameter(out AbstractParameter paramToSend))
                return;

            if (!VerifySynthOutputDevice())
                return;

            var xpanderParam = (XpanderParameter)paramToSend;

            // Send page select only when the page or sub-page has changed.
            _pageSubPageHelper.GetPageSubPage(out int lastPage, out int lastSubPage);
            if (lastPage != xpanderParam.Page || lastSubPage != xpanderParam.SubPage)
            {
                SendDataToSynthOutputDevice(xpanderParam.PageSelectMessage);
                _pageSubPageHelper.UpdatePageSubPage(xpanderParam.Page, xpanderParam.SubPage);
                Thread.Sleep(ParameterTransmitDelay);
            }

            SendDataToSynthOutputDevice(paramToSend.Message);
        }
    }
}