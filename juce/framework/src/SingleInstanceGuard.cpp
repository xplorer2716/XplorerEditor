#include "midiapp/service/SingleInstanceGuard.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#endif

namespace midiapp::service
{
    SingleInstanceGuard::SingleInstanceGuard(const std::string& lockFilePath)
        : _path(lockFilePath)
    {
#ifdef _WIN32
        // CREATE_ALWAYS + no sharing: a second instance fails to open.
        _handle = CreateFileA(lockFilePath.c_str(), GENERIC_WRITE, 0, nullptr,
                              OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        _acquired = _handle != INVALID_HANDLE_VALUE;
#else
        _fd = ::open(lockFilePath.c_str(), O_CREAT | O_RDWR, 0644);
        if (_fd >= 0)
        {
            _acquired = ::flock(_fd, LOCK_EX | LOCK_NB) == 0;
            if (!_acquired)
            {
                ::close(_fd);
                _fd = -1;
            }
        }
#endif
    }

    SingleInstanceGuard::~SingleInstanceGuard()
    {
#ifdef _WIN32
        if (_acquired && _handle != nullptr)
        {
            CloseHandle(_handle);
        }
#else
        if (_fd >= 0)
        {
            ::flock(_fd, LOCK_UN);
            ::close(_fd);
        }
#endif
    }
}
