#include "PianoKeyboardLayoutQuery.hpp"

#include <Carbon/Carbon.h>

#include <map>

// macOS implementation of the DEC-JUC-116 seam. [RQ-GUI-074]
//
// Apple's kVK_ANSI_* virtual keycodes are positional by definition (Apple's
// own name for them: the ANSI physical layout), so the reference table below
// uses the SDK's own named constants rather than hand-typed numbers — the one
// part of this file least likely to be wrong from memory. UCKeyTranslate then
// resolves each keycode through the CURRENT keyboard input source. Plain C
// Carbon/HIToolbox API: no Objective-C needed. NOT compiled or run in this
// session (no macOS toolchain available here) — CI is this file's first real
// build, and only a Mac can confirm the resolved characters are correct.

namespace xplorer::app
{
    namespace
    {
        const std::map<char, CGKeyCode>& referenceKeyCodes()
        {
            static const std::map<char, CGKeyCode> table = {
                {'a', kVK_ANSI_A}, {'w', kVK_ANSI_W}, {'s', kVK_ANSI_S}, {'e', kVK_ANSI_E},
                {'d', kVK_ANSI_D}, {'f', kVK_ANSI_F}, {'t', kVK_ANSI_T}, {'g', kVK_ANSI_G},
                {'y', kVK_ANSI_Y}, {'h', kVK_ANSI_H}, {'u', kVK_ANSI_U}, {'j', kVK_ANSI_J},
                {'k', kVK_ANSI_K}, {'o', kVK_ANSI_O}, {'l', kVK_ANSI_L}, {'p', kVK_ANSI_P},
                {';', kVK_ANSI_Semicolon},
            };
            return table;
        }

        class MacKeyboardLayoutQuery final : public KeyboardLayoutQuery
        {
        public:
            explicit MacKeyboardLayoutQuery(const UCKeyboardLayout* layout, TISInputSourceRef source)
                : _layout(layout), _source(source)
            {
            }

            ~MacKeyboardLayoutQuery() override
            {
                CFRelease(_source);
            }

            [[nodiscard]] std::optional<char32_t> characterForPosition(char referenceChar) const override
            {
                const auto& table = referenceKeyCodes();
                const auto it = table.find(referenceChar);
                if (it == table.end())
                {
                    return std::nullopt;
                }

                UniCharCount actualLength = 0;
                UniChar unicodeString[4] = {};
                UInt32 deadKeyState = 0;

                // No modifiers: the unshifted base character, matching the
                // lowercase reference letters this table names positions
                // after. kUCKeyTranslateNoDeadKeysBit trades full dead-key
                // composition for a single call — acceptable here, since none
                // of these seventeen positions is a dead key on any layout
                // this feature targets.
                const OSStatus status = UCKeyTranslate(
                    _layout, it->second, kUCKeyActionDown, 0, LMGetKbdType(),
                    kUCKeyTranslateNoDeadKeysBit, &deadKeyState, 4, &actualLength, unicodeString);

                if (status != noErr || actualLength == 0)
                {
                    return std::nullopt;
                }
                return static_cast<char32_t>(unicodeString[0]);
            }

        private:
            const UCKeyboardLayout* _layout;
            TISInputSourceRef _source;
        };
    }

    std::unique_ptr<KeyboardLayoutQuery> makeNativeKeyboardLayoutQuery()
    {
        TISInputSourceRef source = TISCopyCurrentKeyboardInputSource();
        if (source == nullptr)
        {
            return nullptr;
        }

        CFDataRef layoutData =
            static_cast<CFDataRef>(TISGetInputSourceProperty(source, kTISPropertyUnicodeKeyLayoutData));
        if (layoutData == nullptr)
        {
            // Non-Unicode input source (e.g. some IMEs) — no layout table to
            // resolve against. DEC-JUC-118.
            CFRelease(source);
            return nullptr;
        }

        const auto* layout = reinterpret_cast<const UCKeyboardLayout*>(CFDataGetBytePtr(layoutData));
        return std::make_unique<MacKeyboardLayoutQuery>(layout, source);
    }
}
