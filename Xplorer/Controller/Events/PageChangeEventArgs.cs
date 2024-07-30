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
using System.Diagnostics;
using Xplorer.Common;

namespace Xplorer.Controller.Events
{
    /// <summary>
    /// A page change event arg
    /// </summary>
    internal class PageChangeEventArgs : EventArgs
    {
        private readonly string _pageName;
        private readonly int _subPage;

        /// <summary>
        /// Initializes a new instance of the <see cref="PageChangeEventArgs"/> class.
        /// </summary>
        /// <param name="page">The page.</param>
        /// <param name="subPage">The sub page.</param>
        internal PageChangeEventArgs(XpanderConstants.EnumPages page, int subPage)
        {
            _pageName = Enum.GetName(typeof(XpanderConstants.EnumPages), page);
            _subPage = subPage;
        }

        // do not use it
        private PageChangeEventArgs()
        {
            Debug.Fail("Do not use this ctor");
        }

        /// <summary>
        /// Gets the name of the page.
        /// </summary>
        /// <value>
        /// The name of the page.
        /// </value>
        internal string PageName
        {
            get
            {
                return _pageName;
            }
        }

        /// <summary>
        /// Gets the sub page.
        /// </summary>
        internal int SubPage
        {
            get
            {
                return _subPage;
            }
        }
    }
}