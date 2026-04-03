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
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Windows.Forms;
using Xplorer.Common;

namespace Xplorer.View
{
    /// <summary>
    /// Manages page-switching refresh logic for the ENV, LFO, RAMP and TRACK multi-page control
    /// families. Replaces the duplicated Radio_ENV/LFO/RAMP/TRACK_X_Click bodies with a single
    /// data-driven implementation driven by <see cref="XpanderConstants.PageFamilies"/>.
    /// Also provides the static tag ↔ parameter-name resolution methods, removing the hard-coded
    /// StartsWith chains and the #warning TODO that were in MainForm.Overrides.cs.
    /// </summary>
    internal sealed class PageRefreshManager
    {
        private readonly MainForm _form;

        // Control tags to refresh per page family, in the order they appeared in the original handlers.
        // Key = ControlTagPrefix (e.g. "ENV_X").
        private static readonly IReadOnlyDictionary<string, IReadOnlyList<string>> _pageTags =
            new Dictionary<string, IReadOnlyList<string>>
            {
                ["ENV_X"] = new List<string>
                {
                    "ENV_X_ATTACK", "ENV_X_DECAY", "ENV_X_DELAY",
                    "ENV_X_MODE_DADR", "ENV_X_MODE_FREERUN", "ENV_X_MODE_RESET",
                    "ENV_X_RELEASE", "ENV_X_SUSTAIN",
                    "ENV_X_TRIG_EXTRIG", "ENV_X_TRIG_GATED", "ENV_X_TRIG_LFOTRIG",
                    "ENV_X_TRIG_LFO_SOURCE", "ENV_X_TRIG_SINGLE_MULTI",
                    "ENV_X_VOLUME",
                },
                ["LFO_X"] = new List<string>
                {
                    "LFO_X_AMP", "LFO_X_LAG", "LFO_X_RETRIG",
                    "LFO_X_RETRIG_MODE", "LFO_X_SAMPLE_INPUT",
                    "LFO_X_SPEED", "LFO_X_WAVESHAPE",
                },
                ["RAMP_X"] = new List<string>
                {
                    "RAMP_X_RATE",
                    "RAMP_X_TRIG_EXTRIG", "RAMP_X_TRIG_GATED",
                    "RAMP_X_TRIG_LFO_SOURCE", "RAMP_X_TRIG_LFOTRIG",
                    "RAMP_X_TRIG_SINGLE_MULTI",
                },
                ["TRACK_X"] = new List<string>
                {
                    "TRACK_X_IN",
                    "TRACK_X_PT1", "TRACK_X_PT2", "TRACK_X_PT3",
                    "TRACK_X_PT4", "TRACK_X_PT5",
                },
            };

        internal PageRefreshManager(MainForm form)
        {
            Debug.Assert(form != null);
            _form = form;
        }

        /// <summary>
        /// Refreshes all valued controls for the page represented by <paramref name="button"/>.
        /// When <paramref name="e"/> is non-null the click originated from the user and a
        /// page-update SysEx is sent to the synth; when null the call comes from the controller.
        /// Replaces the bodies of Radio_ENV_X_Click, Radio_LFO_X_Click,
        /// Radio_RAMP_X_Click and Radio_TRACK_X_Click.
        /// </summary>
        internal void RefreshPage(RadioButton button, EventArgs e)
        {
            if (button == null) return;

            if (e != null)
            {
                _form.XController.SendPageUpdate((string)button.Tag);
            }

            string familyPrefix = GetFamilyPrefixForButton(button);
            if (familyPrefix == null) return;

            if (!_pageTags.TryGetValue(familyPrefix, out IReadOnlyList<string> tags))
                return;

            Dictionary<string, Control> controlsMap = _form.InternalRegisteredControlsMap;
            string firstTag = null;
            foreach (string tag in tags)
            {
                string parameterName = _form.ResolveParameterNameForTag(tag);
                if (controlsMap.TryGetValue(tag, out Control control))
                {
                    IValuedControl vc = control as IValuedControl;
                    if (vc != null)
                    {
                        vc.Value = _form.BaseController.GetParameter(parameterName).Value;
                        if (firstTag == null) firstTag = tag;
                    }
                }
            }

            if (firstTag != null && controlsMap.TryGetValue(firstTag, out Control firstControl))
            {
                _form.VfdDisplayHelper.UpdateState(firstControl as IValuedControl);
            }
        }

        /// <summary>
        /// Resolves a multi-page control tag (e.g. "ENV_X_ATTACK") to its concrete parameter name
        /// (e.g. "ENV_2_ATTACK") by finding the currently checked radio button.
        /// Non-multi-page tags are returned unchanged.
        /// Replaces the hard-coded StartsWith chains in GetParameterNameForValuedControlTag.
        /// </summary>
        internal static string ResolveParameterNameForTag(
            string valuedControlTag,
            Dictionary<string, RadioButton> pagesRadioButtonsMap)
        {
            string tag = valuedControlTag;

            foreach (PageFamilyDescriptor family in XpanderConstants.PageFamilies)
            {
                if (!tag.StartsWith(family.ControlTagPrefix)) continue;

                int prefixLen = family.ControlTagPrefix.Length;
                for (int iX = 1; iX <= family.Count; iX++)
                {
                    string radioName = tag.Substring(0, prefixLen).Replace('X', iX.ToString()[0]);
                    if (pagesRadioButtonsMap.TryGetValue(radioName, out RadioButton rdButton) && rdButton.Checked)
                    {
                        tag = tag.Replace("_X_", $"_{iX}_");
                        break;
                    }
                }
                break;
            }

            return tag;
        }

        /// <summary>
        /// Given a concrete parameter name (e.g. "ENV_2_ATTACK") returns the corresponding
        /// valued-control tag ("ENV_X_ATTACK") and radio-button name ("ENV_2").
        /// Returns false for non-multi-page parameter names.
        /// Replaces GetValuedControlAndButtonNameForParameterName, removing the #warning TODO.
        /// </summary>
        internal static bool TryResolveControlAndButtonNameForParameterName(
            string parameterName,
            out string valuedControlTag,
            out string radioButtonTag)
        {
            foreach (PageFamilyDescriptor family in XpanderConstants.PageFamilies)
            {
                if (!parameterName.StartsWith(family.ParameterNamePrefix)) continue;

                int digitIndex = family.DigitIndex;
                if (digitIndex >= parameterName.Length) break;

                valuedControlTag = parameterName.Substring(0, digitIndex) + "X" + parameterName.Substring(digitIndex + 1);
                radioButtonTag = parameterName.Substring(0, digitIndex + 1);
                return true;
            }

            valuedControlTag = null;
            radioButtonTag = null;
            return false;
        }

        // ── private helpers ──────────────────────────────────────────────

        /// <summary>
        /// Returns the ControlTagPrefix (e.g. "ENV_X") for the given radio button by matching
        /// its name against the known page families (e.g. "ENV_1" → "ENV_X").
        /// </summary>
        private static string GetFamilyPrefixForButton(RadioButton button)
        {
            string name = button.Name;
            foreach (PageFamilyDescriptor family in XpanderConstants.PageFamilies)
            {
                // ControlTagPrefix is e.g. "ENV_X"; strip "_X" to get radio prefix "ENV_"
                string radioPrefix = family.ControlTagPrefix.Replace("_X", "_");
                if (name.StartsWith(radioPrefix))
                    return family.ControlTagPrefix;
            }
            return null;
        }
    }
}
