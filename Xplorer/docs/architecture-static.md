# Xplorer — Static Architecture

> This document covers only the Xplorer-specific classes.  
> For the base framework (abstract classes, service layer, base view), see [`MidiApp.MidiController` — Static Architecture](../../MidiApp/MidiApp.MidiController/docs/architecture-static.md).

## Overview

The **Xplorer** project is the concrete application built on top of `MidiApp.MidiController`. It implements a real-time patch editor for the **Oberheim Xpander** and **Matrix-12** analog synthesizers. It provides the concrete controller, tone model, parameter types, modulation matrix, and a full WinForms user interface.

---

## How Xplorer Extends MidiApp.MidiController

```mermaid
classDiagram
    direction TB

    class AbstractController {
        <<abstract – MidiApp>>
    }
    class AbstractTone {
        <<abstract – MidiApp>>
    }
    class AbstractParameter {
        <<abstract – MidiApp>>
    }
    class IToneReader {
        <<interface – MidiApp>>
    }
    class IToneWriter {
        <<interface – MidiApp>>
    }
    class AbstractControllerMainForm {
        <<abstract – MidiApp>>
    }

    class XpanderController {
        <<sealed, partial – Xplorer>>
    }
    class XpanderTone {
        <<sealed, partial – Xplorer>>
    }
    class XpanderParameter {
        <<Xplorer>>
    }
    class XPanderToneReader {
        <<Xplorer>>
    }
    class XPanderToneWriter {
        <<Xplorer>>
    }
    class MainForm {
        <<sealed, partial – Xplorer>>
    }

    AbstractController <|-- XpanderController
    AbstractTone <|-- XpanderTone
    AbstractParameter <|-- XpanderParameter
    IToneReader <|.. XPanderToneReader
    IToneWriter <|.. XPanderToneWriter
    AbstractControllerMainForm <|-- MainForm
```

---

## Controller Layer

### XpanderController (partial, 6 files)

```mermaid
classDiagram
    direction TB

    class XpanderController {
        <<sealed, partial>>
        -PageSubPageHelper _pageSubPageHelper
        -AllDataDumpRequestState _allDataDumpRequestState
        -bool _firstStart
        -string _clipboardSource
        +CurrentProgramNumber : int
        +ToneName : string
        +Start()
        +Stop()
        +LoadTone(filename)
        +LoadTone(filename, reader)
        +SaveTone(filename)
        +RandomizeTone(argument)
        +MorphTones(first, second, factor)
        +ExtractSinglePatchesFromAllDataDumpFileToDirectory()
        +DetermineSysexFileType(filename) : SysexFileType
        +GetProductNameAndVersionAsString() : string
        #CreateToneInstance() : XpanderTone
    }

    class XpanderController_MIDIEvents {
        <<partial>>
        #AutomationInputDeviceChannelMessageReceived()
        #AutomationInputDeviceSysExMessageReceived()
        #SynthInputDeviceSysExMessageReceived()
        #SynthInputDeviceChannelMessageReceived()
        -HandleAllDataDumpRequest(message, isSingle) : bool
        -HandleModulationEditFromSynth()
        -IsPageEditFollowsSysEx() : bool
        -IsModulationEditFollowsSysEx() : bool
        -IsSinglePatchProgramDumpSysex() : bool
    }

    class XpanderController_ModMatrix {
        <<partial>>
        +ChangeModulationSource()
        +ChangeModulationDestination()
        +ChangeModulationSourceAmount()
        +ChangeModulationSourceQuantize()
        +IsMaxSourceCountForDestinationReached() : bool
        +SourceAvailabilityForEntry() : bool
        +GetAvailableModulationDestinationsForEntry()
    }

    class XpanderController_Clipboard {
        <<partial>>
        -string _clipboardSource
        +ClipboardSource : string
        +CanClipboardPasteTo(dest) : bool
        +ClearClipboard()
        +IsClipboardEmpty() : bool
        +PasteClipboardTo(dest)
    }

    class XpanderController_Events {
        <<partial>>
        +FullToneChangeEvent : EventHandler~FullToneChangeEventArgs~
        +PageChangeEvent : EventHandler~PageChangeEventArgs~
        +ModulationEntryChangeEvent : EventHandler~ModulationEntryChangeEventArgs~
        +AllDataDumpRequestProgressionEvent : EventHandler~AllDataDumpRequestProgressionEventArgs~
        +MIDIDataSendReceiveEvent : EventHandler~MidiDataSendReceivedEventArgs~
        -NotifyFullToneChangeEvent()
        -NotifyPageChangeEvent()
        -NotifyModulationEntryChangeEvent()
        -NotifyAllDataDumpRequestProgressionEvent()
        -NotifyMIDIDataSendReceiveEvent()
    }

    class XpanderController_WorkerThread {
        <<partial>>
        #WorkerThreadProc()
    }

    XpanderController <|-- XpanderController_MIDIEvents : partial
    XpanderController <|-- XpanderController_ModMatrix : partial
    XpanderController <|-- XpanderController_Clipboard : partial
    XpanderController <|-- XpanderController_Events : partial
    XpanderController <|-- XpanderController_WorkerThread : partial
```

### Controller Support Classes

```mermaid
classDiagram
    direction TB

    class PageSubPageHelper {
        <<sealed>>
        -int _lastPageSelected
        -int _lastSubPageSelected
        -IDictionary _authorizedRotaryEvents
        +UpdatePageSubPage(page, subPage)
        +GetPageSubPage(out page, out subPage)
        +IsPageEnvLfoRampTrack() : bool
        +IsRotaryEventAuthorized() : bool
    }

    class AllDataDumpRequestState {
        <<sealed>>
        -bool _isWaitingForAllDataDumpRequest
        +IsWaitingForAllDataDumpRequest : bool
        +Destination : string
        +ReceptionMode : EnumAllDataDumpRequestMode
        +SinglePatches : IList
        +MultiPatches : IList
        +Initialize(dest, mode)
    }

    class SinglePatchIterator {
        +MoveNext() : bool
        +IsSinglePatch(data) : bool$
    }

    class SysexFileType {
        <<enum>>
        SingleTone
        AllDataDump
        Unknown
    }

    class AllUsersSettings {
        <<sealed>>
        +MidiConfig : MidiConfiguration
        +RandomizerConfig : RandomizerConfiguration
    }

    class AllUsersSettingsService {
        <<static>>
        +AllUsersSettings : AllUsersSettings$
        +Load()$
        +Save()$
    }

    SinglePatchIterator --|> SysexIterator : extends MidiApp
    AllUsersSettingsService --> AllUsersSettings : manages
```

### Xplorer-Specific Events

```mermaid
classDiagram
    direction TB

    class FullToneChangeEventArgs {
        +ParameterMap : Dictionary~string_int~
        +ModulationMatrix : ReadOnlyCollection~ModulationMatrixEntry~
    }

    class PageChangeEventArgs {
        +PageName : string
        +SubPage : int
    }

    class ModulationEntryChangeEventArgs {
        +Entry : ModulationMatrixEntry
        +EntryNumber : int
        +Parameter : EnumModulationParameter
    }

    class AllDataDumpRequestProgressionEventArgs {
        +AllDataDumpRequestState : AllDataDumpRequestState
    }

    class MidiDataSendReceivedEventArgs {
        +Device : EnumMIDIDevice
    }

    class EnumModulationParameter {
        <<enum>>
        MODULATIONSOURCE
        MODULATIONDESTINATION
        MODULATIONAMOUNT
        MODULATIONQUANTIZE
        ALL
        NONE
    }

    FullToneChangeEventArgs --|> EventArgs
    PageChangeEventArgs --|> EventArgs
    ModulationEntryChangeEventArgs --|> EventArgs
    AllDataDumpRequestProgressionEventArgs --|> EventArgs
    MidiDataSendReceivedEventArgs --|> EventArgs
```

---

## Model Layer

### XpanderTone (partial, 3 files)

```mermaid
classDiagram
    direction TB

    class XpanderTone {
        <<sealed, partial>>
        -OrderedDictionary _parameterMap
        -HashSet~string~ _parametersNamesForToneMorphing
        +ToneName : string
        +ParameterMap : OrderedDictionary
        +CurrentProgramNumber : int
        +EditingProgramNumber : int
        +ModulationMatrix : ReadOnlyCollection~ModulationMatrixEntry~
        +ToByteArray() : byte[]
        +FromByteArray(data)
        +FromStream(stream)
        #InitializeParameterMap()
        +GetEligibleParametersForToneMorhping() : HashSet~string~
    }

    class XpanderTone_Properties {
        <<partial>>
        +CurrentProgramNumber : int
        +EditingProgramNumber : int
        +Detune(detuneAnalog)
        +DefineVCOFrequenciesTuning(vcoFreq)
        +ForceEnv2ModVca2AfterRandomizeMatrix()
    }

    class XpanderTone_ModMatrix {
        <<partial>>
        -ModulationMatrixEntry[] _modulationMatrix
        +ModulationMatrix : ReadOnlyCollection~ModulationMatrixEntry~
        +ChangeModulationSource()
        +ChangeModulationDestination()
        +ChangeModulationSourceAmount()
        +ChangeModulationSourceQuantize()
        +RandomizeModulationMatrix()
        +IsMaxSourceCountForDestinationReached() : bool
        +ClearModulationMatrix()
    }

    XpanderTone <|-- XpanderTone_Properties : partial
    XpanderTone <|-- XpanderTone_ModMatrix : partial
    XpanderTone --> AbstractTone : extends MidiApp
```

### Parameter Hierarchy

```mermaid
classDiagram
    direction TB

    class AbstractParameter {
        <<abstract – MidiApp>>
        +Name, MinValue, MaxValue, Step, Value
        +Changed : bool
        +Message : SysExMessage
        +Clone()
    }

    class XpanderParameter {
        +Page : int
        +SubPage : int
        +PageSelectMessage : SysExMessage
        +SetValueUnchanged(value)
        #UpdateMessageFromValue()
        +Clone() : object
    }

    class XpanderSignedParameter {
        <<sealed>>
        #UpdateMessageFromValue()
    }

    class XpanderModMatrixParameter {
        <<sealed>>
        +SetSourceId(id)
        +SetModulationCommand(cmd)
        #UpdateMessageFromValue()
    }

    class XpanderFullToneParameter {
        <<sealed>>
        +Message : SysExMessage
        #UpdateMessageFromValue()
    }

    AbstractParameter <|-- XpanderParameter
    XpanderParameter <|-- XpanderSignedParameter
    XpanderParameter <|-- XpanderModMatrixParameter
    AbstractParameter <|-- XpanderFullToneParameter

    note for XpanderParameter "Adds Page/SubPage for\nXpander page-select SysEx"
    note for XpanderFullToneParameter "Full tone SysEx bypass\n(not page-based)"
```

### SysEx Data Model

```mermaid
classDiagram
    direction TB

    class XPanderSinglePatch {
        +PATCH_INTRO : byte[]$
        +ToByteArray(programNumber) : byte[]
        +FromByteArray(data) : XPanderSinglePatch
        +GetNameFromByteArray(data) : string$
    }

    class ModulationMatrixEntry {
        +MIN_AMOUNT = -63
        +MAX_AMOUNT = 63
        +Destination : EnumModulationDestinations
        +Source : EnumModulationSourcesModMatrix
        +IdSource : int
        +Amount : int
        +Quantize : int
    }

    class PacketizedBinaryReader {
        +ReadPacketizedByte() : byte
        +ReadPacketizedBytes(count) : byte[]
    }

    class PacketizedBinaryWriter {
        +WritePacketizedByte(value)
        +WritePacketizedBytes(data)
    }

    class XPanderToneReader {
        +ReadTone(filename, tone)
        +ReadTones(filename) : ICollection
    }

    class XPanderToneWriter {
        +WriteTone(filename, tone)
    }

    XPanderToneReader ..|> IToneReader
    XPanderToneWriter ..|> IToneWriter
    XPanderToneReader --> PacketizedBinaryReader : uses
    XPanderToneWriter --> PacketizedBinaryWriter : uses
    XPanderToneReader --> XPanderSinglePatch : reads
    XPanderToneWriter --> XPanderSinglePatch : writes
    XpanderTone o-- ModulationMatrixEntry : 20 entries
```

---

## View Layer

### MainForm and Managers

```mermaid
classDiagram
    direction TB

    class MainForm {
        <<sealed, partial>>
        -XpanderController _controller
        +XController : XpanderController
        +SplashScreen : SplashScreenForm
        +VfdDisplayHelper : VfdDisplayHelper
        -SettingsManager _settingsManager
        -FileOperationsManager _fileOperationsManager
        -ModulationMatrixManager _modulationMatrixManager
        -Dictionary _pagesRadioButtonsMap
        -Dictionary _knobControlsMap
        #Controller : AbstractController
    }

    class SettingsManager {
        <<sealed>>
        -MainForm _form
        +LoadSettings()
        +SaveSettings()
    }

    class FileOperationsManager {
        <<sealed>>
        -MainForm _form
        +LoadToneFromFile(fileName)
        +SaveToneToFile(fileName)
        +ExtractSingleTones()
        +BackupAllDataDump()
        +RestoreAllDataDump()
    }

    class ModulationMatrixManager {
        <<sealed>>
        -MainForm _form
        +HandleModSourceChanged()
        +HandleModDestChanged()
        +HandleModAmountChanged()
        +HandleModQuantizeChanged()
        +UpdateModulationMatrixUI()
    }

    class VfdDisplayHelper {
        <<sealed>>
        -AbstractController _controller
        -VacuumFluoDisplayControl _display
        +UpdateState(control)
        +UpdateWholeDisplay()
        +UpdateModulationEntry(entry)
    }

    MainForm --|> AbstractControllerMainForm : extends MidiApp
    MainForm *-- SettingsManager
    MainForm *-- FileOperationsManager
    MainForm *-- ModulationMatrixManager
    MainForm *-- VfdDisplayHelper
    MainForm --> XpanderController : uses
```

### Auxiliary Forms

```mermaid
classDiagram
    direction TB

    class SplashScreenForm {
        +VersionInformation : string
        +NextStep(message)
    }

    class AboutForm {
        About dialog
    }

    class ToneMorphingForm {
        +FirstToneFilename : string
        +SecondToneFilename : string
        +MorphingFactor : float
    }

    class ProgressForm {
        Progress indicator for<br/>long-running operations
    }

    class RenamePatchForm {
        +PatchName : string
    }

    class StoreAndGotoPatchForm {
        +ProgramNumber : int
    }

    class ExtractSingleToneForm {
        +DestinationFolder : string
    }

    class SettingsForm {
        +Pages : ISettingsPage[]
    }

    class ISettingsPage {
        <<interface>>
        +LoadSettings()
        +SaveSettings()
        +PageTitle : string
    }

    class MidiPage {
        MIDI device / channel config
    }

    class RandomizerPage {
        Randomizer options
    }

    class UserInterfacePage {
        UI preferences
    }

    SettingsForm o-- ISettingsPage
    ISettingsPage <|.. MidiPage
    ISettingsPage <|.. RandomizerPage
    ISettingsPage <|.. UserInterfacePage
```

---

## Common Layer

```mermaid
classDiagram
    direction TB

    class XpanderConstants {
        <<static>>
        +SINGLE_TONES_MAX_COUNT = 100$
        +MULTI_PATCHES_MAX_COUNT = 100$
        +TONE_NAME_LENGTH = 8$
        +MODENTRIES_COUNT = 20$
        +MAX_MODULATION_SOURCE = 6$
        +EnumPages
        +EnumModulationDestinations
        +EnumModulationSourcesModMatrix
        +EnumModulationEditCommands
        +EnumLFOWaveShapes
        +EnumRotaryEncoders
        +ModulationDestinationForPageSubPage()$
    }

    class XplorerConstants {
        +WEBSITE_URL$
        +RELEASES_URL$
        +USER_MANUAL_URL$
    }

    class SysexFileType {
        <<enum>>
        SingleTone
        AllDataDump
        Unknown
    }
```

---

## File Organization by Layer

```mermaid
graph LR
    subgraph Controller
        XC[XpanderController.cs]
        XC_ME[XpanderController.MIDIEvents.cs]
        XC_MM[XpanderController.ModulationMatrix.cs]
        XC_CB[XpanderController.Clipboard.cs]
        XC_EV[XpanderController.Events.cs]
        XC_WT[XpanderController.WorkerThread.cs]
        PSH[PageSubPageHelper.cs]
        ADDS[AllDataDumpRequestState.cs]
        SPI[SinglePatchIterator.cs]
        SFT2[SysexFileType.cs]
        subgraph Events
            FTCE[FullToneChangeEventsArgs.cs]
            PCE[PageChangeEventArgs.cs]
            MECE[ModulationEntryChangeEventArgs.cs]
            ADRPE[AllDataDumpRequestProgressionEventArgs.cs]
            MDSRE[MIDIDataSendReceivedEventArgs.cs]
        end
        subgraph Service/Settings
            AUS[AllUsersSettings.cs]
            AUSS[AllUsersSettingsService.cs]
        end
    end

    subgraph Model
        subgraph Tone
            XT[XpanderTone.cs]
            XT_P[XpanderTone.Properties.cs]
            XT_MM[XpanderTone.ModulationMatrix.cs]
            XSP[XPanderSinglePatch.cs]
            MME[ModulationMatrixEntry.cs]
            XTR[XPanderToneReader.cs]
            XTW[XPanderToneWriter.cs]
            PBR[PacketizedBinaryReader.cs]
            PBW[PacketizedBinaryWriter.cs]
        end
        subgraph Parameters
            XP[XpanderParameter.cs]
            XSPar[XpanderSignedParameter.cs]
            XMMP[XpanderModMatrixParameter.cs]
            XFTP[XpanderFullToneParameter.cs]
        end
    end

    subgraph View
        MF[MainForm.cs + .Overrides.cs + .Designer.cs]
        SM[SettingsManager.cs]
        FOM[FileOperationsManager.cs]
        MMM[ModulationMatrixManager.cs]
        VFD[VfdDisplayHelper.cs]
        SSF[SplashScreenForm.cs]
        AF[AboutForm.cs]
        TMF[ToneMorphingForm.cs]
        PF[ProgressForm.cs]
        RPF[RenamePatchForm.cs]
        SGPF[StoreAndGotoPatchForm.cs]
        ESTF[ExtractSingleToneForm.cs]
        subgraph Settings
            SF[SettingsForm.cs]
            MP[MidiPage.cs]
            RP[RandomizerPage.cs]
            UIP[UserInterfacePage.cs]
            ISP[ISettingsPage.cs]
        end
    end

    subgraph Common
        XConst[XpanderConstants.cs]
        XpConst[XplorerConstants.cs]
        SFType[SysexFileType.cs]
    end
```

---

## External Dependencies

```mermaid
graph TD
    Xplorer["Xplorer (WinExe)"]
    MidiController["MidiApp.MidiController"]
    UIControls["MidiApp.UIControls"]
    SanfordCore["Sanford.Multimedia.Midi.Core"]
    WinForms["System.Windows.Forms"]

    Xplorer --> MidiController
    Xplorer --> UIControls
    Xplorer --> SanfordCore
    Xplorer --> WinForms
    MidiController --> SanfordCore
    MidiController --> UIControls
```

---

## Key Differences from MidiApp.MidiController

| Concern | MidiApp.MidiController (base) | Xplorer (concrete) |
|---|---|---|
| **Controller** | `AbstractController` — generic MIDI lifecycle | `XpanderController` — Xpander/Matrix-12 SysEx protocol, page/subpage, display, clipboard, all-data-dump |
| **Tone** | `AbstractTone` — ordered parameter map | `XpanderTone` — 20-entry modulation matrix, VCO detune, `ToByteArray`/`FromByteArray` serialization |
| **Parameters** | `AbstractParameter` — generic min/max/step/value | `XpanderParameter` + `SignedParameter` + `ModMatrixParameter` + `FullToneParameter` — page-select SysEx, signed encoding |
| **Reader/Writer** | `IToneReader`/`IToneWriter` interfaces | `XPanderToneReader`/`XPanderToneWriter` — packetized binary format, `SinglePatchIterator` |
| **View** | `AbstractControllerMainForm` — generic registration | `MainForm` — VFD display, mod matrix UI, settings pages, 8+ auxiliary forms |
| **Events** | `AutomationParameterChangeEvent` only | 5 additional events: `FullToneChange`, `PageChange`, `ModulationEntryChange`, `AllDataDumpProgression`, `MIDIDataSendReceive` |
| **WorkerThread** | Generic scan → enqueue → dequeue → send | Override adds automatic page-select before each parameter send |
| **Settings** | None | `AllUsersSettings` / `AllUsersSettingsService` — MIDI config, randomizer config, UI prefs |

---

## Xplorer-Specific Design Patterns

| Pattern | Where | Description |
|---|---|---|
| **Singleton (lazy)** | `MainForm.Controller` property | Lazy-init of `XpanderController` |
| **State Machine** | `AllDataDumpRequestState` | Tracks multi-step all-data-dump reception |
| **Page Clipboard** | `XpanderController.Clipboard` | Copy/paste of parameter pages (ENV, LFO, RAMP, TRACK) by name prefix matching |
| **Manager Delegation** | `SettingsManager`, `FileOperationsManager`, `ModulationMatrixManager` | Main form delegates complex operations to focused manager classes |
| **Packetized I/O** | `PacketizedBinaryReader`/`Writer` | Oberheim's 7-bit SysEx encoding for binary data |
| **Filtered Iterator** | `SinglePatchIterator` | Extends `SysexIterator` to skip non-single-patch data |
