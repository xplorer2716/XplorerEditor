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
namespace Xplorer.View.Settings
{
    /// <summary>
    /// Contract for a settings page
    /// </summary>
    internal interface ISettingsPage
    {
        /// <summary>
        /// True if Initialize was already called
        /// </summary>
        bool IsInitialized { get; }

        /// <summary>
        /// Initialize the page
        /// </summary>
        void Initialize();

        /// <summary>
        /// set up internal state when OnOK is fired on the page host
        /// </summary>
        void ProcessOnOK();
    }
}