
# XPLORER - A real time editor for the Oberheim Xpander and Matrix-12 synthesizers

![Platform](https://img.shields.io/badge/platform-.NET-blue)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-green.svg)](https://www.gnu.org/licenses/gpl-3.0)

![Xplorer](https://github.com/xplorer2716/XplorerEditor/blob/main/README.XplorerFullScreen.jpg?raw=true)

Xplorer is a real-time and bi-directional software editor for the Oberheim Matrix-12 and Xpander synthesizers.

Xplorer is not just another generic synthesizer editor with ranges of slider controls that are all the same and don’t really tell you what you are doing.
Xplorer was developed by an Xpander owner for use by Xpander and Matrix-12 owners. With Xplorer you quickly get an overview of all parameters of a single patch, including an overview of the Modulation Matrix.

With Xplorer you can tweak the sound simultaneously using a computer mouse, a hardware MIDI controller, your DAW software, or directly on your Xpander or Matrix-12. Use the method you prefer to change the sound. Xplorer’s display shows you in real-time which parameter is currently being modified.

“Real-time and bi-directional” means that every change you make in Xplorer is reflected into the synthesizer memory with minimal latency, and vice-versa. Whether you change the sound parameters on the synthesizer, in Xplorer, or with a MIDI controller, the software and the synthesizer will simultaneously update the parameters of the sound, without needing to reload the patch into the synthesizer memory.

The main features of Xplorer are:

- Single patch tone file load/save on disk
- Extract single patches from an “all data data dump” SysEx file into a given folder
- Get all single patches from synthesizer into a given folder
- All data dump request backup and restore (save and restore synthesizer whole memory to/from a sysex file)
- Modification of the 226 parameters of a single patch
- Real-time MIDI automation for all parameters with freely assignable control changes, even for filter modes (except Modulation Matrix)
- Copy/paste page for TRACK, ENV, LFO, and RAMP pages (e.g., you can copy all ENV1 page parameters and paste them to ENV3)
- Rename patch, go to patch, store patch, save patch to disk
- Real-time update of all parameters when the user tweaks the synthesizer knobs (dual editing can be done without needing to reload the patch and resynchronize the synthesizer and the PC as most editors need to do)
- Patch randomizer (generates a new patch from random values)
- Auto-sized user interface depending on system’s default font size
- ...


# Requirements

- Windows 7,8,10,11 (x86/x64) [*]
- Display: 1280 x 900, 256 colors (Minimum); 1680 x 1050 high color, 32-bit (Recommended)
- a MIDI interface (recommended: a MIDI interface with one MIDI OUT and one MIDI IN, one Virtual MIDI cable driver - like LoopBe1: http://www.nerds.de/en/loopbe1.html)

[*] for MAC users:
- Mac/Intel users can use virtualization to run Xplorer. See this FAQ: http://xplorer.programmer.free.fr/bb/viewtopic.php?id=10
- Alternatively any old PC/cheap x64 NUC will run Xplorer perfectly.


# Installation

- Download the latest release: https://github.com/xplorer2716/XplorerEditor/releases (archive + manual)
- Unzip the archive
- Launch Xplorer.exe from the extracted archive
- **Read the manual** to configure Xplorer as required




# Contributing

I have no time to develop new features, still I maintain the app (bug fixing) for my own needs.
You are free to fork, contribute and submit PRs.

## IMPORTANT - Future of Xplorer

Xplorer was started about ten years ago as a personal project, and then commercialized for a few years.
It is today the only truly functional editor for the Oberheim Xpander and Matrix-12 synthesizers.

**In order to perpetuate the life of the application and these mythical synthesizers, it would be advisable to migrate the source code to a perennial framework such as JUCE - https://juce.com/.**

Overall, 3 main parts need to be migrated, with different levels of complexity:
1. Replacement of the MIDI implementation based on Leslie Sanford's C# MIDI toolkit by its C++ equivalent in JUCE. This could be strictly limited to the functions used in Xplorer (configuration of MIDI ports, sending and receiving MIDI messages). For someone who knows JUCE, this should be easy to do.
2. Convert the MidiApp framework implementation and its use in Xplorer (Model and Controller parts) to C++. This should be relatively straightforward, for a guy that is familiar with both platforms.
3. The big one, complete reimplementation of the UI in JUCE. The UI part of Xplorer is implemented in .NET Winforms, which has been pushed to the limit, notably for the management of all controls, transparency management and in-house binding with the controller part. I think this is the most important effort, and requires some knowledge of Winforms and UI with JUCE.


## Installation

### Prerequisites

- [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/)
- .NET Desktop Runtime (4.8+)

### Steps

1. Clone the repository

The project is made of 2 repos:
- the MidiApp repo, which is a framework to develop MIDI apps with .NET desktop.
- the XplorerEditor itself
- The MidiApp repo is used as a submodule into XplorerEditor. You have to fetch submodules to get everything:
    ```sh
    git clone --recurse-submodules https://github.com/xplorer2716/XplorerEditor.git
    ```

2. Open the Xplorer.sln solution in Visual Studio
- **Ensure to select x86 as target platform in the solution**, due to x86 platform invoke mapping in 3rd party library.

3. Build the application
- if Visual Studio does not find the Sanford.Multimedia DLL, reference it from MidiApp subfolder: `.\MidiApp\3rdParty\Sanford.Multimedia.Midi`
- DO NOT add an external reference to Sanford.Multimedia nuget packages you could find on Github, binaries may not match.
- Launch `XplorerEditor\Xplorer\bin\x86\Debug\xplorer.exe`
- **Read the manual** to configure Xplorer as required

4. Troobleshooting

Traces can be activated in xplorer.config file:
```
  <system.diagnostics>
    <switches>
      <add name="MidiApp.MidiController.Service.Logger" value="0" />
    </switches>
  </system.diagnostics>
  ```
  
  - value 0 = off, 4 = verbose.
  - logfile is located in :\ProgramData\Xplorer\Xplorer\applog.txt
  - MIDI-OX (http://www.midiox.com/) is your companion to debug MIDI messages
  


## Submit your contribution

1. Fork the repository
2. Create a new branch (`git checkout -b feature/YourFeature`)
3. Commit your changes (`git commit -m 'Add some feature'`)
4. Push to the branch (`git push origin feature/YourFeature`)
5. Open a Pull Request and describe your changes


## Credits

- C# MIDI Toolkit by Leslie Sanford (original source code: https://www.codeproject.com/Articles/6228/C-MIDI-Toolkit)


## License

This project is licensed under the GPL v3.

see https://github.com/xplorer2716/XplorerEditor/blob/main/LICENSE


# Links
- website: http://xplorer.programmer.free.fr
- former forum: http://xplorer.programmer.free.fr/bb/


