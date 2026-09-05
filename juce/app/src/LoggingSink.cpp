#include "LoggingSink.hpp"

namespace xplorer::app
{
    JuceFileLoggerSink::JuceFileLoggerSink(const juce::File& file, const juce::String& welcomeMessage)
        : _fileLogger(file, welcomeMessage)
    {
    }

    void JuceFileLoggerSink::write(const std::string& line)
    {
        _fileLogger.logMessage(line);
    }
}
