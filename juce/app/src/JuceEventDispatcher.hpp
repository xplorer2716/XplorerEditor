#pragma once

// EventDispatcher implementation marshalling controller events to the JUCE
// message thread. [RQ-GUI-006, RQ-FMW-061, ADR-JUC-006]

#include "midiapp/controller/EventDispatcher.hpp"

namespace xplorer::app
{
    class JuceEventDispatcher final : public midiapp::controller::EventDispatcher
    {
    public:
        void post(std::function<void()> action) override;
    };
}
