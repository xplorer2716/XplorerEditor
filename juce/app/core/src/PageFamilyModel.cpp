#include "xplorer/app/PageFamilyModel.hpp"

namespace xplorer::app
{
    // Descriptors and control-tag lists ported verbatim from
    // XpanderConstants.PageFamilies and PageRefreshManager._pageTags.
    const std::vector<PageFamilyDescriptor>& pageFamilies()
    {
        static const std::vector<PageFamilyDescriptor> families = {
            {"ENV_X", "ENV_", 4, 5,
             {"ENV_X_ATTACK", "ENV_X_DECAY", "ENV_X_DELAY", "ENV_X_MODE_DADR", "ENV_X_MODE_FREERUN",
              "ENV_X_MODE_RESET", "ENV_X_RELEASE", "ENV_X_SUSTAIN", "ENV_X_TRIG_EXTRIG",
              "ENV_X_TRIG_GATED", "ENV_X_TRIG_LFOTRIG", "ENV_X_TRIG_LFO_SOURCE",
              "ENV_X_TRIG_SINGLE_MULTI", "ENV_X_VOLUME"}},
            {"LFO_X", "LFO_", 4, 5,
             {"LFO_X_AMP", "LFO_X_LAG", "LFO_X_RETRIG", "LFO_X_RETRIG_MODE", "LFO_X_SAMPLE_INPUT",
              "LFO_X_SPEED", "LFO_X_WAVESHAPE"}},
            {"RAMP_X", "RAMP_", 5, 4,
             {"RAMP_X_RATE", "RAMP_X_TRIG_EXTRIG", "RAMP_X_TRIG_GATED", "RAMP_X_TRIG_LFO_SOURCE",
              "RAMP_X_TRIG_LFOTRIG", "RAMP_X_TRIG_SINGLE_MULTI"}},
            // NOTE: the reference PageRefreshManager._pageTags lists the TRACK
            // point *control names* ("TRACK_X_PT1".."PT5") instead of their
            // parameter *tags* ("TRACK_X_POINT_1".."POINT_5"); the lookup is
            // keyed by tag, so in the reference the track points are never
            // refreshed on a TRACK instance switch (latent bug). This port
            // uses the real tags so they refresh correctly. [architecture-analysis_juce §11]
            {"TRACK_X", "TRACK_", 6, 3,
             {"TRACK_X_IN", "TRACK_X_POINT_1", "TRACK_X_POINT_2", "TRACK_X_POINT_3",
              "TRACK_X_POINT_4", "TRACK_X_POINT_5"}},
        };
        return families;
    }

    std::string resolveControlTag(const std::string& controlTag, int instance)
    {
        const auto position = controlTag.find("_X_");
        if (position == std::string::npos)
        {
            return controlTag;
        }
        std::string resolved = controlTag;
        resolved.replace(position, 3, "_" + std::to_string(instance) + "_");
        return resolved;
    }

    std::optional<FamilyParameter> familyParameterFor(const std::string& parameterName)
    {
        for (const auto& family : pageFamilies())
        {
            if (parameterName.rfind(family.parameterNamePrefix, 0) != 0)
            {
                continue;
            }
            if (static_cast<std::size_t>(family.digitIndex) >= parameterName.size())
            {
                break;
            }
            const char digit = parameterName[static_cast<std::size_t>(family.digitIndex)];
            if (digit < '1' || digit > '9')
            {
                break;
            }
            FamilyParameter result;
            result.controlTag = parameterName.substr(0, static_cast<std::size_t>(family.digitIndex))
                                + "X" + parameterName.substr(static_cast<std::size_t>(family.digitIndex) + 1);
            result.familyPrefix = family.controlTagPrefix;
            result.instance = digit - '0';
            return result;
        }
        return std::nullopt;
    }
}
