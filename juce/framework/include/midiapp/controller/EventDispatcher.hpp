#pragma once

#include <functional>

namespace midiapp::controller
{
    /// Marshals controller events toward the UI thread. The application
    /// installs a JUCE message-thread implementation; tests use the
    /// synchronous one. [RQ-FMW-061, ADR-JUC-005]
    class EventDispatcher
    {
    public:
        virtual ~EventDispatcher() = default;
        virtual void post(std::function<void()> action) = 0;
    };

    class SynchronousEventDispatcher final : public EventDispatcher
    {
    public:
        void post(std::function<void()> action) override { action(); }
    };
}
