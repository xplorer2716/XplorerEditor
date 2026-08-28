#pragma once

#include <string>

#ifdef _WIN32
#include <windows.h> // HANDLE [RQ-QLT-013]
#endif

namespace midiapp::service
{
    /// Port of MidiApp FileMutex: ensures a single running instance per user
    /// session via an exclusively-locked lock file. [RQ-FMW-072]
    class SingleInstanceGuard
    {
    public:
        /// Tries to acquire the lock file; check acquired() afterwards.
        explicit SingleInstanceGuard(const std::string& lockFilePath);
        ~SingleInstanceGuard();

        SingleInstanceGuard(const SingleInstanceGuard&) = delete;
        SingleInstanceGuard& operator=(const SingleInstanceGuard&) = delete;

        [[nodiscard]] bool acquired() const { return _acquired; }

    private:
        bool _acquired = false;
#ifdef _WIN32
        HANDLE _handle = nullptr; // [RQ-QLT-013]
#else
        int _fd = -1;
#endif
        std::string _path;
    };
}
