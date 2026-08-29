#include "PianoKeyboardLayoutQuery.hpp"

#include <windows.h>

#include <map>

// Windows implementation of the DEC-JUC-116 seam, serving TWO features now:
// the piano window's note mapping (RQ-GUI-074) and the rotary knobs'
// preset-value keys (RQ-GUI-080). [RQ-GUI-074, RQ-GUI-080, ADR-JUC-037]
//
// Why the knob feature can MATCH on what this returns: JUCE's own
// juce_Windowing_windows.cpp doKeyChar() computes a KeyPress's keyCode with
// this exact pair — MapVirtualKey(scancode, 1) then MapVirtualKey(vk, 2),
// LOWORD'd — so a resolved character and the keyCode of a press at that
// position are the same computation on the same layout, not two things that
// happen to agree. [ADR-JUC-037 (DEC-JUC-126)]
//
// PC/AT Set 1 hardware scancodes are positional and unchanged since the
// original IBM PC XT — "the key labelled A on a US keyboard" is scancode
// 0x1E regardless of the active input locale, so it is hard-coded here. The
// resolution pair, MapVirtualKey(scancode, MAPVK_VSC_TO_VK) then
// MapVirtualKey(vk, MAPVK_VK_TO_CHAR), is the SAME pair JUCE's own
// KeyPress::isKeyCurrentlyDown already calls for the reverse direction
// (juce_Windowing_windows.cpp), reused here left-to-right instead of
// right-to-left. MapVirtualKey resolves against the calling thread's current
// input locale, which is what RQ-GUI-074 wants: "the layout active right
// now". NOT compiled or run in this session (no Windows toolchain available
// here) — CI is this file's first real build.

namespace xplorer::app
{
    namespace
    {
        const std::map<char, UINT>& referenceScanCodes()
        {
            static const std::map<char, UINT> table = {
                // Piano note positions [RQ-GUI-074]
                {'a', 0x1E}, {'w', 0x11}, {'s', 0x1F}, {'e', 0x12}, {'d', 0x20}, {'f', 0x21}, {'t', 0x14},
                {'g', 0x22}, {'y', 0x15}, {'h', 0x23}, {'u', 0x16}, {'j', 0x24}, {'k', 0x25}, {'o', 0x18},
                {'l', 0x26}, {'p', 0x19}, {';', 0x27},
                // Knob preset-value positions: the number row 0x02..0x0B, plus
                // the two candidates for the eleventh slot — 0x0C (right of
                // '0') and 0x1A (after 'P'). [RQ-GUI-080, ADR-JUC-037]
                {'1', 0x02}, {'2', 0x03}, {'3', 0x04}, {'4', 0x05}, {'5', 0x06}, {'6', 0x07},
                {'7', 0x08}, {'8', 0x09}, {'9', 0x0A}, {'0', 0x0B}, {'-', 0x0C}, {'[', 0x1A},
            };
            return table;
        }

        class WindowsKeyboardLayoutQuery final : public KeyboardLayoutQuery
        {
        public:
            [[nodiscard]] std::optional<char32_t> characterForPosition(char referenceChar) const override
            {
                const auto& table = referenceScanCodes();
                const auto it = table.find(referenceChar);
                if (it == table.end())
                {
                    return std::nullopt;
                }

                const UINT vk = MapVirtualKeyW(it->second, MAPVK_VSC_TO_VK);
                if (vk == 0)
                {
                    return std::nullopt;
                }

                const UINT translated = MapVirtualKeyW(vk, MAPVK_VK_TO_CHAR);
                // Bit 31 set: a dead key. Left unmapped rather than guessed,
                // per RQ-GUI-074's failure rule.
                if (translated == 0 || (translated & 0x80000000u) != 0)
                {
                    return std::nullopt;
                }
                // The low-order word carries the character (MSDN
                // MapVirtualKey, MAPVK_VK_TO_CHAR); a plain letter position
                // yields one UTF-16 code unit, safe to widen directly.
                return static_cast<char32_t>(translated & 0xFFFFu);
            }
        };
    }

    std::unique_ptr<KeyboardLayoutQuery> makeNativeKeyboardLayoutQuery()
    {
        // MapVirtualKey is always available on Windows — no failure mode to
        // report here, unlike the X11 "no display" case.
        return std::make_unique<WindowsKeyboardLayoutQuery>();
    }
}
