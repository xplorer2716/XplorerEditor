#include "JuceEventDispatcher.hpp"

#include <juce_events/juce_events.h>

namespace xplorer::app
{
    void JuceEventDispatcher::post(std::function<void()> action)
    {
        juce::MessageManager::callAsync(std::move(action));
    }
}
