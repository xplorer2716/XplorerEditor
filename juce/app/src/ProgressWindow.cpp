#include "ProgressWindow.hpp"

namespace xplorer::app
{
    class ProgressWindow::Content final : public juce::Component
    {
    public:
        explicit Content(double& progress) : _bar(progress)
        {
            _label.setJustificationType(juce::Justification::centredLeft);
            addAndMakeVisible(_label);
            addAndMakeVisible(_bar);
            setSize(360, 84);
        }

        void setStatus(const juce::String& status) { _label.setText(status, juce::dontSendNotification); }

        void resized() override
        {
            auto area = getLocalBounds().reduced(12);
            _label.setBounds(area.removeFromTop(28));
            area.removeFromTop(6);
            _bar.setBounds(area.removeFromTop(24));
        }

    private:
        juce::Label _label;
        juce::ProgressBar _bar;
    };

    ProgressWindow::ProgressWindow()
        : juce::DocumentWindow("", juce::Colour::fromRGB(24, 28, 34), 0)
    {
        auto content = std::make_unique<Content>(_progress);
        _content = content.get();
        setContentOwned(content.release(), true);
        setUsingNativeTitleBar(true);
        setAlwaysOnTop(true);
    }

    void ProgressWindow::begin(const juce::String& title, int maxValue)
    {
        setName(title);
        _maxValue = juce::jmax(1, maxValue);
        _progress = 0.0;
        centreWithSize(getWidth(), getHeight());
        setVisible(true);
        toFront(true);
    }

    void ProgressWindow::setStatus(const juce::String& status, int value)
    {
        _progress = juce::jlimit(0.0, 1.0, static_cast<double>(value) / static_cast<double>(_maxValue));
        _content->setStatus(status);
    }
}
