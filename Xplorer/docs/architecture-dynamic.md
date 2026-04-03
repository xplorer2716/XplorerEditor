# Xplorer — Dynamic Architecture

> This document covers only the Xplorer-specific dynamic flows.  
> For the base flows (generic startup, parameter change, CC# automation, worker thread, error handling), see [`MidiApp.MidiController` — Dynamic Architecture](../../MidiApp/MidiApp.MidiController/docs/architecture-dynamic.md).

## Overview

Xplorer extends the base `MidiApp.MidiController` flows with Xpander/Matrix-12-specific behavior: SysEx protocol handling (page-select, modulation edit, all-data-dump), modulation matrix management, page clipboard, tone morphing, and synth display control.

---

## 1. Application Startup (Xplorer-specific additions)

The base startup flow is described in the MidiApp.MidiController dynamic architecture. Xplorer adds:

```mermaid
sequenceDiagram
    participant Program as Program.Main()
    participant Splash as SplashScreenForm
    participant Settings as AllUsersSettingsService
    participant MainForm as MainForm
    participant XCtrl as XpanderController
    participant Synth as Synthesizer

    Program->>Program: FileMutex.Lock()
    Program->>Program: Register TopLevelExceptionHandler
    Program->>Settings: Load()
    Program->>Splash: new SplashScreenForm()
    Program->>Program: SplashThread (background)

    Program->>MainForm: new MainForm(splash)
    MainForm->>MainForm: InitializeComponent()

    MainForm->>MainForm: LoadSettings()
    Note right of MainForm: SettingsManager applies<br/>AllUsersSettings to controller

    MainForm->>MainForm: RegisterForControllerEvents()
    Note right of MainForm: Subscribes to 5 additional<br/>Xplorer events (see below)

    MainForm->>XCtrl: Start()
    Note right of XCtrl: First start: auto-sync

    XCtrl->>XCtrl: _firstStart = true
    XCtrl->>Synth: SendProgramChangeToSynthOutput()
    XCtrl->>Synth: SendProgramDumpRequestToSynth()
    Note right of XCtrl: Requests current patch<br/>from synth on first start

    XCtrl->>Synth: SendGreetingsToSynth()
    Note right of Synth: Xplorer version shown<br/>on synth display

    XCtrl->>XCtrl: base.Start()
    Note right of XCtrl: Starts input devices<br/>+ WorkerThread (see MidiApp docs)

    Splash->>Splash: Close()
```

### Xplorer Event Registration

```mermaid
graph LR
    MainForm -- subscribes --> AutomationParameterChangeEvent["AutomationParameterChangeEvent<br/>(inherited from MidiApp)"]
    MainForm -- subscribes --> FullToneChangeEvent
    MainForm -- subscribes --> PageChangeEvent
    MainForm -- subscribes --> ModulationEntryChangeEvent
    MainForm -- subscribes --> AllDataDumpRequestProgressionEvent
    MainForm -- subscribes --> MIDIDataSendReceiveEvent
```

---

## 2. WorkerThread Override — Page-Select Before Send

Xplorer overrides the base `WorkerThreadProc` to add automatic Xpander page-select SysEx before each parameter message.

```mermaid
sequenceDiagram
    participant Worker as WorkerThread
    participant PageHelper as PageSubPageHelper
    participant Output as OutputDevice
    participant Synth as Synthesizer

    Note over Worker: Loop while(!stopRequested)

    loop Main loop
        Worker->>Worker: Sleep(ParameterTransmitDelay)

        Worker->>Worker: Scan changed parameters
        Worker->>Worker: EnQueue changed clones

        Worker->>Worker: DequeueParameter()
        alt Parameter available
            Worker->>PageHelper: GetPageSubPage()
            alt Page changed
                Worker->>Output: Send(PageSelectMessage)
                Worker->>PageHelper: UpdatePageSubPage()
                Worker->>Worker: Sleep(ParameterTransmitDelay)
                Note right of Worker: Extra delay for<br/>page-select to settle
            end
            Worker->>Output: Send(param.Message)
            Output->>Synth: SysEx parameter message
        end
    end
```

---

## 3. SysEx Reception from Synth (Xpander protocol)

The base `SynthInputDeviceSysExMessageReceived` is overridden to handle Xpander-specific SysEx messages.

```mermaid
sequenceDiagram
    participant Synth as Synthesizer
    participant SynthInput as SynthInputDevice
    participant XCtrl as XpanderController
    participant Tone as XpanderTone
    participant Events as Controller Events
    participant MainForm as MainForm

    Synth->>SynthInput: SysEx message
    SynthInput->>XCtrl: SysExMessageReceived(e)

    alt Single Patch Program Dump
        Note right of XCtrl: IsSinglePatchProgramDumpSysex()
        alt Normal reception (not AllDataDump)
            XCtrl->>XCtrl: Stop()
            XCtrl->>Tone: FromByteArray(message)
            XCtrl->>Events: NotifyFullToneChangeEvent()
            Events->>MainForm: Update all controls + mod matrix
            XCtrl->>XCtrl: Start()
        else AllDataDump in progress
            XCtrl->>XCtrl: HandleAllDataDumpRequest()
        end

    else Multi Patch Program Dump
        Note right of XCtrl: IsMultiPatchProgramDumpSysex()
        XCtrl->>XCtrl: HandleAllDataDumpRequest()

    else Program Change UP/DOWN (panel dial)
        Note right of XCtrl: IsProgramChangeUP/DOWNSysEx()
        XCtrl->>Tone: CurrentProgramNumber ±1
        XCtrl->>Synth: SendProgramDumpRequestToSynth()

    else Page/SubPage Select
        Note right of XCtrl: IsPageSubPageSelectSysEx()
        XCtrl->>XCtrl: PageSubPageHelper.UpdatePageSubPage()
        alt Is ENV/LFO/RAMP/TRACK
            XCtrl->>Events: NotifyPageChangeEvent()
            Events->>MainForm: Switch displayed page
        end

    else Page Edit Follows (rotary/button)
        Note right of XCtrl: IsPageEditFollowsSysEx()
        XCtrl->>XCtrl: GetParameterForPageSubPageAndID()
        XCtrl->>XCtrl: SetValueUnchanged(value)
        XCtrl->>Events: NotifyAutomationParameterChangeEvent()
        Events->>MainForm: Update single control

    else Modulation Edit Follows
        Note right of XCtrl: IsModulationEditFollowsSysEx()
        XCtrl->>XCtrl: HandleModulationEditFromSynth()
        XCtrl->>Events: NotifyModulationEntryChangeEvent()
        Events->>MainForm: Update mod matrix UI
    end

    XCtrl->>Events: NotifyMIDIDataSendReceiveEvent()
    Events->>MainForm: Blink MIDI status LED
```

---

## 4. Page Refresh (Page Change Handler)

When the user clicks a page radio button or the controller receives a page-select message from the synth, `PageRefreshManager` coordinates the page switch and control refresh.

```mermaid
sequenceDiagram
    participant User as User / Synth
    participant XCtrl as XpanderController
    participant Events as Controller.PageChangeEvent
    participant MainForm as MainForm
    participant PageMgr as PageRefreshManager
    participant ControlsMap as RegisteredControlsMap

    User->>XCtrl: Send page-select SysEx
    alt User clicks radio button
        User->>MainForm: Click Radio_ENV_2
    else Synth sends page change
        XCtrl->>Events: NotifyPageChangeEvent("ENV_2")
    end

    Events->>MainForm: OnPageChange(sender, "ENV_2")
    MainForm->>MainForm: radio.Checked = true
    MainForm->>PageMgr: RefreshPage(radio, eventArgs)

    PageMgr->>PageMgr: GetFamilyPrefixForButton(radio) → "ENV_X"
    PageMgr->>PageMgr: Lookup page tags ["ENV_X_ATTACK", "ENV_X_DECAY", ...]

    loop For each control tag in page
        PageMgr->>MainForm: ResolveParameterNameForTag(tag)
        MainForm->>MainForm: Query PageFamilies for tag.StartsWith + digit
        MainForm-->>PageMgr: Resolved name "ENV_2_ATTACK"

        PageMgr->>ControlsMap: Get control for tag
        PageMgr->>PageMgr: vc.Value = controller.GetParameter(resolvedName).Value
    end

    PageMgr->>MainForm: VfdDisplayHelper.UpdateState(firstControl)

    Note right of PageMgr: All done in one central place<br/>replaces 4 × ~15-line handlers
```

---

## 5. Trigger Mutual-Exclusion Rules

When the user changes an ENV or RAMP trigger mode (external, LFO, gated), `TriggerRuleManager` enforces the synthesizer's mutual-exclusion rules.

```mermaid
sequenceDiagram
    participant User as User
    participant MainForm as MainForm
    participant TrigMgr as TriggerRuleManager
    participant Tone as XpanderTone
    participant Queue as Parameter Queue

    User->>MainForm: Click ENV_X_TRIG_EXTRIG checkbox
    MainForm->>MainForm: ENV_X_TRIG_EXTRIG_CheckedChanged()
    MainForm->>TrigMgr: ApplyExTrigRule(exTrig, lfoTrig, lfoSource, gated)

    alt exTrig checked
        TrigMgr->>MainForm: Uncheck lfoTrig
        TrigMgr->>MainForm: Disable lfoSource
        TrigMgr->>MainForm: Enable gated
    else exTrig unchecked
        TrigMgr->>MainForm: Disable lfoSource
        TrigMgr->>MainForm: Uncheck gated (if lfoTrig also unchecked)
    end

    MainForm->>MainForm: CheckBoxValuedControl_CheckedChanged()
    Note right of MainForm: Enqueue parameter change<br/>to synth

    MainForm->>Tone: SetParameterValue(exTrigParameterName, checked)
    Tone->>Queue: EnQueueParameter()

    Note right of Queue: WorkerThread will<br/>send to synth
```

---

## 6. All Data Dump (Backup / Restore)

### 6a. Backup from Synth

```mermaid
sequenceDiagram
    participant User as User
    participant MainForm as MainForm
    participant FileOps as FileOperationsManager
    participant XCtrl as XpanderController
    participant State as AllDataDumpRequestState
    participant Synth as Synthesizer
    participant Events as Controller Events

    User->>MainForm: Menu → Backup All Data Dump
    MainForm->>FileOps: BackupAllDataDump()
    FileOps->>XCtrl: BackupAllDataDumpToFile(fileName)

    XCtrl->>XCtrl: Stop() then Start()
    Note right of XCtrl: Reset message queue

    XCtrl->>State: Initialize(fileName, Mode.All)
    XCtrl->>Synth: SendAllDataDumpRequestToSynth()

    loop Synth sends 100 single patches + 100 multi patches
        Synth->>XCtrl: SysExMessageReceived()
        XCtrl->>State: Store patch data
        XCtrl->>Events: NotifyAllDataDumpRequestProgressionEvent()
        Events->>MainForm: Update progress bar
    end

    Note right of State: When all received:<br/>Write to file
    State->>State: BinaryWriter → file
```

### 6b. Restore to Synth

```mermaid
sequenceDiagram
    participant User as User
    participant MainForm as MainForm
    participant FileOps as FileOperationsManager
    participant XCtrl as XpanderController
    participant Iterator as SysexIterator
    participant Synth as Synthesizer

    User->>MainForm: Menu → Restore All Data Dump
    MainForm->>FileOps: RestoreAllDataDump()
    FileOps->>XCtrl: RestoreAllDataDumpToSynth(fileName, progressAction)

    XCtrl->>Iterator: new SysexIterator(stream)
    loop For each SysEx in file
        XCtrl->>Synth: SendDataToSynthOutputDevice(sysex)
        XCtrl->>MainForm: progressAction(i, count)
        XCtrl->>XCtrl: Sleep(150ms)
        Note right of XCtrl: Delay between each<br/>single patch send
    end

    XCtrl->>Synth: SendProgramChangeAndGetSinglePatchFromSynth()
    Note right of XCtrl: Resync current patch
```

---

## 7. Modulation Matrix Editing

```mermaid
sequenceDiagram
    participant User as User
    participant ModMgr as ModulationMatrixManager
    participant MainForm as MainForm
    participant XCtrl as XpanderController
    participant Tone as XpanderTone
    participant Queue as Parameter Queue
    participant Synth as Synthesizer

    User->>ModMgr: Change mod source combo
    ModMgr->>XCtrl: ChangeModulationSource(source, amount, quantize, dest, entry)
    XCtrl->>Tone: ChangeModulationSource(... , EnQueueParameter)
    Tone->>Tone: Update ModulationMatrix[entry]
    Tone->>Tone: Create XpanderModMatrixParameter
    Tone->>Queue: EnQueueParameter(modParam)
    Note right of Queue: Bypasses Changed flag,<br/>directly enqueued

    Note over Queue, Synth: WorkerThread picks up and sends

    User->>ModMgr: Change mod amount knob
    ModMgr->>XCtrl: ChangeModulationSourceAmount(source, amount, dest, entry)
    XCtrl->>Tone: ChangeModulationSourceAmount(... , EnQueueParameter)
    Tone->>Queue: EnQueueParameter(modParam)
```

---

## 8. Page Clipboard (Copy / Paste)

```mermaid
sequenceDiagram
    participant User as User
    participant MainForm as MainForm
    participant XCtrl as XpanderController
    participant Tone as XpanderTone
    participant Events as AutomationParameterChangeEvent

    User->>MainForm: Right-click → Copy "ENV_3"
    MainForm->>XCtrl: ClipboardSource = "ENV_3"

    User->>MainForm: Right-click → Paste to "ENV_5"
    MainForm->>XCtrl: CanClipboardPasteTo("ENV_5") → true
    MainForm->>XCtrl: PasteClipboardTo("ENV_5")

    loop For each parameter starting with "ENV_5"
        XCtrl->>Tone: Get source param "ENV_3_*"
        XCtrl->>Tone: Set dest param "ENV_5_*".Value = source.Value
        XCtrl->>Events: NotifyAutomationParameterChangeEvent()
        Events->>MainForm: Update UI control
    end

    Note right of MainForm: WorkerThread will<br/>send modified params
```

---

## 9. Tone Morphing

```mermaid
sequenceDiagram
    participant User as User
    participant ToneForm as ToneMorphingForm
    participant MainForm as MainForm
    participant XCtrl as XpanderController
    participant Reader as XPanderToneReader
    participant ToneA as XpanderTone A
    participant ToneB as XpanderTone B
    participant Result as XpanderTone Result
    participant Synth as Synthesizer

    User->>ToneForm: Select 2 files + morphing factor
    ToneForm->>MainForm: OK
    MainForm->>XCtrl: MorphTones(fileA, fileB, factor)

    XCtrl->>XCtrl: SendAllNotesOff()
    XCtrl->>XCtrl: Stop()

    XCtrl->>Reader: ReadTone(fileA, toneA)
    XCtrl->>Reader: ReadTone(fileB, toneB)
    XCtrl->>Reader: ReadTone(fileA, resultTone)
    Note right of XCtrl: Mod matrix comes<br/>from first tone

    XCtrl->>Result: MorphTones(toneA, toneB, result, factor)
    Note right of Result: value = (1-f)*A + f*B<br/>for each parameter

    XCtrl->>XCtrl: Tone = resultTone
    XCtrl->>XCtrl: UpdateUIAndSendFullToneToSynth()
    XCtrl->>Synth: SendFullToneToSynthIntoProgram()

    XCtrl->>XCtrl: Start()
```

---

## 10. Randomization (Xplorer-specific additions)

The base randomization is described in the MidiApp docs. Xplorer adds synthesizer-specific logic:

```mermaid
sequenceDiagram
    participant User as User
    participant MainForm as MainForm
    participant XCtrl as XpanderController
    participant Settings as RandomizerConfig
    participant Tone as XpanderTone
    participant Synth as Synthesizer

    User->>MainForm: Click "Random"
    MainForm->>XCtrl: RandomizeTone(argument)

    XCtrl->>XCtrl: SendAllNotesOff()
    XCtrl->>XCtrl: Stop()
    XCtrl->>XCtrl: ClearClipboard()

    rect rgb(240, 248, 255)
        Note over XCtrl, Settings: Apply Randomizer Settings
        XCtrl->>Settings: Read VCO2FmNoiseSync flags
        XCtrl->>XCtrl: Add FM/Noise/Sync to excludedParameters

        XCtrl->>Settings: Read VCODetune
        alt Analog or Digital detune
            XCtrl->>Tone: Detune(detuneAnalog)
            XCtrl->>XCtrl: Exclude VCO1/2_DETUNE
        end

        XCtrl->>Settings: Read VCOFreq
        alt Tuned frequencies
            XCtrl->>Tone: DefineVCOFrequenciesTuning()
            XCtrl->>XCtrl: Exclude VCO1/2_FREQ
        end
    end

    XCtrl->>Tone: RandomizeToneParameters(excluded, ratio)

    rect rgb(255, 248, 240)
        Note over XCtrl, Tone: Modulation Matrix Randomization
        XCtrl->>Settings: Read ModulationMatrix flags
        XCtrl->>Tone: RandomizeModulationMatrix(amount, quantize, sourceDest, ratio)
    end

    XCtrl->>Settings: Read VCA2Env
    alt Force ENV2 → VCA2
        XCtrl->>Tone: ForceEnv2ModVca2AfterRandomizeMatrix()
    end

    XCtrl->>Tone: ToneName = "RANDOM"
    XCtrl->>XCtrl: UpdateUIAndSendFullToneToSynth()
    XCtrl->>Synth: Full tone SysEx
    XCtrl->>XCtrl: Start()
```

---

## 9. Synth Display Control

```mermaid
sequenceDiagram
    participant XCtrl as XpanderController
    participant Output as OutputDevice
    participant Synth as Synthesizer Display

    rect rgb(230, 245, 255)
        Note over XCtrl, Synth: Greetings (startup)
        XCtrl->>Output: Send(displayOFFMessage)
        XCtrl->>Output: Send(displayONMessage)
        XCtrl->>XCtrl: Build "XPLORER VERSION X.X.X.X" + URL
        XCtrl->>Output: Send(displayMessage)
    end

    rect rgb(255, 245, 230)
        Note over XCtrl, Synth: Typewriter effect
        loop Character by character
            XCtrl->>Output: Send(partial message padded to 80 chars)
            XCtrl->>XCtrl: Sleep(50ms)
        end
    end
```

---

## 10. Automation Input Override (Program Change handling)

Xplorer extends the base CC# automation handler to also handle `ProgramChange` messages from the DAW.

```mermaid
sequenceDiagram
    participant DAW as DAW
    participant AutoInput as AutomationInputDevice
    participant XCtrl as XpanderController
    participant Tone as XpanderTone
    participant Synth as Synthesizer

    DAW->>AutoInput: ProgramChange message
    AutoInput->>XCtrl: ChannelMessageReceived(e)

    alt ProgramChange in valid range (0-99)
        XCtrl->>Tone: CurrentProgramNumber = e.Data1
        XCtrl->>Synth: SendProgramChangeAndGetSinglePatchFromSynth()
        Note right of XCtrl: Syncs editor with<br/>new program on synth
    else CC# message
        Note right of XCtrl: Handled by base class<br/>(see MidiApp docs)
    else Other message
        XCtrl->>Synth: Forward to synth
    end

    XCtrl->>XCtrl: NotifyMIDIDataSendReceiveEvent()
```

---

## Thread Summary (additions to MidiApp base)

| Thread | Additional Xplorer Responsibility |
|---|---|
| **UI Thread** | Manager classes (Settings, FileOps, ModMatrix), VFD display update, page radio buttons |
| **WorkerThread** | Page-select SysEx injection before each parameter send |
| **MIDI Input Callbacks** | Xpander SysEx protocol parsing (page edit, mod edit, program dump, all-data-dump state machine) |
| **SplashScreen Thread** | Dedicated thread for splash screen form during startup |
