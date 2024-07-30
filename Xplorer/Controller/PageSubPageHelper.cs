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
using MidiApp.MidiController.Service;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using Xplorer.Common;

namespace Xplorer.Controller
{
#warning TODO PS réutiliser la structure PageSubPage

    /// <summary>
    /// A class to manage Page/Sub Page logic
    /// This class is thread safe
    /// </summary>
    internal sealed class PageSubPageHelper
    {
        // page/subpage where some rotary events are authorized for editing
        private readonly IDictionary<Tuple<int, int>, HashSet<XpanderConstants.EnumRotaryEncoders>> _authorizedRotaryEvents =
            new Dictionary<Tuple<int, int>, HashSet<XpanderConstants.EnumRotaryEncoders>>();

        // do not access directly page and subpage, always used locked accessors
        private int _lastPageSelected = (int)XpanderConstants.EnumPages.UNKNOWN;

        private int _lastSubPageSelected = (int)XpanderConstants.EnumPages.UNKNOWN;

        //page / subpage
        private readonly object _pageSubPageLockObject = new object();

        /// <summary>
        /// Ctor
        /// </summary>
        public PageSubPageHelper()
        {
            InitializeAuthorizedEditingRotaryEvents();
        }

        /// <summary>
        /// Sets the last page selected.
        /// </summary>
        /// <value>
        /// The last page selected.
        /// </value>
        internal int LastPageSelected
        {
            private get { return _lastPageSelected; }
            set
            {
                lock (_pageSubPageLockObject)
                {
                    _lastPageSelected = value;
                }
            }
        }

        /// <summary>
        /// Sets the last sub page selected.
        /// </summary>
        /// <value>
        /// The last sub page selected.
        /// </value>
        internal int LastSubPageSelected
        {
            private get { return _lastSubPageSelected; }
            set
            {
                lock (_pageSubPageLockObject)
                {
                    _lastSubPageSelected = value;
                }
            }
        }

        /// <summary>
        /// Gets the current page and sub page.
        /// </summary>
        /// <param name="page">The page.</param>
        /// <param name="subPage">The sub page.</param>
        internal void GetPageSubPage(out int page, out int subPage)
        {
            lock (_pageSubPageLockObject)
            {
                page = _lastPageSelected;
                subPage = _lastSubPageSelected;
            }
        }

        /// <summary>
        /// Determines whether [is authorized rotary] [the specified encoder].
        /// </summary>
        /// <param name="encoder">The encoder.</param>
        /// <returns>
        ///   <c>true</c> if [is authorized rotary] [the specified encoder]; otherwise, <c>false</c>.
        /// </returns>
        internal bool IsAuthorizedRotary(XpanderConstants.EnumRotaryEncoders encoder)
        {
            HashSet<XpanderConstants.EnumRotaryEncoders> authorized;
            int page, subpage;
            GetPageSubPage(out page, out subpage);

            bool isAuthorizedRotary = false;
            if (_authorizedRotaryEvents.TryGetValue(new Tuple<int, int>(page, subpage), out authorized))
            {
                if (authorized.Contains(encoder))
                {
                    isAuthorizedRotary = true;
                }
            }
            Logger.WriteLine(this, TraceLevel.Verbose, "IsAuthorizedRotary: " + isAuthorizedRotary.ToString());
            return isAuthorizedRotary;
        }

        /// <summary>
        /// Determines whether [is lfo retrig] [the specified parameter page].
        /// </summary>
        /// <remarks>
        /// for LFO retrig changes, do not translate as a button value, since editing
        /// can only be done thru rotary
        /// </remarks>
        /// <param name="parameterPage">The parameter page.</param>
        /// <param name="parameterSubPage">The parameter sub page.</param>
        /// <param name="buttonID">The button ID.</param>
        /// <returns>
        ///   <c>true</c> if [is lfo retrig] [the specified parameter page]; otherwise, <c>false</c>.
        /// </returns>
        internal bool IsLfoRetrig(int parameterPage, int parameterSubPage, int buttonID)
        {
            bool isPageLfo = (parameterPage >= (int)XpanderConstants.EnumPages.LFO_1) && (parameterSubPage <= (int)XpanderConstants.EnumPages.LFO_5);
            bool isRetrig = parameterSubPage == 0x01 && buttonID == (int)XpanderConstants.EnumRotaryEncoders.THIRD;

            Logger.WriteLine(this, TraceLevel.Verbose, "IsLfoRetrig: " + (isPageLfo && isRetrig).ToString());
            return isPageLfo && isRetrig;
        }

        /// <summary>
        /// Determines whether [is page env, lfo, ramp, or track].
        /// </summary>
        /// <returns>
        ///   <c>true</c> if [is page env, lfo, ramp, or track]; otherwise, <c>false</c>.
        /// </returns>
        internal bool IsPageEnvLfoRampTrack()
        {
            int page = LastPageSelected;
            bool isPageEnvLfoRampTrack = (page >= (int)XpanderConstants.EnumPages.ENV_1) && (page <= (int)XpanderConstants.EnumPages.RAMP_4);
            Logger.WriteLine(this, TraceLevel.Verbose, "IsPageEnvLfoRampTrack: " + isPageEnvLfoRampTrack.ToString());
            return isPageEnvLfoRampTrack;
        }

        /// <summary>
        /// Determines whether [is page lfo].
        /// </summary>
        /// <returns>
        ///   <c>true</c> if [is page lfo]; otherwise, <c>false</c>.
        /// </returns>
        internal bool IsPageLfo()
        {
            int page = LastPageSelected;
            bool isPageLfo = (page >= (int)XpanderConstants.EnumPages.LFO_1) && (page <= (int)XpanderConstants.EnumPages.LFO_5);
            Logger.WriteLine(this, TraceLevel.Verbose, "IsPageLfo: " + isPageLfo.ToString());
            return isPageLfo;
        }

        /// <summary>
        /// Updates the page / sub page.
        /// </summary>
        /// <param name="page">The page.</param>
        /// <param name="subPage">The sub page.</param>
        internal void UpdatePageSubPage(int page, int subPage)
        {
            lock (_pageSubPageLockObject)
            {
                _lastPageSelected = page;
                _lastSubPageSelected = subPage;
            }
        }

        /// <summary>
        /// Initializes the authorized rotary events (only for parameter editing, not modulation edit)
        /// </summary>
        private void InitializeAuthorizedEditingRotaryEvents()
        {
            Tuple<int, int> pageSubPage;
            HashSet<XpanderConstants.EnumRotaryEncoders> authorized;

            #region VCO 1

            // subpage 1
            pageSubPage = new Tuple<int, int>((int)XpanderConstants.EnumPages.VCO_1_X, 0);
            authorized = new HashSet<XpanderConstants.EnumRotaryEncoders> {
                XpanderConstants.EnumRotaryEncoders.FIRST,
                XpanderConstants.EnumRotaryEncoders.SECOND,
                XpanderConstants.EnumRotaryEncoders.FOURTH,
                XpanderConstants.EnumRotaryEncoders.SIXTH
            };
            _authorizedRotaryEvents.Add(pageSubPage, authorized);
            // none on subpage 2

            #endregion VCO 1

            #region VCO2

            pageSubPage = new Tuple<int, int>((int)XpanderConstants.EnumPages.VCO_2_X, 0);
            authorized = new HashSet<XpanderConstants.EnumRotaryEncoders> {
                XpanderConstants.EnumRotaryEncoders.FIRST,
                XpanderConstants.EnumRotaryEncoders.SECOND,
                XpanderConstants.EnumRotaryEncoders.FOURTH,
                XpanderConstants.EnumRotaryEncoders.SIXTH
            };
            _authorizedRotaryEvents.Add(pageSubPage, authorized);
            // none on subpage 2

            #endregion VCO2

            #region FM_LAG

            pageSubPage = new Tuple<int, int>((int)XpanderConstants.EnumPages.FM_LAG_X, 0);
            authorized = new HashSet<XpanderConstants.EnumRotaryEncoders> {
                XpanderConstants.EnumRotaryEncoders.FIRST,
                XpanderConstants.EnumRotaryEncoders.SECOND,
                XpanderConstants.EnumRotaryEncoders.FOURTH,
                XpanderConstants.EnumRotaryEncoders.FIFTH
            };
            _authorizedRotaryEvents.Add(pageSubPage, authorized);
            // subpage 2
            pageSubPage = new Tuple<int, int>((int)XpanderConstants.EnumPages.FM_LAG_X, 1);
            authorized = new HashSet<XpanderConstants.EnumRotaryEncoders> {
                XpanderConstants.EnumRotaryEncoders.FIRST
            };
            _authorizedRotaryEvents.Add(pageSubPage, authorized);

            #endregion FM_LAG

            #region TRACK_X

            authorized = new HashSet<XpanderConstants.EnumRotaryEncoders> {
                XpanderConstants.EnumRotaryEncoders.FIRST,
                XpanderConstants.EnumRotaryEncoders.SECOND,
                XpanderConstants.EnumRotaryEncoders.THIRD,
                XpanderConstants.EnumRotaryEncoders.FOURTH,
                XpanderConstants.EnumRotaryEncoders.FIFTH,
                XpanderConstants.EnumRotaryEncoders.SIXTH
            };
            for (int i = (int)XpanderConstants.EnumPages.TRACK_1; i < (int)XpanderConstants.EnumPages.TRACK_3 + 1; i++)
            {
                pageSubPage = new Tuple<int, int>(i, 0);
                _authorizedRotaryEvents.Add(pageSubPage, authorized);
            }
            // none on subpage 2

            #endregion TRACK_X

            #region VCF_VCA

            authorized = new HashSet<XpanderConstants.EnumRotaryEncoders> {
                XpanderConstants.EnumRotaryEncoders.FIRST,
                XpanderConstants.EnumRotaryEncoders.SECOND,
                XpanderConstants.EnumRotaryEncoders.THIRD,
                XpanderConstants.EnumRotaryEncoders.FIFTH,
                XpanderConstants.EnumRotaryEncoders.SIXTH
            };
            pageSubPage = new Tuple<int, int>((int)XpanderConstants.EnumPages.VCF_VCA_X, 0);
            _authorizedRotaryEvents.Add(pageSubPage, authorized);
            //none on subpage 2

            #endregion VCF_VCA

            #region ENV_X

            authorized = new HashSet<XpanderConstants.EnumRotaryEncoders> {
                XpanderConstants.EnumRotaryEncoders.FIRST,
                XpanderConstants.EnumRotaryEncoders.SECOND,
                XpanderConstants.EnumRotaryEncoders.THIRD,
                XpanderConstants.EnumRotaryEncoders.FOURTH,
                XpanderConstants.EnumRotaryEncoders.FIFTH,
                XpanderConstants.EnumRotaryEncoders.SIXTH
            };
            for (int i = (int)XpanderConstants.EnumPages.ENV_1; i < (int)XpanderConstants.EnumPages.ENV_5 + 1; i++)
            {
                pageSubPage = new Tuple<int, int>(i, 0);
                _authorizedRotaryEvents.Add(pageSubPage, authorized);
            }
            // subpage 2
            authorized = new HashSet<XpanderConstants.EnumRotaryEncoders> {
                XpanderConstants.EnumRotaryEncoders.SECOND,
                XpanderConstants.EnumRotaryEncoders.FIFTH,
            };
            for (int i = (int)XpanderConstants.EnumPages.ENV_1; i < (int)XpanderConstants.EnumPages.ENV_5 + 1; i++)
            {
                pageSubPage = new Tuple<int, int>(i, 1);
                _authorizedRotaryEvents.Add(pageSubPage, authorized);
            }

            #endregion ENV_X

            #region LFO_X

            authorized = new HashSet<XpanderConstants.EnumRotaryEncoders> {
                XpanderConstants.EnumRotaryEncoders.FIRST,
                XpanderConstants.EnumRotaryEncoders.SECOND,
                XpanderConstants.EnumRotaryEncoders.THIRD,
                XpanderConstants.EnumRotaryEncoders.FOURTH,
                XpanderConstants.EnumRotaryEncoders.SIXTH
            };
            for (int i = (int)XpanderConstants.EnumPages.LFO_1; i < (int)XpanderConstants.EnumPages.LFO_5 + 1; i++)
            {
                pageSubPage = new Tuple<int, int>(i, 0);
                _authorizedRotaryEvents.Add(pageSubPage, authorized);
            }
            //subpage 2
            authorized = new HashSet<XpanderConstants.EnumRotaryEncoders> {
                XpanderConstants.EnumRotaryEncoders.THIRD
            };
            for (int i = (int)XpanderConstants.EnumPages.LFO_1; i < (int)XpanderConstants.EnumPages.LFO_5 + 1; i++)
            {
                pageSubPage = new Tuple<int, int>(i, 1);
                _authorizedRotaryEvents.Add(pageSubPage, authorized);
            }

            #endregion LFO_X

            #region RAMP_X

            authorized = new HashSet<XpanderConstants.EnumRotaryEncoders> {
                XpanderConstants.EnumRotaryEncoders.SECOND,
            };
            for (int i = (int)XpanderConstants.EnumPages.RAMP_1; i < (int)XpanderConstants.EnumPages.RAMP_4 + 1; i++)
            {
                pageSubPage = new Tuple<int, int>(i, 0);
                _authorizedRotaryEvents.Add(pageSubPage, authorized);
            }
            // subpage 2
            authorized = new HashSet<XpanderConstants.EnumRotaryEncoders> {
                XpanderConstants.EnumRotaryEncoders.SECOND,
                XpanderConstants.EnumRotaryEncoders.FIFTH,
            };
            for (int i = (int)XpanderConstants.EnumPages.RAMP_1; i < (int)XpanderConstants.EnumPages.RAMP_4 + 1; i++)
            {
                pageSubPage = new Tuple<int, int>(i, 1);
                _authorizedRotaryEvents.Add(pageSubPage, authorized);
            }

            #endregion RAMP_X
        }
    }
}