// Xplorer JUCE application entry point. [RQ-GUI-005..007, RQ-NFR-008]
#include "JuceEventDispatcher.hpp"
#include "MainComponent.hpp"

#include "midiapp/service/Logger.hpp"

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
            // resizeToFitContent = false: the window states the size and the
            // content follows, rather than the content's constructor stating
            // it and the window reading it back. [DEC-JUC-064]
            setContentOwned(new ScaledCanvasComponent(), false);
            setResizable(true, true);        // [RQ-GUI-005]
            applyWindowScale(*this, 1.0F);   // opens at 1x [RQ-SCL-001]
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
        // Both read the build's own compile-time definitions, never a literal:
        // a literal cannot be kept in step with what was actually built, which
        // is how the About box came to advertise 0.1.0 while every published
        // artefact carried a different version. [RQ-BLD-015, RQ-BLD-016]
        const juce::String getApplicationName() override { return JUCE_APPLICATION_NAME_STRING; }
        const juce::String getApplicationVersion() override { return XPL_VERSION_FULL_STRING; }
        bool moreThanOneInstanceAllowed() override { return false; } // [RQ-FMW-072]

        void initialise(const juce::String&) override
        {
            // No splash: it mirrored the main panel (paintVectorBackground),
            // which is resizable (RQ-SCL-001/002) and user-themeable
            // (RQ-GUI-046, RQ-DSN-095) -- a copy of it cannot match the window
            // that follows at any size or delay. Removed rather than re-timed
            // or realigned. [RQ-GUI-055, ADR-JUC-030 (DEC-JUC-092)]
            _window = std::make_unique<MainWindow>();
        }

        // Top-level exception surface (reference TopLevelExceptionHandler). [RQ-GUI-035]
        void unhandledException(const std::exception* e, const juce::String& sourceFile,
                                int lineNumber) override
        {
            // [RQ-FMW-076, RQ-GUI-035, ADR-FMW-001 (DEC-FMW-002, DEC-FMW-004)]
            XPL_LOG(midiapp::service::LogDomain::UiEvents, midiapp::service::TraceLevel::Error,
                    "Unhandled exception: "
                        + std::string(e != nullptr ? e->what() : "unknown exception") + " at "
                        + sourceFile.toStdString() + ":" + std::to_string(lineNumber));
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
