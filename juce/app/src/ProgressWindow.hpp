#pragma once

// Modeless progress window for the event-driven all-data-dump operations
// (backup, get-all-single-patches), where progression is reported by incoming
// MIDI on the message thread. Port of the reference ProgressForm singleton.
// The blocking restore loop uses juce::ThreadWithProgressWindow instead (see
// Dialogs::runRestoreAllDataWithProgress). [RQ-GUI-025, RQ-GUI-026]

#include <juce_gui_extra/juce_gui_extra.h>

namespace xplorer::app
{
    class ProgressWindow final : public juce::DocumentWindow
    {
    public:
        ProgressWindow();

        /// Configures the bar range and window title, then shows it centred.
        void begin(const juce::String& title, int maxValue);
        void setStatus(const juce::String& status, int value);
        [[nodiscard]] int maxValue() const { return _maxValue; }

        void closeButtonPressed() override {} // non-cancellable, like the reference

    private:
        class Content;
        Content* _content = nullptr;
        double _progress = 0.0;
        int _maxValue = 1;
    };
}
