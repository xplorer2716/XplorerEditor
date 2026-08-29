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
        /// The live settings, for READING. Const by design: the returned
        /// reference points into the implementation's cache, so mutating it
        /// would change what every later reader sees while never reaching the
        /// disk -- a setting that works until the next restart. To change
        /// something, copy this, edit the copy, and pass it to saveSettings(),
        /// which persists and refreshes the cache in one step.
        /// [RQ-BUG-004, ADR-BUG-003 (DEC-BUG-010, DEC-BUG-011)]
        [[nodiscard]] virtual const AllUsersSettings& allUsersSettings() = 0;

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

        /// @param preferredDirectory per-machine app-data directory (the app
        /// passes the ProgramData-equivalent path; tests a temp dir).
        /// @param fallbackDirectory used instead when preferredDirectory
        /// cannot be created — e.g. Linux (/opt) and macOS (/Library) are
        /// root-owned and this project ships no installer to grant a
        /// standard user write access there. Empty (default) disables the
        /// fallback. [RQ-SET-001, ADR-SET-001 (DEC-SET-001)]
        explicit XmlSettingsService(std::string preferredDirectory, std::string fallbackDirectory = {});
        ~XmlSettingsService() override;

        [[nodiscard]] const AllUsersSettings& allUsersSettings() override;
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

        [[nodiscard]] const AllUsersSettings& allUsersSettings() override { return _settings; }
        void saveSettings(const AllUsersSettings& settings) override { _settings = settings; }
        void resetSettings() override { _settings = defaultAllUsersSettings(); }

    private:
        AllUsersSettings _settings;
    };
}
