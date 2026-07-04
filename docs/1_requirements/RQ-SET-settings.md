# RQ-SET — Settings Service

Scope: C++ replacement of `AllUsersSettingsService` / `AllUsersSettings`.

- **RQ-SET-001** — The application shall persist settings shared by all users of the machine in a per-machine application-data location (reference: `ProgramData\Xplorer\Xplorer`), surviving application restarts.
- **RQ-SET-002** — The settings shall include the MIDI configuration: synth input device name, synth output device name, automation input device name, SysEx transmit delay, MIDI channel, editing program number, synth type (Xpander/Matrix-12), smart-all-notes-off flag, and the CC automation table.
- **RQ-SET-003** — The settings shall include the UI configuration (knob LED/border color, knob movement linearity, knob style) and the randomizer configuration of the reference.
- **RQ-SET-004** — When the settings file is missing or unreadable, the service shall fall back to documented defaults and continue; a reset-to-defaults operation shall be provided.
- **RQ-SET-005** — The settings service shall be injectable behind an interface (no global static access from the controller), enabling unit tests with in-memory settings.
- **RQ-SET-006** — When a settings file produced by the reference .NET version is present, the service shall import its values (one-way migration) or, failing that, start with defaults without crashing.
