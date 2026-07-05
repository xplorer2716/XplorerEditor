#pragma once

// Port of Xplorer.Controller.AllDataDumpRequestState. [RQ-CTL-004, RQ-CTL-005]

#include <cstdint>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

namespace xplorer::controller
{
    class AllDataDumpRequestState
    {
    public:
        enum class Mode
        {
            None,
            SinglePatch,
            All,
        };

        [[nodiscard]] bool isWaitingForAllDataDumpRequest() const
        {
            const std::lock_guard lock(_mutex);
            return _isWaiting;
        }

        void setWaitingForAllDataDumpRequest(bool waiting)
        {
            const std::lock_guard lock(_mutex);
            _isWaiting = waiting;
        }

        [[nodiscard]] const std::string& destination() const { return _destination; }
        [[nodiscard]] Mode receptionMode() const { return _mode; }

        [[nodiscard]] std::vector<std::pair<std::string, std::vector<std::uint8_t>>>& singlePatches()
        {
            return _singlePatches;
        }

        [[nodiscard]] std::vector<std::vector<std::uint8_t>>& multiPatches() { return _multiPatches; }

        void clear()
        {
            _singlePatches.clear();
            _multiPatches.clear();
        }

        void initialize(const std::string& destination, Mode receptionMode)
        {
            clear();
            setWaitingForAllDataDumpRequest(true);
            _destination = destination;
            _mode = receptionMode;
        }

    private:
        mutable std::mutex _mutex;
        bool _isWaiting = false;
        Mode _mode = Mode::None;
        std::string _destination;
        std::vector<std::pair<std::string, std::vector<std::uint8_t>>> _singlePatches;
        std::vector<std::vector<std::uint8_t>> _multiPatches;
    };
}
