#include "xplorer/app/SegmentFont.hpp"

#include <array>
#include <cstdint>

namespace xplorer::app
{
    namespace
    {
        // The generated table. Kept in this single translation unit so the
        // only way to reach a mask is segmentMaskFor(), which owns the
        // range check. [ADR-JUC-023 (DEC-JUC-051)]
#include "GeneratedSegmentFont.inc"
    }

    std::uint16_t segmentMaskFor(int codePoint) noexcept
    {
        if (codePoint < FIRST_GLYPH || codePoint > LAST_GLYPH)
        {
            return 0; // renders as a space, as the display always has
        }
        return SEGMENT_MASKS[static_cast<std::size_t>(codePoint - FIRST_GLYPH)];
    }
}
