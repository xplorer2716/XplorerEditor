# Xplorer – Software Architecture Analysis

> **Author**: GitHub Copilot (Senior Software Architect review)  
> **Date**: 2026  
> **Target**: .NET 10 · Windows Forms · 3 Git repositories · 5 projects  
> **Purpose**: Oberheim Xpander / Matrix-12 real-time MIDI patch editor

---

## 1. Repository & Project Map

The solution spans **3 Git repositories** and **5 C# projects**:

```mermaid
graph TD
    subgraph repo-xplorer ["🗂 Repo: XplorerEditor"]
        Xplorer["📦 Xplorer\n(WinForms App, .NET 10)"]
    end

    subgraph repo-midiapp ["🗂 Repo: MidiApp"]
        MidiController["📦 MidiApp.MidiController\n(Abstract MIDI framework)"]
        UIControls["📦 MidiApp.UIControls\n(Custom WinForms controls)"]
    end

    subgraph repo-sanford ["🗂 Repo: Sanford.Multimedia.Midi"]
        SanfordCore["📦 Sanford.Multimedia.Midi.Core\n(Low-level Win32 MIDI I/O)"]
        SanfordUI["📦 Sanford.Multimedia.Midi.UI.Windows\n(PianoControl)"]
    end

    Xplorer --> MidiController
    Xplorer --> UIControls
    MidiController --> SanfordCore
    UIControls --> SanfordCore
    SanfordUI --> SanfordCore
```

---

## 2. Layered Architecture

The application follows a **3-layer MVC-inspired architecture** with clear separation between View, Controller, and Model.

```mermaid
flowchart TB
    subgraph VIEW ["🖥 View Layer  [Xplorer\\View]"]
        MainForm["MainForm\n(AbstractControllerMainForm)"]
        Helpers["Managers / Helpers\nFileOperationsManager\nSettingsManager\nModulationMatrixManager\nVfdDisplayHelper"]
        Dialogs["Dialogs\nToneMorphingForm\nSettingsForm\nExtractSingleToneForm\nRenamePatchForm\nStoreAndGotoPatchForm\nProgressForm\nAboutForm"]
    end

    subgraph CTRL ["⚙️ Controller Layer  [Xplorer\\Controller + MidiApp.MidiController]"]
        XpanderController["XpanderController (partial)\n─ .cs (core)\n─ .MIDIEvents.cs\n─ .WorkerThread.cs\n─ .ModulationMatrix.cs\n─ .Clipboard.cs"]
        AbstractController["AbstractController (abstract, partial)\n─ .cs (core)\n─ .MIDIDevices.cs\n─ .Events.cs\n─ .Rules.cs\n─ .WorkerThread.cs"]
        SettingsSvc["AllUsersSettingsService\n(static, XML serialization)"]
    end

    subgraph MODEL ["📐 Model Layer  [Xplorer\\Model + MidiApp.MidiController\\Model]"]
        XpanderTone["XpanderTone (partial)\n─ .cs\n─ .Properties.cs\n─ .ModulationMatrix.cs"]
        AbstractTone["AbstractTone (abstract)"]
        Params["Parameters\nXpanderParameter\nXpanderSignedParameter\nXpanderModMatrixParameter\nXpanderFullToneParameter\nAbstractParameter"]
        IO["Tone I/O\nIToneReader → XPanderToneReader\nIToneWriter → XPanderToneWriter\nPacketizedBinaryReader/Writer"]
    end

    subgraph MIDI ["🎹 MIDI Infrastructure  [Sanford.Multimedia.Midi]"]
        InputDevice["InputDevice\n(Win32 midiIn*)"]
        OutputDevice["OutputDevice\n(Win32 midiOut*)"]
        DelegateQueue["DelegateQueue\n(thread-safe async dispatch)"]
    end

    subgraph UICONTROLS ["🎛 UI Controls  [MidiApp.UIControls]"]
        KnobControl
        ComboBoxValuedControl
        CheckBoxValuedControl
        VacuumFluoDisplayControl
        LedPanelControl
        BlueTrackBar
        RadioButtonPanel
    end

    MainForm -->|uses| Helpers
    MainForm -->|opens| Dialogs
    MainForm -->|delegates to| XpanderController
    Helpers -->|calls| XpanderController
    XpanderController -->|inherits| AbstractController
    AbstractController -->|owns| XpanderTone
    XpanderTone -->|inherits| AbstractTone
    XpanderTone -->|contains| Params
    AbstractController -->|uses| IO
    AbstractController -->|reads/writes| SettingsSvc
    AbstractController -->|sends/receives| InputDevice
    AbstractController -->|sends/receives| OutputDevice
    InputDevice --> DelegateQueue
    MainForm -->|hosts| KnobControl
    MainForm -->|hosts| ComboBoxValuedControl
    MainForm -->|hosts| CheckBoxValuedControl
    MainForm -->|hosts| VacuumFluoDisplayControl
```

---

## 3. Key Subsystems

### 3.1 Parameter Queue & Worker Thread

The core real-time engine uses an **old-school polling worker thread** pattern:

```mermaid
sequenceDiagram
    participant UI as MainForm (UI Thread)
    participant Ctrl as XpanderController
    participant Queue as Parameter Queue
    participant WT as Worker Thread
    participant Synth as Oberheim Xpander (MIDI Out)

    UI->>Ctrl: SetParameter(name, value)
    Ctrl->>Ctrl: tone.ParameterMap[name].Value = value
    Ctrl->>Ctrl: parameter.Changed = true
    Note over WT: Thread.Sleep(TransmitDelay)
    WT->>Ctrl: Scan ParameterMap for Changed=true
    Ctrl->>Queue: EnQueueParameter(clone)
    WT->>Queue: DequeueParameter()
    WT->>Ctrl: PageSubPageHelper.GetPageSubPage()
    alt Page changed
        WT->>Synth: SendDataToSynthOutputDevice(PageSelectMessage)
    end
    WT->>Synth: SendDataToSynthOutputDevice(parameter.Message)
```

### 3.2 MIDI Event Flow (Bidirectional)

The application manages **3 MIDI devices** simultaneously:

```mermaid
flowchart LR
    AutoIn["🎹 Automation Input\n(DAW / MIDI Controller)"]
    SynthIn["🔌 Synth Input\n(Xpander → PC)"]
    SynthOut["🔌 Synth Output\n(PC → Xpander)"]

    AutoIn -->|ChannelMessage CC| AutoHandler["AutomationInputDeviceChannelMessageReceived\n→ map CC# → parameter name\n→ autoscale value\n→ update Tone"]
    AutoIn -->|SysEx| AutoSysEx["AutomationInputDeviceSysExMessageReceived\n→ notify UI only (no redirect)"]

    SynthIn -->|SinglePatchDump SysEx| ToneUpdate["FromByteArray()\n→ Stop worker\n→ Reload full tone\n→ NotifyFullToneChangeEvent\n→ Restart worker"]
    SynthIn -->|PanelButton SysEx| ParamUpdate["GetParameterForPageSubPageAndID\n→ Update parameter value\n→ NotifyAutomationParameterChangeEvent"]
    SynthIn -->|ModulationEdit SysEx| ModMatrix["HandleModulationEditFromSynth\n→ sync local mod matrix"]
    SynthIn -->|ProgramChange SysEx| ProgChange["Update CurrentProgramNumber"]

    AutoHandler --> SynthOut
    ToneUpdate -->|full resync| SynthOut
```

### 3.3 Settings Architecture

```mermaid
classDiagram
    class AllUsersSettingsService {
        +static AllUsersSettings AllUsersSettings
        +static SaveSettings()
        +static ResetSettings()
        -static LoadSettings() AllUsersSettings
    }
    class AllUsersSettings {
        +MidiConfiguration MidiConfig
        +UiConfiguration UiConfig
        +RandomizerConfiguration RandomizerConfig
    }
    class MidiConfiguration {
        +string SynthInputDeviceName
        +string SynthOutputDeviceName
        +int SysexTransmitDelay
        +int MidiChannel
        +int EditingProgramNumber
        +bool SynthTypeIsMatrix12
        +bool SmartAllNotesOff
        +List~string~ AutomationTable
    }
    class UiConfiguration {
        +int KnobLedBorderColor
        +bool KnobMovementIsLinear
        +bool KnobStyleIsStandard
    }
    class RandomizerConfiguration

    AllUsersSettingsService --> AllUsersSettings
    AllUsersSettings *-- MidiConfiguration
    AllUsersSettings *-- UiConfiguration
    AllUsersSettings *-- RandomizerConfiguration
```

### 3.4 Tone Model & I/O

```mermaid
classDiagram
    class AbstractTone {
        +string ToneName*
        +OrderedDictionary ParameterMap*
        +int MIDIChannel
        +FromByteArray()*
        +ToByteArray()*
    }
    class XpanderTone {
        +int CurrentProgramNumber
        +ReadOnlyCollection~ModulationMatrixEntry~ ModulationMatrix
        +ClearModulationMatrix()
        +ChangeModulationSource()
        +ChangeModulationDestination()
    }
    class AbstractParameter {
        +string Name
        +int Value
        +int MinValue / MaxValue
        +bool Changed
        +IMidiMessage Message*
        +Clone()*
    }
    class XpanderParameter {
        +EnumPages Page
        +int SubPage
        +IMidiMessage PageSelectMessage
    }
    class XpanderModMatrixParameter
    class XpanderSignedParameter
    class XpanderFullToneParameter

    class IToneReader {
        +ReadTone(filename, tone)
        +ReadTones(filename) ICollection
    }
    class IToneWriter {
        +WriteTone(filename, tone)
    }
    class XPanderToneReader
    class XPanderToneWriter

    AbstractTone <|-- XpanderTone
    XpanderTone *-- AbstractParameter
    AbstractParameter <|-- XpanderParameter
    XpanderParameter <|-- XpanderModMatrixParameter
    AbstractParameter <|-- XpanderSignedParameter
    AbstractParameter <|-- XpanderFullToneParameter
    IToneReader <|.. XPanderToneReader
    IToneWriter <|.. XPanderToneWriter
```

---

## 4. Class Inheritance Overview

```mermaid
classDiagram
    class Form
    class AbstractControllerMainForm {
        #virtual AbstractController Controller
        #Dictionary~string,Control~ RegisteredControlsMap
        +RecursivelyRegisterControls()
        +RegisterForControllerEvents()
        +OnAutomationParameterChange()
        +OnFullToneChange()
    }
    class MainForm {
        +XpanderController XController
        +VfdDisplayHelper VfdDisplayHelper
        -FileOperationsManager
        -SettingsManager
        -ModulationMatrixManager
    }

    class AbstractController {
        +AbstractTone Tone
        +bool IsRunning
        +Queue~AbstractParameter~ ParameterQueue
        +StringIntDualDictionary ControlChangeAutomationTable
        +Start() / Stop()
        #abstract WorkerThreadProc()
        #abstract CreateToneInstance()
    }
    class XpanderController {
        +LoadTone() / SaveTone()
        +RestoreAllDataDumpToSynth()
        +ChangeModulationSource()
        +Randomize()
        -PageSubPageHelper
        -AllDataDumpRequestState
    }

    Form <|-- AbstractControllerMainForm
    AbstractControllerMainForm <|-- MainForm
    AbstractController <|-- XpanderController
    MainForm --> XpanderController : uses (singleton)
```

---

## 5. SOLID Analysis

| Principle | Assessment | Detail |
|---|---|---|
| **S** – Single Responsibility | ✅ Mostly respected | `FileOperationsManager`, `SettingsManager`, `ModulationMatrixManager`, `VfdDisplayHelper` correctly offload MainForm responsibilities. Controller partial classes separate MIDI events, worker thread, mod matrix. |
| **O** – Open/Closed | ✅ Good | `AbstractController` / `AbstractTone` are designed for extension without modification. `IToneReader` / `IToneWriter` allow new synth formats. |
| **L** – Liskov Substitution | ✅ Respected | `XpanderController` fully extends `AbstractController`. `XpanderTone` fully extends `AbstractTone`. |
| **I** – Interface Segregation | ✅ Good | `IValuedControl`, `IToneReader`, `IToneWriter`, `ISettingsPage` are small, focused interfaces. |
| **D** – Dependency Inversion | ⚠️ Partial violations | `AbstractController` casts `AbstractTone` to `XpanderTone` in several places. `FileOperationsManager` holds a concrete `MainForm` reference. `AllUsersSettingsService` is a static class (untestable). |

---

## 6. Key Design Patterns Used

| Pattern | Where |
|---|---|
| **Template Method** | `AbstractController`, `AbstractTone` — abstract methods for tone creation, worker thread, sysex handling |
| **Observer / Events** | Controller fires `FullToneChangeEvent`, `AutomationParameterChangeEvent`, `MIDIDataSendReceiveEvent` → View updates |
| **Command Queue** | `Queue<AbstractParameter>` + worker thread — decouples UI from MIDI timing |
| **Strategy** | `IToneReader` / `IToneWriter` — swappable serialization |
| **Partial Classes** | `XpanderController`, `XpanderTone`, `MainForm` split by concern |
| **Manager / Helper** | `FileOperationsManager`, `SettingsManager`, `VfdDisplayHelper` decompose MainForm |
| **Clone (Prototype)** | `AbstractParameter.Clone()` used before enqueuing to avoid race conditions |

---

## 7. Threading Model

```mermaid
flowchart TB
    subgraph UIThread ["🧵 UI Thread (STA)"]
        MainForm
        Timer["WinForms Timer\n(UI refresh)"]
        DoEvents["Application.DoEvents()\n⚠️ used in ProgressForm"]
    end

    subgraph WorkerThread ["🧵 Worker Thread"]
        Loop["while(!StopRequested)\n  Thread.Sleep(TransmitDelay)\n  scan ParameterMap.Changed\n  EnQueue / DeQueue\n  Send to MIDI Out"]
    end

    subgraph MidiCallbackThread ["🧵 MIDI Driver Callback Thread"]
        DelegateQueue["DelegateQueue\n(Sanford async dispatch)"]
        SysExHandler["SysExMessageReceived\nChannelMessageReceived"]
    end

    MainForm -->|SetParameter| WorkerThread
    DelegateQueue -->|Post events| SysExHandler
    SysExHandler -->|Invoke| MainForm
    Timer -->|Tick| MainForm
```

> ⚠️ **Known issue**: `Application.DoEvents()` is used during all-data-dump restore and is even flagged with `#warning` in the source code. This is a UI freeze risk.

---

## 8. Improvement Proposals

### 8.1 🔴 High Priority

| # | Issue | Recommendation |
|---|---|---|
| 1 | `Application.DoEvents()` in `ProgressForm` + `#warning` in code | Replace with `async/await` + `IProgress<T>` pattern. Use `Task.Run()` for the dump operation. |
| 2 | Worker thread uses `Thread.Sleep` polling | Replace with `System.Threading.Channels.Channel<T>` (or `BlockingCollection<T>`) for a proper producer/consumer queue. Eliminates busy-sleep. |
| 3 | `AllUsersSettingsService` is a static class | Convert to an interface `ISettingsService` with a singleton implementation. Inject it into the controller. Enables unit testing. |

### 8.2 🟡 Medium Priority

| # | Issue | Recommendation |
|---|---|---|
| 4 | Concrete `XpanderTone` cast from `AbstractTone` in controller | Introduce a synth-specific interface (e.g. `IXpanderTone`) or use generics: `AbstractController<TTone>` where `TTone : AbstractTone`. |
| 5 | `FileOperationsManager` depends on concrete `MainForm` | Extract an interface `IMainFormFileOperations` and depend on it. |
| 6 | `OrderedDictionary` (non-generic) for `ParameterMap` | Migrate to `Dictionary<string, AbstractParameter>` + `List<string>` for order, or `OrderedDictionary<string, AbstractParameter>` (.NET 9+). |
| 7 | No DI container | Introduce `Microsoft.Extensions.DependencyInjection` for controller/service wiring. Would also solve the static settings service issue. |

### 8.3 🟢 Low Priority / Modernization

| # | Issue | Recommendation |
|---|---|---|
| 8 | No unit tests | Only a placeholder NUnit `Assert.Pass()` exists. Add tests for `XpanderTone` serialization, parameter bounds, modulation matrix logic. |
| 9 | `TODO: multi patch support` comment in MIDI event handler | Track this as a proper issue/backlog item. |
| 10 | XML settings serialization (`XmlSerializer`) | Consider migrating to `System.Text.Json` for consistency with .NET modern stack. |
| 11 | `SplashScreenForm` uses a raw `Thread` | Replace with `Task.Run()` + proper `CancellationToken`. |

---

## 9. Architecture Summary

```mermaid
C4Context
    title Xplorer – Context Diagram

    Person(user, "Musician", "Uses the editor to create and manage synth patches")
    System(xplorer, "Xplorer Editor", "Real-time patch editor for Oberheim Xpander / Matrix-12")
    System_Ext(synth, "Oberheim Xpander / Matrix-12", "Vintage polyphonic synthesizer")
    System_Ext(daw, "DAW / MIDI Controller", "Sends CC automation messages")
    System_Ext(fs, "File System", "Stores .syx patch files")

    Rel(user, xplorer, "Edits parameters, loads/saves patches")
    Rel(xplorer, synth, "Sends SysEx parameter changes, receives patch dumps", "MIDI OUT/IN")
    Rel(daw, xplorer, "Sends CC automation", "MIDI IN (Automation)")
    Rel(xplorer, fs, "Reads/writes .syx files")
```

### Strengths
- Clean separation between a **reusable MidiApp framework** (MidiApp.MidiController, MidiApp.UIControls) and the **Xpander-specific application** (Xplorer)
- Good use of abstract base classes enabling future support for other synths
- Thoughtful partial class decomposition of large classes
- Robust error reporting via `BugReportFactory` with MIDI-aware exception details
- Custom hardware-accurate UI controls (VFD display, knob, LED panel)

### Weaknesses
- **No async/await** — threading is entirely manual (Thread + Thread.Sleep)
- **No unit tests** for core business logic (tone serialization, modulation matrix, parameter bounds)
- **Static service** (`AllUsersSettingsService`) creates hidden global state
- **Concrete type casting** leaks abstraction at the controller/model boundary
- `Application.DoEvents()` is a known UI anti-pattern still present in the codebase
