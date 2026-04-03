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

using MidiApp.UIControls;
using System.Diagnostics;
using System.Windows.Forms;

namespace Xplorer.View
{
    /// <summary>
    /// Manages Xpander trigger mutual-exclusion UI rules for ENV and RAMP pages.
    /// Enforces the rule that EXTRIG and LFOTRIG cannot be simultaneously active,
    /// and controls the enabled state of dependent controls accordingly.
    /// Both ENV and RAMP pages share the same two rule methods; callers pass the
    /// relevant controls as arguments.
    /// </summary>
    internal sealed class TriggerRuleManager
    {
        private readonly MainForm _form;

        internal TriggerRuleManager(MainForm form)
        {
            Debug.Assert(form != null);
            _form = form;
        }

        /// <summary>
        /// Applies enable/disable and mutual-exclusion rules when an EXTRIG control is changed.
        /// Used by ENV_X_TRIG_EXTRIG_CheckedChanged and RAMP_X_TRIG_EXTRIG_CheckedChanged.
        /// </summary>
        /// <param name="exTrigControl">The EXTRIG checkbox control that was changed.</param>
        /// <param name="lfoTrigControl">The LFOTRIG checkbox control (mutually exclusive with EXTRIG).</param>
        /// <param name="lfoSourceControl">The LFO source selector control.</param>
        /// <param name="gatedControl">The GATED control whose enabled state depends on trigger selection.</param>
        internal void ApplyExTrigRule(
            CheckBoxValuedControl exTrigControl,
            CheckBoxValuedControl lfoTrigControl,
            Control lfoSourceControl,
            Control gatedControl)
        {
            if (exTrigControl.Checked)
            {
                lfoTrigControl.Checked = false;
                lfoSourceControl.Enabled = false;
                gatedControl.Enabled = true;
            }
            else
            {
                lfoSourceControl.Enabled = false;
                if (!lfoTrigControl.Checked)
                {
                    gatedControl.Enabled = false;
                }
            }
        }

        /// <summary>
        /// Applies enable/disable and mutual-exclusion rules when an LFOTRIG control is changed.
        /// Used by ENV_X_TRIG_LFOTRIG_CheckedChanged and RAMP_X_TRIG_LFOTRIG_CheckedChanged.
        /// </summary>
        /// <param name="exTrigControl">The EXTRIG checkbox control (mutually exclusive with LFOTRIG).</param>
        /// <param name="lfoTrigControl">The LFOTRIG checkbox control that was changed.</param>
        /// <param name="lfoSourceControl">The LFO source selector control.</param>
        /// <param name="gatedControl">The GATED control whose enabled state depends on trigger selection.</param>
        internal void ApplyLfoTrigRule(
            CheckBoxValuedControl exTrigControl,
            CheckBoxValuedControl lfoTrigControl,
            Control lfoSourceControl,
            Control gatedControl)
        {
            if (lfoTrigControl.Checked)
            {
                exTrigControl.Checked = false;
                lfoSourceControl.Enabled = true;
                gatedControl.Enabled = true;
            }
            else
            {
                lfoSourceControl.Enabled = false;
                if (!exTrigControl.Checked)
                {
                    gatedControl.Enabled = false;
                }
            }
        }
    }
}
