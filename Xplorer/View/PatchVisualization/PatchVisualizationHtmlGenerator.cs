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

using MidiApp.MidiController.Model;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Reflection;
using System.Text;
using Xplorer.Common;
using Xplorer.Controller;

namespace Xplorer.View.PatchVisualization
{
    /// <summary>
    /// Extracts patch data from XpanderTone and generates a standalone HTML visualization file.
    /// </summary>
    internal static class PatchVisualizationHtmlGenerator
    {
        private const string TEMPLATE_FILENAME = "patch-visualizer-template.html";
        private const string DAGRE_FILENAME = "dagre.min.js";
        private const string JSON_PLACEHOLDER_BEGIN = "/*__PATCH_DATA_BEGIN__*/";
        private const string JSON_PLACEHOLDER_END = "/*__PATCH_DATA_END__*/";
        private const string DAGRE_PLACEHOLDER = "/*__DAGRE_JS__*/";

        // Mapping from modulation source enum name to the module id used in the visualization
        private static readonly Dictionary<string, string> _sourceToModuleId = new Dictionary<string, string>
        {
            { "KBD", "mod-kbd" }, { "LAG", "mod-fmlag" }, { "VEL", "mod-kbd" }, { "RVEL", "mod-kbd" }, { "PRES", "mod-kbd" },
            { "TRK1", "mod-trk1" }, { "TRK2", "mod-trk2" }, { "TRK3", "mod-trk3" },
            { "RMP1", "mod-rmp1" }, { "RMP2", "mod-rmp2" }, { "RMP3", "mod-rmp3" }, { "RMP4", "mod-rmp4" },
            { "ENV1", "mod-env1" }, { "ENV2", "mod-env2" }, { "ENV3", "mod-env3" }, { "ENV4", "mod-env4" }, { "ENV5", "mod-env5" },
            { "PED1", "mod-kbd" }, { "PED2", "mod-kbd" },
            { "LFO1", "mod-lfo1" }, { "LFO2", "mod-lfo2" }, { "LFO3", "mod-lfo3" }, { "LFO4", "mod-lfo4" }, { "LFO5", "mod-lfo5" },
            { "VIB", "mod-kbd" }, { "LEV1", "mod-kbd" }, { "LEV2", "mod-kbd" }
        };

        // Mapping from modulation destination enum name to the module id used in the visualization
        private static readonly Dictionary<string, string> _destToModuleId = new Dictionary<string, string>
        {
            { "VCO1_FRQ", "mod-vco1" }, { "VCO1_PW", "mod-vco1" }, { "VCO1_VOL", "mod-vco1" },
            { "VCO2_FRQ", "mod-vco2" }, { "VCO2_PW", "mod-vco2" }, { "VCO2_VOL", "mod-vco2" },
            { "VCF_FRQ", "mod-vcf" }, { "VCF_RES", "mod-vcf" },
            { "VCA1_VOL", "mod-vca1" }, { "VCA2_VOL", "mod-vca2" },
            { "LFO1_SPD", "mod-lfo1" }, { "LFO1_AMP", "mod-lfo1" },
            { "LFO2_SPD", "mod-lfo2" }, { "LFO2_AMP", "mod-lfo2" },
            { "LFO3_SPD", "mod-lfo3" }, { "LFO3_AMP", "mod-lfo3" },
            { "LFO4_SPD", "mod-lfo4" }, { "LFO4_AMP", "mod-lfo4" },
            { "LFO5_SPD", "mod-lfo5" }, { "LFO5_AMP", "mod-lfo5" },
            { "ENV1_DLY", "mod-env1" }, { "ENV1_ATK", "mod-env1" }, { "ENV1_DCY", "mod-env1" }, { "ENV1_REL", "mod-env1" }, { "ENV1_AMP", "mod-env1" },
            { "ENV2_DLY", "mod-env2" }, { "ENV2_ATK", "mod-env2" }, { "ENV2_DCY", "mod-env2" }, { "ENV2_REL", "mod-env2" }, { "ENV2_AMP", "mod-env2" },
            { "ENV3_DLY", "mod-env3" }, { "ENV3_ATK", "mod-env3" }, { "ENV3_DCY", "mod-env3" }, { "ENV3_REL", "mod-env3" }, { "ENV3_AMP", "mod-env3" },
            { "ENV4_DLY", "mod-env4" }, { "ENV4_ATK", "mod-env4" }, { "ENV4_DCY", "mod-env4" }, { "ENV4_REL", "mod-env4" }, { "ENV4_AMP", "mod-env4" },
            { "ENV5_DLY", "mod-env5" }, { "ENV5_ATK", "mod-env5" }, { "ENV5_DCY", "mod-env5" }, { "ENV5_REL", "mod-env5" }, { "ENV5_AMP", "mod-env5" },
            { "FM_AMP", "mod-fmlag" }, { "LAG_RATE", "mod-fmlag" }
        };

        /// <summary>
        /// Generates an HTML visualization file for the current patch and opens it in the default browser.
        /// </summary>
        internal static void GenerateAndOpen(XpanderController controller)
        {
            PatchVisualizationModel model = ExtractModel(controller);
            string json = SerializeToJson(model);
            string htmlContent = BuildHtml(json);
            string outputPath = GetOutputPath(model.PatchName);

            File.WriteAllText(outputPath, htmlContent, Encoding.UTF8);

            var psi = new ProcessStartInfo
            {
                FileName = outputPath,
                UseShellExecute = true
            };
            Process.Start(psi);
        }

        /// <summary>
        /// Extracts a PatchVisualizationModel from the current controller state.
        /// </summary>
        private static PatchVisualizationModel ExtractModel(XpanderController controller)
        {
            var model = new PatchVisualizationModel
            {
                PatchName = controller.ToneName,
                ProgramNumber = controller.CurrentProgramNumber
            };

            // Collect modulated destinations for marking parameters
            HashSet<string> modulatedDestinations = [];
            for (int i = 1; i <= XpanderConstants.MODENTRIES_COUNT; i++)
            {
                var entry = controller.GetModulationEntryByNumber(i);
                if (entry.Source != XpanderConstants.EnumModulationSourcesModMatrix.NONE)
                {
                    modulatedDestinations.Add(entry.Destination.ToString());
                }
            }

            // Tag→ModDest mapping for marking parameters as modulated (reuses same logic as MainForm)
            var tagToModDest = new Dictionary<string, string>
            {
                { "VCO1_FREQ", "VCO1_FRQ" }, { "VCO1_PW", "VCO1_PW" }, { "VCO1_VOLUME", "VCO1_VOL" },
                { "VCO2_FREQ", "VCO2_FRQ" }, { "VCO2_PW", "VCO2_PW" }, { "VCO2_VOLUME", "VCO2_VOL" },
                { "VCF_FREQ", "VCF_FRQ" }, { "VCF_RES", "VCF_RES" },
                { "VCF_VCA1_VOLUME", "VCA1_VOL" }, { "VCF_VCA2_VOLUME", "VCA2_VOL" },
                { "FM_AMP", "FM_AMP" }, { "FMLAG_RATE", "LAG_RATE" }
            };

            // Extract modules
            ExtractVcoModules(controller, model, tagToModDest, modulatedDestinations);
            ExtractVcfVcaModule(controller, model, tagToModDest, modulatedDestinations);
            ExtractFmLagModule(controller, model, tagToModDest, modulatedDestinations);

            for (int i = 1; i <= XpanderConstants.LFO_COUNT; i++)
                ExtractLfoModule(controller, model, i, modulatedDestinations);

            for (int i = 1; i <= XpanderConstants.ENV_COUNT; i++)
                ExtractEnvModule(controller, model, i, modulatedDestinations);

            for (int i = 1; i <= XpanderConstants.TRACK_COUNT; i++)
                ExtractTrackModule(controller, model, i);

            for (int i = 1; i <= XpanderConstants.RAMP_COUNT; i++)
                ExtractRampModule(controller, model, i);

            // Extract active modulation routings
            for (int i = 1; i <= XpanderConstants.MODENTRIES_COUNT; i++)
            {
                var entry = controller.GetModulationEntryByNumber(i);
                if (entry.Source == XpanderConstants.EnumModulationSourcesModMatrix.NONE)
                    continue;

                string srcName = entry.Source.ToString();
                string destName = entry.Destination.ToString();

                model.Modulations.Add(new ModulationRouting
                {
                    Source = srcName,
                    Destination = destName,
                    SourceModuleId = _sourceToModuleId.GetValueOrDefault(srcName, "mod-kbd"),
                    DestModuleId = _destToModuleId.GetValueOrDefault(destName, "mod-vco1"),
                    Amount = entry.Amount,
                    Quantize = entry.Quantize
                });
            }

            return model;
        }

        #region Module extraction helpers

        private static int GetParamValue(XpanderController controller, string paramName)
        {
            var param = controller.GetParameter(paramName);
            return param?.Value ?? 0;
        }

        private static ModuleParameter MakeParam(XpanderController controller, string paramKey, string displayName, bool isModulated)
        {
            int value = GetParamValue(controller, paramKey);
            return new ModuleParameter
            {
                Name = displayName,
                DisplayValue = value.ToString("00", CultureInfo.InvariantCulture),
                RawValue = value,
                IsModulated = isModulated
            };
        }

        private static bool IsModulated(string destEnumName, HashSet<string> modulatedDestinations)
        {
            return modulatedDestinations.Contains(destEnumName);
        }

        private static void ExtractVcoModules(XpanderController controller, PatchVisualizationModel model,
            Dictionary<string, string> tagToModDest, HashSet<string> modulatedDests)
        {
            for (int vco = 1; vco <= XpanderConstants.VCO_COUNT; vco++)
            {
                string prefix = string.Format("VCO{0}", vco);
                var module = new ModuleModel
                {
                    Id = string.Format("mod-vco{0}", vco),
                    Name = string.Format("VCO {0}", vco),
                    Type = "vco",
                    Index = vco
                };

                module.Parameters.Add(MakeParam(controller, prefix + "_FREQ", "FREQ", IsModulated(prefix + "_FRQ", modulatedDests)));
                module.Parameters.Add(MakeParam(controller, prefix + "_DETUNE", "DETUNE", false));
                module.Parameters.Add(MakeParam(controller, prefix + "_PW", "PW", IsModulated(prefix + "_PW", modulatedDests)));
                module.Parameters.Add(MakeParam(controller, prefix + "_VOLUME", "VOL", IsModulated(prefix + "_VOL", modulatedDests)));

                // Waveshapes
                AddBoolParam(module, controller, prefix + "_WAVESHAPE_TRI", "TRI");
                AddBoolParam(module, controller, prefix + "_WAVESHAPE_SAW", "SAW");
                AddBoolParam(module, controller, prefix + "_WAVESHAPE_PULSE", "PULSE");

                // Mods
                AddBoolParam(module, controller, prefix + "_MOD_KEYB", "KEYB");
                AddBoolParam(module, controller, prefix + "_MOD_LAG", "LAG");
                AddBoolParam(module, controller, prefix + "_MOD_LEV1", "LEV1");
                AddBoolParam(module, controller, prefix + "_MOD_VIB", "VIB");

                model.Modules.Add(module);
            }
        }

        private static void ExtractVcfVcaModule(XpanderController controller, PatchVisualizationModel model,
            Dictionary<string, string> tagToModDest, HashSet<string> modulatedDests)
        {
            var module = new ModuleModel { Id = "mod-vcf", Name = "VCF", Type = "vcf", Index = 1 };
            module.Parameters.Add(MakeParam(controller, "VCF_FREQ", "FREQ", IsModulated("VCF_FRQ", modulatedDests)));
            module.Parameters.Add(MakeParam(controller, "VCF_RES", "RES", IsModulated("VCF_RES", modulatedDests)));
            module.Parameters.Add(MakeParam(controller, "VCF_MODE", "MODE", false));
            module.Parameters.Add(AddBoolParam(module, controller, "VCF_MOD_KEYB", "KEYB"));
            module.Parameters.Add(AddBoolParam(module, controller, "VCF_MOD_LAG", "LAG"));
            module.Parameters.Add(AddBoolParam(module, controller, "VCF_MOD_LEV1", "LEV1"));
            module.Parameters.Add(AddBoolParam(module, controller, "VCF_MOD_VIB", "VIB"));
            model.Modules.Add(module);

            var vca1 = new ModuleModel { Id = "mod-vca1", Name = "VCA 1", Type = "vca", Index = 1 };
            vca1.Parameters.Add(MakeParam(controller, "VCF_VCA1_VOLUME", "VOL", IsModulated("VCA1_VOL", modulatedDests)));
            model.Modules.Add(vca1);

            var vca2 = new ModuleModel { Id = "mod-vca2", Name = "VCA 2", Type = "vca", Index = 2 };
            vca2.Parameters.Add(MakeParam(controller, "VCF_VCA2_VOLUME", "VOL", IsModulated("VCA2_VOL", modulatedDests)));
            model.Modules.Add(vca2);
        }

        private static void ExtractFmLagModule(XpanderController controller, PatchVisualizationModel model,
            Dictionary<string, string> tagToModDest, HashSet<string> modulatedDests)
        {
            var module = new ModuleModel { Id = "mod-fmlag", Name = "FM / LAG", Type = "misc", Index = 1 };
            module.Parameters.Add(MakeParam(controller, "FM_AMP", "FM AMP", IsModulated("FM_AMP", modulatedDests)));
            module.Parameters.Add(MakeParam(controller, "FM_DESTINATION", "FM DEST", false));
            module.Parameters.Add(MakeParam(controller, "LAG_IN", "LAG IN", false));
            module.Parameters.Add(MakeParam(controller, "FMLAG_RATE", "LAG RATE", IsModulated("LAG_RATE", modulatedDests)));
            AddBoolParam(module, controller, "LAG_MODE_LEGATO", "LEGATO");
            AddBoolParam(module, controller, "LAG_TIMING_LINEAR_EXPO", "LIN/EXP");
            AddBoolParam(module, controller, "LAG_LINEAR_EQUAL_TIME", "EQ TIME");
            model.Modules.Add(module);
        }

        private static void ExtractLfoModule(XpanderController controller, PatchVisualizationModel model,
            int lfoNumber, HashSet<string> modulatedDests)
        {
            string prefix = string.Format("LFO_{0}", lfoNumber);
            string destPrefix = string.Format("LFO{0}", lfoNumber);
            var module = new ModuleModel
            {
                Id = string.Format("mod-lfo{0}", lfoNumber),
                Name = string.Format("LFO {0}", lfoNumber),
                Type = "lfo",
                Index = lfoNumber
            };

            module.Parameters.Add(MakeParam(controller, prefix + "_SPEED", "SPD", IsModulated(destPrefix + "_SPD", modulatedDests)));
            module.Parameters.Add(MakeParam(controller, prefix + "_AMP", "AMP", IsModulated(destPrefix + "_AMP", modulatedDests)));
            module.Parameters.Add(MakeParam(controller, prefix + "_WAVESHAPE", "WAVE", false));
            module.Parameters.Add(MakeParam(controller, prefix + "_RETRIG", "RETRIG", false));
            module.Parameters.Add(MakeParam(controller, prefix + "_SAMPLE_INPUT", "SAMPLE", false));
            AddBoolParam(module, controller, prefix + "_LAG", "LAG");
            module.Parameters.Add(MakeParam(controller, prefix + "_RETRIG_MODE", "MODE", false));

            model.Modules.Add(module);
        }

        private static void ExtractEnvModule(XpanderController controller, PatchVisualizationModel model,
            int envNumber, HashSet<string> modulatedDests)
        {
            string prefix = string.Format("ENV_{0}", envNumber);
            string destPrefix = string.Format("ENV{0}", envNumber);
            var module = new ModuleModel
            {
                Id = string.Format("mod-env{0}", envNumber),
                Name = string.Format("ENV {0}", envNumber),
                Type = "env",
                Index = envNumber
            };

            module.Parameters.Add(MakeParam(controller, prefix + "_DELAY", "DLY", IsModulated(destPrefix + "_DLY", modulatedDests)));
            module.Parameters.Add(MakeParam(controller, prefix + "_ATTACK", "ATK", IsModulated(destPrefix + "_ATK", modulatedDests)));
            module.Parameters.Add(MakeParam(controller, prefix + "_DECAY", "DCY", IsModulated(destPrefix + "_DCY", modulatedDests)));
            module.Parameters.Add(MakeParam(controller, prefix + "_SUSTAIN", "SUS", false));
            module.Parameters.Add(MakeParam(controller, prefix + "_RELEASE", "REL", IsModulated(destPrefix + "_REL", modulatedDests)));
            module.Parameters.Add(MakeParam(controller, prefix + "_VOLUME", "AMP", IsModulated(destPrefix + "_AMP", modulatedDests)));
            AddBoolParam(module, controller, prefix + "_MODE_RESET", "RESET");
            AddBoolParam(module, controller, prefix + "_MODE_FREERUN", "FREERUN");
            AddBoolParam(module, controller, prefix + "_MODE_DADR", "DADR");
            AddBoolParam(module, controller, prefix + "_TRIG_SINGLE_MULTI", "S/M");
            AddBoolParam(module, controller, prefix + "_TRIG_EXTRIG", "EXTRIG");
            AddBoolParam(module, controller, prefix + "_TRIG_LFOTRIG", "LFOTRIG");
            AddBoolParam(module, controller, prefix + "_TRIG_GATED", "GATED");

            model.Modules.Add(module);
        }

        private static void ExtractTrackModule(XpanderController controller, PatchVisualizationModel model, int trackNumber)
        {
            string prefix = string.Format("TRACK_{0}", trackNumber);
            var module = new ModuleModel
            {
                Id = string.Format("mod-trk{0}", trackNumber),
                Name = string.Format("TRACK {0}", trackNumber),
                Type = "misc",
                Index = trackNumber
            };

            module.Parameters.Add(MakeParam(controller, prefix + "_IN", "IN", false));
            for (int pt = 1; pt <= XpanderConstants.TRACK_POINTS_COUNTS; pt++)
            {
                module.Parameters.Add(MakeParam(controller, string.Format("{0}_POINT_{1}", prefix, pt),
                    string.Format("PT{0}", pt), false));
            }

            model.Modules.Add(module);
        }

        private static void ExtractRampModule(XpanderController controller, PatchVisualizationModel model, int rampNumber)
        {
            string prefix = string.Format("RAMP_{0}", rampNumber);
            var module = new ModuleModel
            {
                Id = string.Format("mod-rmp{0}", rampNumber),
                Name = string.Format("RAMP {0}", rampNumber),
                Type = "misc",
                Index = rampNumber
            };

            module.Parameters.Add(MakeParam(controller, prefix + "_RATE", "RATE", false));
            AddBoolParam(module, controller, prefix + "_TRIG_SINGLE_MULTI", "S/M");
            AddBoolParam(module, controller, prefix + "_TRIG_EXTRIG", "EXTRIG");
            AddBoolParam(module, controller, prefix + "_TRIG_LFOTRIG", "LFOTRIG");
            AddBoolParam(module, controller, prefix + "_TRIG_GATED", "GATED");

            model.Modules.Add(module);
        }

        private static ModuleParameter AddBoolParam(ModuleModel module, XpanderController controller, string paramKey, string displayName)
        {
            int value = GetParamValue(controller, paramKey);
            var param = new ModuleParameter
            {
                Name = displayName,
                DisplayValue = value == 1 ? "Y" : "N",
                RawValue = value,
                IsModulated = false
            };
            module.Parameters.Add(param);
            return param;
        }

        #endregion

        #region JSON serialization (manual, no dependency)

        private static string SerializeToJson(PatchVisualizationModel model)
        {
            var sb = new StringBuilder();
            sb.Append('{');
            sb.AppendFormat("\"patchName\":{0},", JsonString(model.PatchName));
            sb.AppendFormat("\"programNumber\":{0},", model.ProgramNumber);

            // modules
            sb.Append("\"modules\":[");
            for (int m = 0; m < model.Modules.Count; m++)
            {
                if (m > 0) sb.Append(',');
                var mod = model.Modules[m];
                sb.Append('{');
                sb.AppendFormat("\"id\":{0},", JsonString(mod.Id));
                sb.AppendFormat("\"name\":{0},", JsonString(mod.Name));
                sb.AppendFormat("\"type\":{0},", JsonString(mod.Type));
                sb.AppendFormat("\"index\":{0},", mod.Index);
                sb.Append("\"params\":[");
                for (int p = 0; p < mod.Parameters.Count; p++)
                {
                    if (p > 0) sb.Append(',');
                    var par = mod.Parameters[p];
                    sb.Append('{');
                    sb.AppendFormat("\"name\":{0},", JsonString(par.Name));
                    sb.AppendFormat("\"display\":{0},", JsonString(par.DisplayValue));
                    sb.AppendFormat("\"raw\":{0},", par.RawValue);
                    sb.AppendFormat("\"mod\":{0}", par.IsModulated ? "true" : "false");
                    sb.Append('}');
                }
                sb.Append("]}");
            }
            sb.Append("],");

            // modulations
            sb.Append("\"modulations\":[");
            for (int i = 0; i < model.Modulations.Count; i++)
            {
                if (i > 0) sb.Append(',');
                var r = model.Modulations[i];
                sb.Append('{');
                sb.AppendFormat("\"src\":{0},", JsonString(r.Source));
                sb.AppendFormat("\"dest\":{0},", JsonString(r.Destination));
                sb.AppendFormat("\"srcId\":{0},", JsonString(r.SourceModuleId));
                sb.AppendFormat("\"destId\":{0},", JsonString(r.DestModuleId));
                sb.AppendFormat("\"amount\":{0},", r.Amount);
                sb.AppendFormat("\"qtz\":{0}", r.Quantize);
                sb.Append('}');
            }
            sb.Append("]}");

            return sb.ToString();
        }

        private static string JsonString(string value)
        {
            if (value == null) return "null";
            return "\"" + value.Replace("\\", "\\\\").Replace("\"", "\\\"").Replace("\n", "\\n").Replace("\r", "\\r") + "\"";
        }

        #endregion

        #region HTML generation

        /// <summary>
        /// Builds the final HTML by injecting the Dagre library and patch JSON data into the template.
        /// </summary>
        private static string BuildHtml(string patchJson)
        {
            string templatePath = GetTemplatePath();
            string template = File.ReadAllText(templatePath, Encoding.UTF8);

            // Inject dagre.min.js library inline
            string dagrePath = Path.Combine(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), DAGRE_FILENAME);
            string dagreJs = File.ReadAllText(dagrePath, Encoding.UTF8);
            template = template.Replace(DAGRE_PLACEHOLDER, dagreJs);

            // Replace everything between BEGIN and END markers (inclusive) with the JSON data
            int beginIndex = template.IndexOf(JSON_PLACEHOLDER_BEGIN, StringComparison.Ordinal);
            int endIndex = template.IndexOf(JSON_PLACEHOLDER_END, StringComparison.Ordinal);
            if (beginIndex < 0 || endIndex < 0)
                throw new InvalidOperationException("Patch visualizer template is missing data placeholders.");

            int endOfMarker = endIndex + JSON_PLACEHOLDER_END.Length;
            return string.Concat(template.AsSpan(0, beginIndex), patchJson, template.AsSpan(endOfMarker));
        }

        private static string GetTemplatePath()
        {
            string appDir = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
            return Path.Combine(appDir, TEMPLATE_FILENAME);
        }

        private static string GetOutputPath(string patchName)
        {
            string safeName = patchName?.Trim();
            if (string.IsNullOrEmpty(safeName)) safeName = "patch";

            // sanitize filename
            foreach (char c in Path.GetInvalidFileNameChars())
            {
                safeName = safeName.Replace(c, '_');
            }

            return Path.Combine(Path.GetTempPath(), safeName + ".html");
        }

        #endregion
    }
}
