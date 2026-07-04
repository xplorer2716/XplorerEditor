// AbstractController: paced transmit worker.
// Port of AbstractController.WorkerThread.cs semantics with modern
// primitives (interruptible wait, no busy-sleep). [RQ-FMW-040..042, ADR-005]
#include "midiapp/controller/AbstractController.hpp"

#include <chrono>

namespace midiapp::controller
{
    void AbstractController::startWorkerThread()
    {
        if (_workerThread.joinable())
        {
            stopWorkerThread();
        }
        {
            const std::lock_guard lock(_queueMutex);
            _parameterQueue.clear(); // start from an empty queue [RQ-FMW-041]
        }
        _workerThread = std::jthread([this](std::stop_token stopToken)
                                     { workerThreadProc(std::move(stopToken)); });
    }

    void AbstractController::stopWorkerThread()
    {
        if (_workerThread.joinable())
        {
            _workerThread.request_stop();
            _workerWake.notify_all();
            _workerThread.join(); // cooperative: wait is interruptible [RQ-FMW-041]
        }
        const std::lock_guard lock(_queueMutex);
        _parameterQueue.clear();
    }

    bool AbstractController::waitForTransmitDelay(std::stop_token stopToken)
    {
        std::unique_lock lock(_workerWakeMutex);
        _workerWake.wait_for(lock, stopToken,
                             std::chrono::milliseconds(_parameterTransmitDelay),
                             [&stopToken] { return stopToken.stop_requested(); });
        return !stopToken.stop_requested();
    }

    void AbstractController::scanChangedParametersIntoQueue()
    {
        // Reference scan: enqueue a clone of every changed parameter, so the
        // sent snapshot is immune to later value changes. [RQ-FMW-040]
        for (const auto& entry : _tone->parameterMap())
        {
            if (entry.parameter->changed())
            {
                entry.parameter->setChanged(false);
                enqueueParameter(entry.parameter->clone());
            }
        }
    }

    void AbstractController::workerThreadProc(std::stop_token stopToken)
    {
        while (waitForTransmitDelay(stopToken))
        {
            scanChangedParametersIntoQueue();

            // One transmission per tick: value-accurate, time-paced.
            std::unique_ptr<model::AbstractParameter> parameterToSend;
            if (dequeueParameter(parameterToSend))
            {
                sendToSynthOutput(parameterToSend->message());
            }
        }
    }

    void AbstractController::enqueueParameter(std::unique_ptr<model::AbstractParameter> parameter)
    {
        const std::lock_guard lock(_queueMutex);
        _parameterQueue.push_back(std::move(parameter));
    }

    bool AbstractController::dequeueParameter(std::unique_ptr<model::AbstractParameter>& parameter)
    {
        const std::lock_guard lock(_queueMutex);
        if (_parameterQueue.empty())
        {
            parameter = nullptr;
            return false;
        }
        parameter = std::move(_parameterQueue.front());
        _parameterQueue.pop_front();
        return true;
    }
}
