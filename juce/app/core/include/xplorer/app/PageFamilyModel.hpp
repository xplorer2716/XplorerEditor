#pragma once

// Page-family tag resolution (ENV/LFO/RAMP/TRACK). UI-framework-free port of
// Xplorer.View.PageRefreshManager + XpanderConstants.PageFamilies: maps the
// shared "_X_" control tags to concrete parameter names for the active
// instance, and back. Headless-testable. [RQ-GUI-010..012, ADR-006]

#include <optional>
#include <string>
#include <vector>

namespace xplorer::app
{
    struct PageFamilyDescriptor
    {
        std::string controlTagPrefix;   ///< e.g. "ENV_X"
        std::string parameterNamePrefix; ///< e.g. "ENV_"
        int digitIndex;                 ///< index of the instance digit in a concrete name
        int count;                      ///< number of instances (ENV=5 … TRACK=3)
        std::vector<std::string> controlTags; ///< the "_X_" tags to refresh, in reference order
    };

    /// The four page families in reference declaration order.
    [[nodiscard]] const std::vector<PageFamilyDescriptor>& pageFamilies();

    /// Resolves a "_X_" control tag to the concrete parameter name for the
    /// given 1-based instance (e.g. ("ENV_X_ATTACK", 3) → "ENV_3_ATTACK").
    /// Returns the tag unchanged when it is not a family tag.
    [[nodiscard]] std::string resolveControlTag(const std::string& controlTag, int instance);

    /// Reverse: for a concrete parameter name, yields the "_X_" control tag
    /// and the instance it belongs to; nullopt for non-family parameters.
    struct FamilyParameter
    {
        std::string controlTag; ///< the "_X_" tag
        std::string familyPrefix; ///< controlTagPrefix, e.g. "ENV_X"
        int instance;           ///< 1-based
    };
    [[nodiscard]] std::optional<FamilyParameter> familyParameterFor(const std::string& parameterName);
}
