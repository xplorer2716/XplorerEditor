#include "PianoKeyboardLayoutQuery.hpp"

#include <X11/XKBlib.h>
#include <X11/Xlib.h>

#include <map>

// Linux/X11 implementation of the DEC-JUC-116 seam. [RQ-GUI-074]
//
// X11/XKB keycodes are positional under the near-universal "evdev" ruleset:
// keycode = the Linux kernel scancode (<linux/input-event-codes.h> KEY_*) + 8.
// That offset is fixed by the XKB "evdev" rules file, not by the active
// layout, so "the keycode for the key labelled A on a US keyboard" is a
// constant we can hard-code — exactly DEC-JUC-114's inversion. Verified
// against the standard evdev keycode table; NOT exercised against a real X
// server in this session (this container has no DISPLAY), so
// makeNativeKeyboardLayoutQuery() correctly reports "unavailable" here and
// this table is CI's first real compilation, not this session's.
//
// No Wayland path: without an X server (bare Wayland, no XWayland),
// XOpenDisplay fails and the factory returns nullptr, which
// buildPianoKeyMapping() already treats as "keep JUCE's default"
// (DEC-JUC-118) — the correct degradation, not a gap to fill here.

namespace xplorer::app
{
    namespace
    {
        const std::map<char, unsigned int>& referenceKeycodes()
        {
            // evdev keycode = KEY_<X11 letter> scancode + 8.
            static const std::map<char, unsigned int> table = {
                // Piano note positions [RQ-GUI-074]
                {'a', 38}, {'w', 25}, {'s', 39}, {'e', 26}, {'d', 40}, {'f', 41}, {'t', 28},
                {'g', 42}, {'y', 29}, {'h', 43}, {'u', 30}, {'j', 44}, {'k', 45}, {'o', 32},
                {'l', 46}, {'p', 33}, {';', 47},
                // Knob preset-value positions: KEY_1..KEY_0 are kernel
                // scancodes 2..11, so evdev keycodes 10..19; KEY_MINUS (12) is
                // 20 and KEY_LEFTBRACE (26) is 34 — the two candidates for the
                // eleventh slot. [RQ-GUI-080, ADR-JUC-037]
                {'1', 10}, {'2', 11}, {'3', 12}, {'4', 13}, {'5', 14}, {'6', 15},
                {'7', 16}, {'8', 17}, {'9', 18}, {'0', 19}, {'-', 20}, {'[', 34},
            };
            return table;
        }

        /// X11 keysyms 0x0020..0x00FF are, by the keysym encoding itself, the
        /// Latin-1/Unicode code point of the same value (X.org keysymdef.h).
        /// Every layout this feature targets (AZERTY, QWERTZ and the other
        /// Latin-alphabet European layouts) resolves the reference positions
        /// inside this range; anything outside it (or NoSymbol) is reported
        /// unresolvable rather than decoded further. [RQ-GUI-074]
        std::optional<char32_t> keysymToLatin1(KeySym keysym)
        {
            if (keysym >= 0x20 && keysym <= 0xff)
            {
                return static_cast<char32_t>(keysym);
            }
            return std::nullopt;
        }

        class X11KeyboardLayoutQuery final : public KeyboardLayoutQuery
        {
        public:
            explicit X11KeyboardLayoutQuery(Display* display) : _display(display) {}

            ~X11KeyboardLayoutQuery() override
            {
                XCloseDisplay(_display);
            }

            [[nodiscard]] std::optional<char32_t> characterForPosition(char referenceChar) const override
            {
                const auto& table = referenceKeycodes();
                const auto it = table.find(referenceChar);
                if (it == table.end())
                {
                    return std::nullopt;
                }

                XkbStateRec state;
                XkbGetState(_display, XkbUseCoreKbd, &state);

                const KeySym keysym =
                    XkbKeycodeToKeysym(_display, static_cast<KeyCode>(it->second), state.group, 0);
                if (keysym == NoSymbol)
                {
                    return std::nullopt;
                }
                return keysymToLatin1(keysym);
            }

        private:
            Display* _display;
        };
    }

    std::unique_ptr<KeyboardLayoutQuery> makeNativeKeyboardLayoutQuery()
    {
        Display* display = XOpenDisplay(nullptr);
        if (display == nullptr)
        {
            return nullptr; // no X server reachable — DEC-JUC-118
        }
        return std::make_unique<X11KeyboardLayoutQuery>(display);
    }
}
