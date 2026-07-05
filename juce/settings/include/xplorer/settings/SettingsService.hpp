#pragma once

#include "xplorer/settings/AllUsersSettings.hpp"

#include <memory>
#include <string>

namespace xplorer::settings
{
    /// Injectable settings access, replacing the reference's static
    /// AllUsersSettingsService (architecture-analysis §8.1 item 3). [RQ-SET-005]
    class ISettingsService
    {
    public:
        virtual ~ISettingsService() = default;

        /// Loads on first access; falls back to defaults (and persists them)
        /// when the file is missing, unreadable or partial. [RQ-SET-004]
        [[nodiscard]] virtual AllUsersSettings& allUsersSettings() = 0;

        virtual void saveSettings(const AllUsersSettings& settings) = 0;

        /// Persists the defaults and drops the cache. [RQ-SET-004]
        virtual void resetSettings() = 0;
    };

    /// XML file implementation, schema-compatible with the .NET
    /// XmlSerializer output of the reference, so an existing
    /// xplorer.users.config imports as-is. [RQ-SET-001, RQ-SET-006]
    class XmlSettingsService final : public ISettingsService
    {
    public:
        static constexpr auto SETTINGS_FILE_NAME = "xplorer.users.config";

        /// @param settingsDirectory per-machine app-data directory (the app
        /// passes the ProgramData-equivalent path; tests a temp dir).
        explicit XmlSettingsService(std::string settingsDirectory);
        ~XmlSettingsService() override;

        [[nodiscard]] AllUsersSettings& allUsersSettings() override;
        void saveSettings(const AllUsersSettings& settings) override;
        void resetSettings() override;

        [[nodiscard]] std::string settingsFilePath() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> _impl;
    };

    /// In-memory implementation for tests. [RQ-SET-005]
    class InMemorySettingsService final : public ISettingsService
    {
    public:
        InMemorySettingsService() : _settings(defaultAllUsersSettings()) {}

        [[nodiscard]] AllUsersSettings& allUsersSettings() override { return _settings; }
        void saveSettings(const AllUsersSettings& settings) override { _settings = settings; }
        void resetSettings() override { _settings = defaultAllUsersSettings(); }

    private:
        AllUsersSettings _settings;
    };
}
