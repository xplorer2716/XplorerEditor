// Xplorer JUCE application entry point. [RQ-GUI-005..007, RQ-NFR-008]
#include "JuceEventDispatcher.hpp"
#include "MainComponent.hpp"
#include "BinaryData.h"

#include <juce_gui_extra/juce_gui_extra.h>

namespace xplorer::app
{
    class MainWindow final : public juce::DocumentWindow
    {
    public:
        MainWindow()
            : juce::DocumentWindow("Xplorer",
                                   juce::Colours::black,
                                   juce::DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new ScaledCanvasComponent(), true);
            setResizable(true, true); // [RQ-GUI-005]
            centreWithSize(getWidth(), getHeight());
            setVisible(true);
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
    };

    class XplorerApplication final : public juce::JUCEApplication
    {
    public:
        const juce::String getApplicationName() override { return "Xplorer"; }
        const juce::String getApplicationVersion() override { return "0.1.0"; }
        bool moreThanOneInstanceAllowed() override { return false; } // [RQ-FMW-072]

        void initialise(const juce::String&) override
        {
            // Reference splash behavior: shown while the app builds, auto-dismissed.
            (new juce::SplashScreen("Xplorer",
                                    juce::ImageCache::getFromMemory(BinaryData::mainbackground_jpg,
                                                                    BinaryData::mainbackground_jpgSize),
                                    true))
                ->deleteAfterDelay(juce::RelativeTime::seconds(2), false);
            _window = std::make_unique<MainWindow>();
        }

        // Top-level exception surface (reference TopLevelExceptionHandler). [RQ-GUI-035]
        void unhandledException(const std::exception* e, const juce::String& sourceFile,
                                int lineNumber) override
        {
            juce::AlertWindow::showMessageBoxAsync(
                juce::MessageBoxIconType::WarningIcon, "Xplorer - unexpected error",
                juce::String(e != nullptr ? e->what() : "unknown exception") + "\nat "
                    + sourceFile + ":" + juce::String(lineNumber)
                    + "\nPlease report this at github.com/xplorer2716/XplorerEditor.");
        }

        void shutdown() override
        {
            _window.reset();
        }

    private:
        std::unique_ptr<MainWindow> _window;
    };
}

START_JUCE_APPLICATION(xplorer::app::XplorerApplication)
